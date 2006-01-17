/* wwwfetch.c */
#define HAVE_BOOLEAN
#include "xdvi-config.h"
#if defined(HTEX) || defined(XHDVI)
#include <kpathsea/c-fopen.h>
#include <kpathsea/c-stat.h>

#define LINE 1024

#ifdef WIN32
#include "winhtex.h"
#else
#include "wwwconf.h"
#include "WWWLib.h"
#include "WWWInit.h"
#include "WWWCache.h"

#if 0	/* seems unnecessary?! */
#ifdef HAVE_NETDB_H
#include <netdb.h> /* for struct hostent */
#endif
#endif	/* seems unnecessary?! */

/* Given absolute URL, open a temporary filename, and do the transfer */
int
www_fetch(url, savefile)
char *url;
char *savefile;
{
        int status;
	HTRequest * request; 
	HTFormat content_type;

#if 0	/* done at startup in main() in xdvi.c */
	HTProfile_newPreemptiveClient(HTAppName, HTAppVersion);
	HTCacheMode_setEnabled(NO);
#endif
 	/* Don't ask when overwriting temporary savefile. */
	HTAlert_setInteractive(NO);

	request = HTRequest_new();
	status = HTLoadToFile(url, request, savefile);

	/* Extract the content_type before deleting the request. */
	if (debug & DBG_HYPER) {
	    /* content_type = request->response->content_type; */
	    content_type = HTResponse_format(HTRequest_response(request));

	    if (content_type == HTAtom_for("application/x-dvi"))
	        fprintf(stderr, "www_fetch(%s->%s) returned a dvi file.\n", 
			url, savefile);
	    else
		fprintf(stderr, "www_fetch(%s->%s) returned content-type: %s\n", 
			url, savefile, HTAtom_name(content_type));
	}
	HTRequest_delete(request);

#if 0	/* done in cleanup_and_exit() in hypertex.c */
	HTCache_flushAll();
	HTProfile_delete();
#endif

#ifdef DOFORKS
	exit(1); /* No cleanup! */
#else /* DOFORKS */
	return status;		/* return status: YES (1) or NO (0) */
#endif /* DOFORKS */
}
#endif /* !WIN32 */

/* Turn a relative URL into an absolute one: */
void
make_absolute(rel, base, len)
    char *rel, *base;
    int len;
{
        char *cp, *parsed;

	if (base == NULL) return;
        cp = strchr(rel, '\n');
        if (cp) *cp = '\0'; /* Eliminate newline char */

        parsed = HTParse(rel, base, PARSE_ALL);
        strncpy(rel, parsed, len);
        free(parsed);
}

/* buf is a URL - savefile is local file name to save the file in */
static void
make_tmp_filename(tmpdir, buf, savefile)
    char *tmpdir, *buf, **savefile;
{
	char *cp, *cp2;
	int i, j, fd;

	*savefile = NULL;
	cp = strrchr(buf, '/');
	if (cp == NULL) cp = strrchr(buf, ':');
	if (cp == NULL) return; /* Unsuccessful... */
	if (strlen(cp) < 3) cp = "/tmpfile";
	cp++;
	*savefile = (char *) malloc((strlen(cp) + strlen(tmpdir)+3)*sizeof(char));
	cp2 = strrchr(cp, '.'); /* file extension */
	if (cp2 != NULL) {
		*cp2 = '\0';
		cp2++;
		for (i=0; i < 26; i++) { /* Attempt to create new file */
			sprintf(*savefile,"%s/%s%c.%s", tmpdir,cp,'a'+i,cp2);
			for (j=0; j < nURLs; j++) {
				if (!strcmp(*savefile, filelist[j].file)) break;
			}
			if (j < nURLs) continue;
			if ((fd = creat(*savefile,0600)) >= 0) {
				close(fd);
				break;
			}
		}
		cp2--;
		*cp2 = '.'; /* Undo the damage we just did to buf... */
	} else {
		for (i=0; i < 26; i++) { /* Attempt to create new file */
			sprintf(*savefile,"%s/%s%c", tmpdir,cp,'a'+i);
			for (j=0; j < nURLs; j++) {
				if (!strcmp(*savefile, filelist[j].file)) break;
			}
			if (j < nURLs) continue;
			if ((fd = creat(*savefile,0600)) >= 0) {
				close(fd);
				break;
			}
		}
	}
	if (i == 26) { /* Unsuccessful */
		free(*savefile);
		*savefile = NULL;
	}
}


