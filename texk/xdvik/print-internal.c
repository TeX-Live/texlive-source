/*
 * Copyright (c) 2002-2004 Paul Vojta and the xdvik development team
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * PAUL VOJTA OR ANY OTHER AUTHOR OF THIS SOFTWARE BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

/*
 * SU: Printing/saving backend code (all the non-GUI stuff),
 * adapted from non-k xdvi's popups.c.
 *
 * Implementation notes:
 *   All inter-process communication happens in an asynchronous
 *   fasion, so that the GUI remains responsive.  This means that if
 *   there are several processes in sequence (e.g. dvips invocation,
 *   then ps2pdf invocation), the latter must be called by the child
 *   watching procedure of the former (e.g. ps2pdf_exited by the
 *   fork_process() callback in call_ps2pdf()), else we can't catch its
 *   return status, do cleanup afterwards etc., because the code inside
 *   the caller may be executed *before* the child has finished. (Note
 *   e.g. how unlink()ing the temporary PS file is done inside
 *   ps2pdf_exited() and not in the callers). This is the reason why we
 *   often need to pass information to the subprocess via a data pointer.
 */

#include "xdvi-config.h"
#include "xdvi.h"
#include "dvisel.h" /* for select_pages_info struct */
#include "print-internal.h"
#include "print-log.h"
#include "search-internal.h"
#include "events.h"
#include "message-window.h"
#include "util.h"
#include "x_util.h"
#include "string-utils.h"
#include "my-snprintf.h"

#include <errno.h>
#include <signal.h>
#include <sys/stat.h>

/* if POSIX O_NONBLOCK is not available, use O_NDELAY */
#if !defined(O_NONBLOCK) && defined(O_NDELAY)
# define O_NONBLOCK O_NDELAY
#endif

#ifdef EWOULDBLOCK
# ifdef EAGAIN
#  define AGAIN_CONDITION	(errno == EWOULDBLOCK || errno == EAGAIN)
# else
#  define AGAIN_CONDITION	(errno == EWOULDBLOCK)
# endif
#else /* EWOULDBLOCK */
# ifdef EAGAIN
#  define AGAIN_CONDITION	(errno == EAGAIN)
# endif
#endif /* EWOULDBLOCK */

#if HAVE_POLL
# include <poll.h>
# define XIO_IN POLLIN
# define XIO_OUT POLLOUT
#else
# define XIO_IN 1
# define XIO_OUT 2
#endif /* HAVE_POLL */

static struct xchild print_child = {NULL, 0, "dvips", NULL, NULL, SIGKILL, NULL
				   };

static char *read_from_dvips(int, void *);

static struct xio print_xio = { NULL, 0, XIO_IN,
#if HAVE_POLL
				NULL,
#endif
				read_from_dvips,
				NULL, /* write proc - not needed */
				NULL  /* data */
};

static void dvips_alarm(struct xtimer *this, void *data);

static struct xtimer dvips_timer = {NULL, {0, 0}, XTM_DEFAULT, dvips_alarm, NULL
#if XDVI_XT_TIMER_HACK
				    , NULL, NULL
#endif
};

static int dvips_sig;	/* SIGINT or SIGKILL */

static int dvips_status;
static const int DVIPS_STAT_NONE = 0;
static const int DVIPS_STAT_RUN = 1;
static const int  DVIPS_STAT_WAIT = 2;

/*
  FIXME: implement this in terms of fork_process()!
  Currently it doesn't work since the DVI file and the
  output file are realized via redirecting stdin/stdout.
  This also obscures the log output (the I/O redirection
  isn't visible to users).
   
  TODO: Remove the detailed log output, and implement
  a progress window that only shows the page numbers.
  The detailed log should go to an `xdvi log output' page.
*/

