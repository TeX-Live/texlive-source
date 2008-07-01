/* routines for launching a browser to retrieve remote documents.
 * Copyright(C) 2002-2004 the xdvik development team.
 */
/*
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
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL PAUL VOJTA OR ANY OTHER AUTHOR OF THIS SOFTWARE BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include "xdvi-config.h"
#include "xdvi.h"

#include <ctype.h>
#include "kpathsea/c-fopen.h"
#include "kpathsea/c-stat.h"

#include <sys/types.h>
#include <sys/wait.h> /* for waitpid(), WEXITSTATUS */

#include "util.h"
#include "message-window.h"
#include "events.h"
#include "browser.h"
#include "string-utils.h"
#include "statusline.h"

static const char *const default_browser_cmd =
"xdg-open %s"
":htmlview %s"
":firefox -remote \"openURL(%s,new-window)\""
":mozilla -remote \"openURL(%s,new-window)\""
":netscape -remote \"openURL(%s,new-window)\""
":xterm -e w3m %s"
":xterm -e lynx %s"
":xterm -e wget %s";

static Boolean
do_fork_browser(char *argv[])
{
    pid_t pid;
    switch(pid = vfork()) {			   
    case -1: /* forking error */
	perror("fork");
	return False;
    case 0: /* child */
	execvp(argv[0], argv);
	/* arrive here only if execvp failed */
 	XDVI_ERROR((stderr, "Execution of %s failed: %s", argv[0], strerror(errno)));
	_exit(EXIT_FAILURE);
	return False; /* notreached - make compiler happy */
    default: /* parent */
	{
	    int timeout;
	    for (timeout = 0; timeout < 15; timeout++) {
		int status;
		if (waitpid(pid, &status, WNOHANG)) {
		    TRACE_HTEX((stderr, "waiting for %d: %d", (int)pid, status));
		    if (WIFEXITED(status)) {
			if (WEXITSTATUS(status) != 0) {
			    fprintf(stderr, "Command `%s' exited with error status %d\n",
				    argv[0], WEXITSTATUS(status));
			    return False;
			}
			else {
			    TRACE_HTEX((stderr, "Child exited OK."));
			    return True;
			}
		    }
		    else { /* when do we arrive here?? */
			sleep(1);
		    }
		}
		else { /* waiting for child to finish */
		    sleep(1);
		}
	    }
	    return True; /* child not finished in time */
	}
    }
}

static Boolean
fork_browser(const char *browser, const char *url)
{
    char *cmd, *orig_cmd, *last_arg;
    char **argv = NULL;
    int argv_len = 0;
    int i = 0, j = 0;
    int match = 0;
    int retval;
    

    cmd = xmalloc(strlen(browser) + strlen(url) + 1);
    orig_cmd = cmd; /* for freeing it later */
    last_arg = cmd;

    /* skip over leading space */
    while (isspace((int)browser[i]))
	i++;

    while (browser[i] != '\0') {
	while (j >= argv_len) {
	    argv_len += 10;
	    argv = xrealloc(argv, argv_len * sizeof *argv);
	}
	/* chop into separate arguments at spaces */
	if (isspace((int)browser[i])) {
	    *cmd++ = '\0';
	    argv[j++] = format_arg(last_arg, url, &match);
	    last_arg = cmd;
	    /* skip over multiple spaces */
	    while (isspace((int)browser[i]))
		i++;
	}
	/* remove quotes around arguments containing spaces */
	else if (browser[i] == '\'' || browser[i] == '"') {
	    int len = 0;
	    /* normalize %% and replace %s by URL */
	    argv[j++] = unquote_arg(browser+i, url, &match, &len);
	    if (len == 0) { /* excess quote at end of arg; try to recover: */
		j--;
		break;
	    }
	    i += len + 1;
	}
	else {
	    *cmd++ = browser[i++];
	}
    }
    *cmd = browser[i]; /* null-teminate */
    /* append last, unless it contained only skipped spaces */
    if (strlen(last_arg) > 0) {
	argv[j++] = format_arg(last_arg, url, &match);
    }
    
    if (match == 0)
	argv[j++] = xstrdup(url);
    argv[j++] = NULL;

    for (i = 0; argv[i] != NULL; i++) {
	TRACE_HTEX((stderr, "arg[%d]: |%s|", i, argv[i]));
    }

    /* This will wait for child exits: */
    retval = do_fork_browser(argv);
    for (i = 0; argv[i] != NULL; i++)
	free(argv[i]);
    free(orig_cmd);
    free(argv);
    return retval;
}