typedef struct {
	char *url;
	char *savefile;
	pid_t childnum;
} Fetch_Children;

#define MAXC 10 /* Don't send off more than 10 simultaneously */
Fetch_Children fetch_children[MAXC];
static int nchildren = MAXC+1;

void
wait_for_urls() /* Wait for all the children to finish... */
{
	int i;

	if (nchildren > MAXC) {
	    /* Initialization needed: */
	    for (i=0; i < MAXC; i++) {
		fetch_children[i].url = NULL;
		fetch_children[i].savefile = NULL;
	    }
	} else {
	    for (i=0; i < nchildren; i++) {
#ifdef DOFORKS
		ret = wait(&status); /* Wait for one to finish */
		for (j=0; j < nchildren; j++) {
		    if (ret == fetch_children[j].childnum) {
			if (debug & DBG_HYPER) 
			  fprintf(stderr,"wait_for_urls(): URL %s in file %s: status %d\n",
				  fetch_children[j].url,
				  fetch_children[j].savefile,
				  status);
			break;
		    }
		}
#else /* DOFORKS */
		if (debug & DBG_HYPER) 
		  fprintf(stderr, "wait_for_urls(): URL %s in file %s\n",
			  fetch_children[i].url,
			  fetch_children[i].savefile);
#endif /* DOFORKS */
	    }
	}
	nchildren = 0;
}

/* Start a fetch of a relative URL */
int
fetch_relative_url(base_url, rel_url, tmpdir)
    char *base_url;
    const char *rel_url;
    char *tmpdir;
{
	int i, resp;
	char *savefile;
	char *cp;
	char buf[LINE];
	FILE *fp;

	/* Step 1: make the relative URL absolute: */
        strncpy(buf, rel_url, LINE); /* Put it in buf */
	make_absolute(buf, base_url, LINE);

	/* Step 1.5: Check whether we already have it - if so return */
	for (i=0; i < nURLs; i++) {
		if (!strcmp(buf, filelist[i].url)) return(i);
	}
	
	/* Step 2: Find a temporary file to store the output in */
	make_tmp_filename(tmpdir, buf, &savefile);
	if (savefile == NULL) {
		fprintf(stderr, "Could not find temporary file for %s in %s\n",
			buf, tmpdir);
		return(-1);
	}	
	if ((fp = fopen(savefile, FOPEN_W_MODE)) == NULL) {
	        fprintf(stderr, "Cannot open %s for writing\n", savefile);
		return(-1);
	} else {
		fclose(fp); unlink(savefile);
	}

	/* Step 3: Fork to fetch the URL */
	if (nchildren >= MAXC) wait_for_urls(); /* Wait for the old ones */
	cp = NULL;
	fetch_children[nchildren].url = MyStrAllocCopy(&cp, buf);
	cp = NULL;
	fetch_children[nchildren].savefile = MyStrAllocCopy(&cp, savefile);

	/* Step 4: Update the URL-filename list */
	if (nURLs == 0) {
		filelist = xmalloc(FILELISTCHUNK*sizeof(FiletoURLconv));
		bzero(filelist,  FILELISTCHUNK*sizeof(FiletoURLconv));
	} else if (nURLs%FILELISTCHUNK == 0) {
		filelist = (FiletoURLconv *) 
		  realloc(filelist, (nURLs+FILELISTCHUNK)*sizeof(FiletoURLconv));
		bzero(filelist + nURLs, FILELISTCHUNK*sizeof(FiletoURLconv));
	}
	MyStrAllocCopy(&(filelist[nURLs].url), buf);
	MyStrAllocCopy(&(filelist[nURLs].file), savefile);
	nURLs++;
#ifdef DOFORKS
	fetch_children[nchildren].childnum = fork();
	if (fetch_children[nchildren].childnum == 0) {  /* Child process */
		www_fetch(buf, savefile); /* Get the URL! */
		exit(0); /* Make sure this process quits... */
	}
	nchildren++;
#else /* DOFORKS */
	resp = www_fetch(buf, savefile); /* Get the URL! */
	if (resp == 0) {
#if 0 
                /* Don't call paint_anchor before window was opened! */
		paint_anchor("Error: Cannot locate URL: %s\n", buf);
#endif
	        fprintf(stderr, "Cannot locate URL: %s\n", buf);
		nURLs--;
		unlink(filelist[nURLs].file); /* Get rid of that temp file */
		return(-1);
	} else {
		nchildren++;
	}
#endif /* DOFORKS */
	return(nURLs-1);
}

#endif /* HTEX || XHDVI */