static void
fork_dvips(char **argv, struct save_or_print_info *info, childProcT proc)
{
    int print_io[2];
    int i;
    struct file_info *finfo = info->finfo;
    /*     printlog_append(argv[0], strlen(argv[0])); */
    FILE *fout = NULL;

    if (info->act == FILE_SAVE || info->print_target == TO_FILE) { /* printing to PS file, open file for writing */
	const char *out_file;

	if (info->print_target == TO_FILE || info->fmt == FMT_PS)
	    out_file = finfo->out_file;
	else
	    out_file = finfo->tmp_ps_file;
    
	if ((fout = XFOPEN(out_file, "w")) == NULL) {
	    popup_message(globals.widgets.top_level,
			  MSG_ERR,
			  NULL, "Could not open %s for writing: %s.",
			  out_file,
			  strerror(errno));
	    return;
	}
    }

    printlog_popup(info);

    printlog_append_str(info, "Calling: `");
    printlog_append_str(info, argv[0]);

    for (i = 1; argv[i] != NULL; i++) {
	printlog_append_str(info, " ");
	printlog_append_str(info, argv[i]);
    }
    printlog_append_str(info, "'\n");
    
    if (xpipe(print_io) != 0) {
 	perror("[xdvi] pipe");
 	return;
    }
 
    /* Fork process */
    
    /* flush output buffers to avoid double buffering (i.e. data
       waiting in the output buffer being written twice, by the parent
       and the child) */
    fflush(stderr);
    fflush(stdout);

    print_child.name = xstrdup(argv[0]);
    print_child.proc = proc;
    print_child.data = info;
    print_child.pid = fork();
    if (print_child.pid == 0) {	/* if child */
	/* change into dir of DVI file so that included image files etc. are found */
	(void)chdir(globals.dvi_file.dirname);

	/* make the input file pointer the STDIN of the dvips process */
	if (info->page_selection == PAGE_MARKED) { /* printing selected pages from temporary file */
	    ASSERT(finfo->tmp_dvi_fp != NULL, "tmp fp mustn't be NULL!");
	    (void)dup2(fileno(finfo->tmp_dvi_fp), STDIN_FILENO);
	}
	else { /* printing from main or backup file */
	    (void)dup2(fileno(finfo->in_fp), STDIN_FILENO);
	}
	(void)lseek(0, 0, SEEK_SET);
	
 	if (fout != NULL) { /* printing to file, make stdout of child go to fout */
	    (void)dup2(fileno(fout), STDOUT_FILENO);
 	    (void)close(fileno(fout));
 	}
	else { /* printing to printer, make stdout of child go to print_io[1] */
 	    (void)dup2(print_io[1], STDOUT_FILENO);
 	}

	/* make stderr of child go to print_io[1] */
 	(void)dup2(print_io[1], STDERR_FILENO);
 	(void)close(print_io[1]);
 	(void)close(print_io[0]);
 
 	if (setsid() == -1) {	/* so we can kill the process group */
 	    perror("setsid");
 	    fflush(stderr);
 	    _exit(1);
 	}
 	(void)execvp(*argv, argv);
 	popup_message(globals.widgets.top_level,
		      MSG_ERR,
		      NULL,
		      "Execution of \"%s\" failed: %s.\n", *argv, strerror(errno));
 	fflush(stderr);
 	_exit(1);
    }
 
    if (fout != NULL)
 	fclose(fout);
 
    if (print_child.pid == -1) {	/* error */
 	perror("[xdvi] vfork");
 	return;
    }
 
    set_chld(&print_child);
    dvips_sig = SIGINT;

    (void)close(print_io[1]);
 
    /* Set up file descriptor for non-blocking I/O */
    prep_fd(print_io[0], True);
    print_xio.fd = print_io[0];
    print_xio.data = info;
    set_io(&print_xio);
 
    dvips_status = DVIPS_STAT_RUN;	/* running */
}

/*
 * Create an argument list for dvips, using information from info
 * and X resources. Returns the result list in freshly allocated
 * memory.
 */