void
launch_browser(const char *filename)
{
    const char *browser;
    int pid;
    struct xchild *my_child;
    struct xio *my_io;
    int err_pipe[2];
    
    /* try to set it from possible resources: First command-line argument
       or X resource, then environment variables, then default_browser_cmd
    */
    for (;;) {
	if ((browser = resource.browser) != NULL)
	    break;
	if ((browser = getenv("BROWSER")) != NULL)
	    break;
	if ((browser = getenv("WWWBROWSER")) != NULL)
	    break;
	XDVI_INFO((stderr, "Browser not set (xdvi.wwwBrowser, -browser or $BROWSER environment variable)."));
	XDVI_INFO((stderr, "Using built-in default: `%s'",
		   default_browser_cmd));
	browser = default_browser_cmd;
	break;
    }
    
    /* fork first time so that we can wait for children, without
       freezing the GUI.  FIXME: this copies stuff from
       fork_process for the inter-process communication stuff -
       it would be better to have this in one function. */
    my_child = xmalloc(sizeof *my_child);
    my_io = xmalloc(sizeof *my_io);
    statusline_info(STATUS_MEDIUM, "Trying to launch browser ...");
    /* flush output buffers to avoid double buffering (i.e. data
       waiting in the output buffer being written twice, by the parent
       and the child) */
    fflush(stdout);
    fflush(stderr);
	
    if (pipe(err_pipe) < 0) {
	perror("pipe");
	_exit(-1);
    }
	
    switch (pid = fork()) {
    case -1:	/* forking error */
	perror("fork");
	close(err_pipe[0]);
	close(err_pipe[1]);
	return;
    case 0:	/* child */
	{
	    char *tmp_browser = xstrdup(browser);

	    close(err_pipe[0]);	/* no reading from stderr */
		
	    /* make stderr of child go to err_pipe[1] */
	    if (dup2(err_pipe[1], STDERR_FILENO) != STDERR_FILENO) {
		perror("dup2 for stderr");
		_exit(EXIT_FAILURE);
		return;	/* make compiler happy */
	    }
		
	    /*
	      BROWSER is a colon-separated list of commands, in decreasing preference;
	      use the first that can be forked successfully. Note that the return
	      value of the command isn't used at all (with GUI programs, xdvi shouldn't
	      hang until they terminate!)
	    */
	    while (tmp_browser != NULL) {
		char *next = strchr(tmp_browser, ':');
		if (next != NULL) {
		    *next++ = '\0';
		}
		TRACE_HTEX((stderr, "trying browser |%s|", tmp_browser));
		/* fork a second time to start the browser */
		if (fork_browser(tmp_browser, filename)) { /* foking worked */
		    _exit(EXIT_SUCCESS);
		}
		tmp_browser = next;
	    }
	    /* child arrives here only if none of the commands worked */
	    XDVI_WARNING((stderr, "None of the browser commands in the `browser' resource (%s) worked\n",
			  browser));
	    free(tmp_browser);
	    _exit(EXIT_FAILURE);
	}
    default: /* parent */
	close(err_pipe[1]);	/* no writing to stderr */

	my_io->next = NULL;
	my_io->fd = err_pipe[0];
	my_io->xio_events = XIO_IN;
#if HAVE_POLL
	my_io->pfd = NULL;
#endif
	my_io->read_proc = read_child_error;
	my_io->write_proc = NULL;
	my_io->data = NULL;
	    
	my_child->next = NULL;
	my_child->pid = pid;
	my_child->name = NULL;
	my_child->proc = handle_child_exit;
	my_child->data = NULL;
	my_child->io = my_io;
	    
	set_chld(my_child);
	statusline_info(STATUS_MEDIUM, "Trying to launch browser ... done.");
    }
}