static char **
create_dvips_argv(const struct save_or_print_info *info, Boolean do_pdf)
{
    const struct select_pages_info *pinfo = info->pinfo;

    size_t argv_len = 128; /* should be ample ... */
    char **argv = xmalloc(argv_len * sizeof *argv);
    size_t idx = 0;
    const char *printer_options = info->printer_options;
    const char *dvips_options = info->dvips_options;
    char from_page[LENGTH_OF_INT];
    char to_page[LENGTH_OF_INT];

    TRACE_GUI((stderr, "dvips options: |%s|", dvips_options));
    
    argv[idx++] = xstrdup(resource.dvips_path);

    if (dvips_options != NULL) {
	char **dvips_args = get_separated_list(dvips_options, " \t", True); /* this allocates dvips_args */
	int i;
	for (i = 0; dvips_args[i] != NULL; i++) {
	    argv[idx++] = dvips_args[i];
	}
	free(dvips_args);
    }

    if (do_pdf)
	argv[idx++] = xstrdup("-Ppdf");
    
    argv[idx++] = xstrdup("-f");
    
    if (info->print_target == TO_PRINTER && printer_options != NULL) { /* printing to printer */
	char **printer_args = get_separated_list(printer_options, " \t", True); /* this allocates printer_args */
	int i;
	ASSERT(*printer_args != NULL, "args should contain at least the string \"lpr\"");
	argv[idx] = xstrdup("-o!");
	argv[idx] = xstrcat(argv[idx], printer_args[0]);
	free(printer_args[0]);
	/* now append everything to the printer pipe command, so that dvips doesn't see it as an option */
	for (i = 1; printer_args[i] != NULL; i++) {
	    argv[idx] = xstrcat(argv[idx], " ");
	    argv[idx] = xstrcat(argv[idx], printer_args[i]);
	    free(printer_args[i]);
	}
	free(printer_args);
	idx++;
    }

    if (info->page_selection == PAGE_RANGE) {
	/* convert back from 0-based to 1-based, also taking globals.pageno_correct into accout
	 * (which is 1 by default, so we need to add 2) */
	argv[idx] = xstrdup("-p=");
	SNPRINTF(from_page, LENGTH_OF_INT, "%d", pinfo->from - globals.pageno_correct + 2);
	argv[idx] = xstrcat(argv[idx], from_page);
	idx++;
	argv[idx] = xstrdup("-l=");
	SNPRINTF(to_page, LENGTH_OF_INT, "%d", pinfo->to - globals.pageno_correct + 2);
	argv[idx] = xstrcat(argv[idx], to_page);
	idx++;
    }

    argv[idx] = NULL; /* terminate it */
    ASSERT(idx <= argv_len, "Too many elements");
    return argv;
}

/* Check if selecting pages worked, report error else */
static Boolean
select_pages_report_error(const struct save_or_print_info *info)
{
    if (info->pinfo->errflag == NO_ERROR)
	return False;
    
    popup_message(globals.widgets.top_level,
		  MSG_INFO,
		  NULL,
		  "Could not save DVI file to %s: %s.",
		  info->finfo->tmp_dvi_file,
		  get_dvi_error(info->pinfo->errflag));
    return True;
}


/*
 * Collect dvips output, appending to print log.
 * Currently return value is a dummy (NULL); TODO: return
 * the output, for later filtering.
 */
static char *
read_from_dvips(int ignored, void *data)
{
    int bytes;
    char line[80];
    struct save_or_print_info *info = data;

    UNUSED(ignored);
    
    for (;;) {
#ifndef MOTIF
	bytes = read(print_xio.fd, line, sizeof line);
#else
	bytes = read(print_xio.fd, line, sizeof line - 1);
#endif
	if (bytes < 0) {
	    if (AGAIN_CONDITION)
		break;
	    perror("xdvi: read_from_dvips");
	    break;
	}

	if (bytes == 0)
	    break;
	else {
#ifdef MOTIF
	    line[bytes] = '\0';
#endif
	    printlog_append(info, line, bytes);
	}
    }
    return NULL; /* TODO */
}

/*
 * Report dvips exit status, and remove temporary DVI file if needed.
 */
static void
dvips_exited(int exitval, struct xchild *child)
{
    char str[128] = "";
    int ms;

    struct save_or_print_info *info = (struct save_or_print_info *)child->data;
    
    read_from_dvips(0, info);
    clear_io(&print_xio);
    (void)close(print_xio.fd);

    if (WIFEXITED(exitval)) {
	if (WEXITSTATUS(exitval) == 0) {
	    if (info->act == FILE_SAVE || info->print_target == TO_FILE) {
		printlog_append_str(info, "\nCreated Postscript file ");
		printlog_append_str(info, info->finfo->out_file);
		printlog_append_str(info, ".\n");
	    }
	    else {
		printlog_append_str(info, "Done.\n");
	    }

	    /* remove temporary DVI file if it exists */
	    if (info->finfo->tmp_dvi_file != NULL) {
		if (globals.debug & DBG_GUI)
		    TRACE_GUI((stderr, "NOT removing temporary DVI file: |%s|", info->finfo->tmp_dvi_file));
		else {
		    if (info->finfo->tmp_dvi_fp != NULL) {
			fclose(info->finfo->tmp_dvi_fp);
			info->finfo->tmp_dvi_fp = NULL;
		    }
		    unlink(info->finfo->tmp_dvi_file);
		    info->finfo->tmp_dvi_file = NULL;
		}
	    }
	}
	else
	    sprintf(str, "\nDvips returned exit code %d.\n", WEXITSTATUS(exitval));
    }
    else if (WIFSIGNALED(exitval))
	sprintf(str, "\nDvips terminated by signal %d.\n", WTERMSIG(exitval));
    else
	sprintf(str, "\nDvips returned unknown status 0x%x.\n", exitval);

    ms = resource.dvips_hang;
    if (str[0] != '\0') {
	xdvi_bell();
	ms = resource.dvips_fail_hang;
	printlog_append_str(info, str);
    }

    if (ms > 0) {
	dvips_timer.data = info;
	set_timer(&dvips_timer, ms);
	dvips_status = DVIPS_STAT_WAIT;
    }
    else {
	dvips_status = DVIPS_STAT_NONE;
    }

    printlog_enable_closebutton(info);
}

/*
 * Report pd2pdf exit status, and remove temporary PS file if needed.
 */
static void
ps2pdf_exited(int status, struct xchild *this)
{
    char *err_msg = NULL;
    int ms = resource.dvips_hang;
    struct save_or_print_info *info = (struct save_or_print_info *)this->data;
    
    /* if child exited with error and xio struct is available for child,
       print error text */
    if (this->io != NULL && (WIFEXITED(status) != 0)) {
	if ((WEXITSTATUS(status) != 0)
	    && (err_msg = (this->io->read_proc)(this->io->fd, NULL)) != NULL) {
	    char buf[LENGTH_OF_INT];
	    SNPRINTF(buf, LENGTH_OF_INT, "%d", WEXITSTATUS(status));
	    ms = resource.dvips_fail_hang;
	    xdvi_bell();
	    printlog_append_str(info, "\n\nError calling ");
	    if (this->name != NULL) {
		printlog_append_str(info, "\"");
		printlog_append_str(info, this->name);
		printlog_append_str(info, "\" ");
	    }
	    printlog_append_str(info, "\nCommand exited with error code ");
	    printlog_append_str(info, buf);
	    printlog_append_str(info, ":\n");
	    printlog_append_str(info, err_msg);
	    free(err_msg);
	}
	else {
	    int retval;
	    printlog_append_str(info, "\nCreated PDF file ");
	    printlog_append_str(info, info->finfo->out_file);
	    printlog_append_str(info, ".\n");
	    TRACE_FILES((stderr, "Removing temporary PS file: `%s'",
			 info->finfo->tmp_ps_file));
	    retval = unlink(info->finfo->tmp_ps_file);
	    if (retval != 0) {
		fprintf(stderr, "Could not unlink `%s': %s.\n",
			info->finfo->tmp_ps_file, strerror(errno));
	    }
	}
    }
    else {
	popup_message(globals.widgets.top_level,
		      MSG_ERR, REPORT_XDVI_BUG_TEMPLATE,
		      "Internal error: ps2pdf_exited() called while child still running?");
    }
    if (ms > 0) {
	dvips_timer.data = info;
	set_timer(&dvips_timer, ms);
	dvips_status = DVIPS_STAT_WAIT;
    }
    else {
	dvips_status = DVIPS_STAT_NONE;
    }

    printlog_enable_closebutton(info);
    
    free(this->name);
    free(this->io);
    free(this);
}

static void
call_ps2pdf(const char *path, const struct save_or_print_info *info)
{
    const char *argv[5];
    size_t idx = 0;

    argv[idx++] = path;
    argv[idx++] = info->finfo->tmp_ps_file;
    argv[idx++] = info->finfo->out_file;
    argv[idx++] = NULL;

    /* need to run this in globals.xdvi_dir again, since the dvips conversion directory
       globals.dvi_file.dirname may not be writable! */
    if (!fork_process("ps2pdf", True, globals.cwd, ps2pdf_exited, (void *)info,
      0, (char **)argv)) {
	popup_message(globals.widgets.top_level,
		      MSG_ERR,
		      NULL, "Couldn't fork %s process: %s\n", argv[0], strerror(errno));
    }
}

static void
dvips_ps2pdf(int exitval, struct xchild *child)
{
    char str[128] = "";
    int ms;
    struct save_or_print_info *info = (struct save_or_print_info *)child->data;
    
    read_from_dvips(0, info);
    clear_io(&print_xio);
    (void)close(print_xio.fd);

    if (WIFEXITED(exitval)) {
	if (WEXITSTATUS(exitval) == 0) {
	    /* dvips ended OK; call ps2pdf: */
	    TRACE_GUI((stderr, "Created temporary PS file |%s|", info->finfo->tmp_ps_file));
	    printlog_append_str(info, "\nCalling ");
	    printlog_append_str(info, resource.ps2pdf_path);
	    printlog_append_str(info, " ...");

	    /* remove temporary DVI file if it exists */
	    if (info->finfo->tmp_dvi_file != NULL) {
		if (globals.debug & DBG_GUI)
		    TRACE_GUI((stderr, "NOT removing temporary DVI file: |%s|", info->finfo->tmp_dvi_file));
		else {
		    if (info->finfo->tmp_dvi_fp != NULL) {
			fclose(info->finfo->tmp_dvi_fp);
			info->finfo->tmp_dvi_fp = NULL;
		    }
		    unlink(info->finfo->tmp_dvi_file);
		    info->finfo->tmp_dvi_file = NULL;
		}
	    }
	    /* invoke ps2pdf conversion */
	    call_ps2pdf(resource.ps2pdf_path, info);
	}
	else
	    sprintf(str, "\nPrint process returned exit code %d.\n",
		    WEXITSTATUS(exitval));
    }
    else if (WIFSIGNALED(exitval))
	sprintf(str, "\nPrint process terminated by signal %d.\n",
		WTERMSIG(exitval));
    else
	sprintf(str, "\nPrint process returned unknown status 0x%x.\n",
		exitval);

    /* enable close button only if dvips conversion already failed */
    ms = resource.dvips_hang;
    if (str[0] != '\0') {
	xdvi_bell();
	ms = resource.dvips_fail_hang;
	printlog_append_str(info, str);

	if (ms > 0) {
	    dvips_timer.data = info;
	    set_timer(&dvips_timer, ms);
	    dvips_status = DVIPS_STAT_WAIT;
	}
	else {
	    dvips_status = DVIPS_STAT_NONE;
	}
	
	printlog_enable_closebutton(info);
    }
}


static void
dvips_alarm(struct xtimer *this, void *data)
{
    struct save_or_print_info *info = (struct save_or_print_info *)data;
    UNUSED(this);

    printlog_popdown(info, False);
    
    dvips_status = DVIPS_STAT_NONE;
}



static void
cb_dvips_keep(Widget w, XtPointer client_data, XtPointer call_data)
{

    UNUSED(w);
    UNUSED(client_data);
    UNUSED(call_data);
    
    /* use negative value so that we don't loose the value itself */
    TRACE_GUI((stderr, "toggling resource from %d to %d, %d to %d",
	       resource.dvips_hang, -resource.dvips_hang,
	       resource.dvips_fail_hang, -resource.dvips_fail_hang));
    resource.dvips_hang = -resource.dvips_hang;
    resource.dvips_fail_hang = -resource.dvips_fail_hang;

    store_preference(NULL, "dvipsHangTime", "%d", resource.dvips_hang);
    store_preference(NULL, "dvipsFailHangTime", "%d", resource.dvips_fail_hang);

    
    if (dvips_status == DVIPS_STAT_WAIT) {
	dvips_status = DVIPS_STAT_NONE;
	cancel_timer(&dvips_timer);
    }
}

/* if user selects the window manager destroy button */
static void
cb_dvips_destroy(Widget w, XtPointer client_data, XtPointer call_data)
{
    struct save_or_print_info *info = (struct save_or_print_info *)client_data;
    
    UNUSED(w);
    UNUSED(call_data);

    if (dvips_status == DVIPS_STAT_RUN) {
	kill(print_child.pid, dvips_sig);
	dvips_sig = SIGKILL;
	printlog_append_str(info, "^C");
    }

    if (dvips_status == DVIPS_STAT_WAIT) {
	dvips_status = DVIPS_STAT_NONE;
	cancel_timer(&dvips_timer);
    }

    printlog_reset(info);
    printlog_popdown(info, True);
}
    
static void
cb_dvips_cancel(Widget w, XtPointer client_data, XtPointer call_data)
{
    struct save_or_print_info *info = (struct save_or_print_info *)client_data;
    
    UNUSED(w);
    UNUSED(call_data);

    if (dvips_status != DVIPS_STAT_RUN)
	return;	/* How did we get here? */

    kill(print_child.pid, dvips_sig);
    dvips_sig = SIGKILL;
    printlog_append_str(info, "^C");
}

static void
cb_dvips_close(Widget w, XtPointer client_data, XtPointer call_data)
{
    struct save_or_print_info *info = (struct save_or_print_info *)client_data;
    
    UNUSED(w);
    UNUSED(call_data);

    if (dvips_status == DVIPS_STAT_RUN)
	return;	/* How did we get here? */

    if (dvips_status == DVIPS_STAT_WAIT) {
	dvips_status = DVIPS_STAT_NONE;
	cancel_timer(&dvips_timer);
    }

    printlog_popdown(info, True);
}


void
internal_save(struct save_or_print_info *info)
{
    struct file_info *finfo = info->finfo;
    static struct callback_info cinfo = {
	cb_dvips_close, cb_dvips_cancel, cb_dvips_destroy, cb_dvips_keep
    };
    int tmp_fd;
    char **argv = NULL;
    int i;
    childProcT dvips_exit_proc = dvips_exited; /* default procedure to call after fork_dvips() */
    Boolean do_pdf = False; /* force `-Ppdf' for dvips? (used for ps2pdf conversion) */

    ASSERT(info->fmt != FMT_NONE, "No valid output format selected!");

    /* fprintf(stderr, "INTERNAL SAVE - format: %d; selection: %d\n", info->fmt, info->page_selection); */
    
    info->callbacks = &cinfo;
    
    switch (info->fmt) {
	
    case FMT_DVI:
	/* here we first create a temporary file, and if that succeeded,
	   move it to the final position */
	select_pages(info);

	if (!select_pages_report_error(info)) {
	    /*
	     * ... else, move the temporary file to its final destination.  For that, we
	     * try the more efficient rename() first; if this fails, try copying the file
	     * (rename() will e.g. fail if the tmp directory is on a different file
	     * system).  We could perhaps examine errno after the renaming attempt to
	     * find out whether we need to do the copy attempt in the first place, but
	     * that'd be rather error-prone ...  */
	    ASSERT(finfo->tmp_dvi_file != NULL, "filename mustn't be NULL");
	    if (rename(finfo->tmp_dvi_file, finfo->out_file) != 0
		&& !copy_file(finfo->tmp_dvi_file, finfo->out_file)) {
		popup_message(globals.widgets.top_level,
			      MSG_ERR, NULL, "Creating %s failed: %s",
			      finfo->out_file, strerror(errno));
	    }
	    else { /* moving worked */
		TRACE_GUI((stderr, "renamed %s to %s\n", finfo->tmp_dvi_file, finfo->out_file));

		if (info->pinfo->warn_files.stack_len > 0) { /* do we need to warn user about referenced files? */
		    char *warn_files = xstrdup("");
		    size_t i;
		    for (i = 0; i < info->pinfo->warn_files.stack_len; i++) {
			warn_files = xstrcat(warn_files, info->pinfo->warn_files.items[i].content);
			warn_files = xstrcat(warn_files, "\n");
		    }
		    popup_message(globals.widgets.top_level,
				  MSG_INFO,
				  "The files listed in the message (probably images) are not part "
				  "of the newly created DVI file; they are only referenced by that file. "
				  "Therefore, if you want to distribute this file or use it on a different computer,"
				  "you will need to distribute all the referenced files, too.\n\n"
				  "Tip: A better format for distributing files is Postscript or PDF. "
				  "To create a Postscript file, use the `File->Print' menu, "
				  "then select `Print to file' in the dialog window.",
				  "Created %s.\n"
				  "Please note that the following files are referenced by this file, "
				  "and are needed for displaying or printing it correctly:\n%s\n",
				  finfo->out_file, warn_files);
		    free(warn_files);
		}
		else {
/* 		    char *ptr = strrchr(finfo->out_file, '/'); */
/* 		    if (ptr == NULL) */
/* 			ptr = finfo->out_file; */
/* 		    else */
/* 			ptr++; */
		    popup_message(globals.widgets.top_level,
				  MSG_INFO,
				  NULL,
				  "Created DVI file:\n%s.", finfo->out_file);
		}
		if (info->finfo->tmp_dvi_fp != NULL) {
		    fclose(info->finfo->tmp_dvi_fp);
		    info->finfo->tmp_dvi_fp = NULL;
		}
		unlink(finfo->tmp_dvi_file);
		info->finfo->tmp_dvi_file = NULL;
	    }
	}
	break;

    case FMT_PS2PDF:
	free(finfo->tmp_ps_file);
	finfo->tmp_ps_file = NULL;
	tmp_fd = xdvi_temp_fd(&(finfo->tmp_ps_file)); /* this allocates finfo->tmp_ps_file */
	if (tmp_fd == -1) {
	    popup_message(globals.widgets.top_level,
			  MSG_ERR, NULL,
			  "Couldn't create temporary PS file for conversion to PDF: %s",
			  strerror(errno));
	    return;
	}
	else {
	    TRACE_GUI((stderr, "name of temporary PS file: |%s|", finfo->tmp_ps_file));
	}
	dvips_exit_proc = dvips_ps2pdf; /* call ps2pdf conversion after dvips exited */
	do_pdf = True;
	/* fall through */
    case FMT_PS:
	if (info->page_selection == PAGE_MARKED) { /* want to save selected pages? */
	    ASSERT(info->pinfo->callback != NULL, "Callback musn't be NULL!");
	    ASSERT(finfo->tmp_dvi_fp != NULL, "Temporary file pointer musn't be NULL!");
	    ASSERT(finfo->tmp_dvi_file != NULL, "Temporary filename musn't be NULL!");
	    select_pages(info);
	}
	if (info->printlog == NULL)
	    printlog_create(info, "Xdvik: Saving", "Automatically close this window after file has been saved");
	else
	    printlog_enable_cancelbutton(info);
	
	if ((argv = create_dvips_argv(info, do_pdf)) == NULL) {
	    /* something went *really* wrong; assume user has already been warned about it */
	    return;
	}
	fork_dvips(argv, info, dvips_exit_proc);

	/* dellocate argv */
	for (i = 0; argv[i] != NULL; i++) {
	    free(argv[i]);
	}
	free(argv);

	break;
    case FMT_ISO_8859_1:
    case FMT_UTF8:
	if (search_extract_text(info)) {
	    popup_message(globals.widgets.top_level,
			  MSG_INFO,
			  NULL,
			  "Created text file %s.", finfo->out_file);
 	}
 	else {
 	    popup_message(globals.widgets.top_level,
			  MSG_INFO,
 			  NULL,
 			  "Extracting text from DVI file failed!");
 	}
	break;
    case FMT_NONE:
	break;
    }
}

void
internal_print(struct save_or_print_info *info)
{
    static struct callback_info cinfo = {
	cb_dvips_close, cb_dvips_cancel, cb_dvips_destroy, cb_dvips_keep
    };
    char **argv = NULL;
    int i;

    info->callbacks = &cinfo;
    
    if (info->page_selection == PAGE_MARKED) { /* want to print selected pages? */
	ASSERT(info->pinfo->callback != NULL, "Callback musn't be NULL!");
	ASSERT(info->finfo->tmp_dvi_fp != NULL, "Temporary file pointer musn't be NULL!");
	ASSERT(info->finfo->tmp_dvi_file != NULL, "Temporary filename musn't be NULL!");
	select_pages(info);
    }
    if (info->printlog == NULL)
	printlog_create(info, "Xdvik: Printing", "Automatically close this window when printing finishes");
    else
	printlog_enable_cancelbutton(info);
	
    if ((argv = create_dvips_argv(info, False)) == NULL) {
	/* something went *really* wrong; assume user has already been warned about it */
	return;
    }

    fork_dvips(argv, info, dvips_exited);
    
    /* dellocate argv */
    for (i = 0; argv[i] != NULL; i++) {
	free(argv[i]);
    }
    free(argv);
}
