/*
# epstopdf: written by Kong Hoon Lee konghoon@hyowon.cc.pusan.ac.kr<4/1/1999>
#
# It converts an EPS file to an encapsulated PDF File and
# coded with the perl script 'epstopdf' by Sebastian Rahtz on
# http://tug.org/applications/pdftex/epstopdf .
# It works like as the perl script without 'perl' for Windows 95
# but requires 'Ghostscript' for Windows.
#
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#define MAX_IN 10000

#define DEBUG 1

#ifdef FPTEX

#include <win32lib.h>
#include <gs32lib.h>

/* This is the callback function for gs. It is mainly used to read and
  write  data on   gs   stdin/stdout. Data exchanges   happen  through
  buffers.  */
int __cdecl
gsdll_callback(int message, char *str, unsigned long count)
{
  char *p;

  switch (message) {

  case GSDLL_STDIN:
    /* Put count chars on gs stdin */
#if DEBUG
    fprintf(stderr, "gs wants %d chars\n", count);
#endif
    p = fgets(str, count, stdin);
    if (p)
      return strlen(str);
    else
      return 0;

  case GSDLL_STDOUT:
    fwrite(str, 1, count, stdout);
    return count;

  case GSDLL_DEVICE:
    /* gs_device = str; */
#if DEBUG
    fprintf(stdout,"Callback: DEVICE %p %s\n", str,
	    count ? "open" : "close");
#endif
    break;

  case GSDLL_SYNC:
#if DEBUG
    fprintf(stdout,"Callback: SYNC %p\n", str);
#endif
    break;

  case GSDLL_PAGE:
    fprintf(stdout,"Callback: PAGE %p\n", str);
    break;

  case GSDLL_SIZE:
#if DEBUG
    fprintf(stdout,"Callback: SIZE %p width=%d height=%d\n", str,
	    (int)(count & 0xffff), (int)((count>>16) & 0xffff) );
#endif
    break;

  case GSDLL_POLL:
#if 0
    fprintf(stderr, "GS: Poll sent (%d)!\n", 0);
#endif
    return 0; /* no error ? */
  default:
    fprintf(stdout,"Callback: Unknown message=%d\n",message);
    break;
  }
  return 0;
}

#undef BEGINDOC
#undef ENDDOC

#define GSEXEC "gswin32c.exe"

#endif /* FPTEX */

#ifndef GSEXEC
#ifdef __WIN32__
#define GSEXEC "gswin32c"
#else
#define GSEXEC "gs"
#endif
#endif

#define BEGINDOC "%%BeginDocument"
#define ENDDOC   "%%EndDocument"


static void usage(void)

{
  printf("epstopdf 2.26:\n");
  printf("written by Kong Hoon Lee, konghoon@dreamwiz.com <4/1/1999>\n");
  printf("changes by Juergen Bausa, bausa@lfpt.rwth-aachen.de <5/25/2000>\n");
  printf("bugfix by Pascal Perichon, Pascal.Perichon@u-bourgogne.fr <5/25/2000>\n\n");
  printf("It converts an EPS file to an encapsulated PDF File and is written\n");
  printf("based on the perl script 'epstopdf' by Sebastian Rahtz on\n");
  printf("http://tug.org/applications/pdftex/epstopdf .\n");
  printf("It works like the perl script without 'perl' but requires 'Ghostscript'.\n");
  printf("The accompanied Makefile can be used to automate the update of all eps/pdf\n");
  printf("files in a directory. Just put it in the directory where your eps files\n");
  printf("are and type 'make' from the command line (you will need a version of\n");
  printf("GNU-make)\n\n");

  printf("This program invokes '%s' and the path including '%s'\n",GSEXEC,GSEXEC);
  printf("should be included to the environment variable 'PATH'.\n");
  printf("'%s' should know, where to find its initialization files\n",GSEXEC);
  printf("and fonts, using an registry entry (Windows) or an environment variable.\n\n");

  printf("Using a different output device, it is also possible to convert eps\n");
  printf("files to bitmaps (e.g. -sDEVICE=bmpmono).\n\n");

  printf("Usage: epstopdf [options] filename-of-an-eps-file\n");
  printf("Options:\n");
  printf("  --help:             print usage\n");
  printf("  --outfile=<file>:   write result to <file>\n");
  printf("  --tmpfile=<file>:   use <file> as temporary file\n");
  printf("  --(no)filter:       read/writ standard input/output (default: false)\n");
  printf("  --(no)gs:           run ghostscript                 (default: true)\n");
  printf("  --(no)compress:     use compression                 (default: true)\n");
  printf("  --(no)hires:        scan HiresBoundingBox           (default: false)\n");
  printf("  --(no)exact:        scan ExactBoundingBox           (default: false)\n");
  printf("  --(no)debug:        debug informations              (default: false)\n");
  printf("  --(no)wait:         wait for keystroke              (default: false)\n");
  printf("  --gsexec=<gs>:      use <gs> to invoke ghostscript  (default: %s)\n",GSEXEC);
  printf("  --enlarge=<a>:      enlarge BB by <a>/72 ''         (default: 0.)\n");
  printf("  -sDEVICE=<dev> :    use <dev> as output device      (default: pdfwrite)\n");
  printf("  -r<a>:              output resolution for gs        (default: 600)\n");
  printf("  --width=<a>:        output width in pixels          (default: none)\n");
  printf("  --height=<a>:       output height in pixels         (default: none)\n");
  printf("  --gsopt=<a>:        add <a> to the gs command line  (default: none)\n");
}


static int round(double a)

{
  return floor(a+0.5);
}

static int isint(double a)

{
  if(fabs(a-round(a))<1e-6) return 1;
  else                      return 0;
}


int main(int argc,char *argv[])

{
  FILE *in,*out;
  char *infile=NULL,*outfile=NULL,*tmpfile=NULL,*copt,*ptr,*gsexec,buf[21];
  char command[MAX_IN],ch[MAX_IN],*BBname,*device,ropt[20],*gsopt;
  double bbllx,bblly,bburx,bbury,xoffset,yoffset,enlarge;
  int    width,height,res,xpix,ypix;
  int i,count,status,BBCorrected,did_CR,debug,compress,usegs,wait,
    BBhires,BBexact,found,depth,filter,ngsopt;
  int doseps;
  unsigned long dosepsbegin, dosepsend, pos;
  unsigned int c;
  fpos_t  fpos;


#if defined (__EMX__)
  _wildcard (&argc, &argv);
#endif

  /* default parameter */
  status=0;
  BBCorrected=0;
  BBhires=0;
  BBexact=0;
  debug=0;
  compress=1;
  usegs=1;
  gsexec=GSEXEC;
  wait=0;
  filter=0;
  enlarge=0.;
  res=600;
  device="pdfwrite";
  xpix=0;
  ypix=0;
  ngsopt=100;
  gsopt=malloc(ngsopt);
  gsopt[0]='\0';


  /******************************************************************

								     Process command line options

  ******************************************************************/

  for(i=1;i<argc;i++){

    /* printf("i=%d  arg=>>%s<<\n",i,argv[i]); */

    if(!strcmp(argv[i],"--help") || !strcmp(argv[i],"-h")){
      usage();
      status=0;
      goto EXIT;
    }

    if(!strcmp(argv[i],"--debug") || !strcmp(argv[i],"-d")){
      debug=1;
      continue;
    }

    if(!strcmp(argv[i],"--nodebug")){
      debug=0;
      continue;
    }

    if(!strcmp(argv[i],"--compress") || !strcmp(argv[i],"-c")){
      compress=1;
      continue;
    }

    if(!strcmp(argv[i],"--nocompress")){
      compress=0;
      continue;
    }

    if(!strcmp(argv[i],"--nogs")){
      usegs=0;
      continue;
    }

    if(!strcmp(argv[i],"--gs")){
      usegs=1;
      continue;
    }

    if(!strcmp(argv[i],"--wait")  || !strcmp(argv[i],"-w")){
      wait=1;
      continue;
    }

    if(!strcmp(argv[i],"--nowait")){
      wait=0;
      continue;
    }

    if(!strcmp(argv[i],"--filter")){
      filter=1;
      continue;
    }

    if(!strcmp(argv[i],"--nofilter")){
      filter=0;
      continue;
    }

    if(!strcmp(argv[i],"--hires")){
      BBhires=1;
      continue;
    }

    if(!strcmp(argv[i],"--nohires")){
      BBhires=0;
      continue;
    }

    if(!strcmp(argv[i],"--exact")){
      BBexact=1;
      continue;
    }

    if(!strcmp(argv[i],"--noexact")){
      BBexact=0;
      continue;
    }

    if(!strncmp(argv[i],"--outfile=",strlen("--outfile="))){
      outfile=malloc(strlen(argv[i])-strlen("--outfile=")+1);
      strcpy(outfile, argv[i]+strlen("--outfile="));
      continue;
    }

    if(!strncmp(argv[i],"--tmpfile=",strlen("--tmpfile="))){
      tmpfile=malloc(strlen(argv[i])-strlen("--tmpfile=")+1);
      strcpy(tmpfile, argv[i]+strlen("--tmpfile="));
      continue;
    }

    if(!strncmp(argv[i],"-r",strlen("-r"))){
      sscanf(argv[i]+strlen("-r"),"%d",&res);
      continue;
    }

    if(!strncmp(argv[i],"--width=",strlen("--width="))){
      sscanf(argv[i]+strlen("--width="),"%d",&xpix);
      continue;
    }

    if(!strncmp(argv[i],"--height=",strlen("--height="))){
      sscanf(argv[i]+strlen("--height="),"%d",&ypix);
      continue;
    }

    if(!strncmp(argv[i],"--gsopt=",strlen("--gsopt="))){
      char *opt=argv[i]+strlen("--gsopt=");
      if(strlen(gsopt)+strlen(opt)+2 < ngsopt){
	ngsopt+=100;
	gsopt=realloc(gsopt,ngsopt);
      }
      strcat(gsopt," ");
      strcat(gsopt,opt);
      continue;
    }

    if(!strncmp(argv[i],"-sDEVICE=",strlen("-sDEVICE="))){
      device=argv[i]+strlen("-sDEVICE=");
      continue;
    }

    if(!strcmp(argv[i],"-o") && i+1<argc){
      outfile=malloc(strlen(argv[i+1])+1);
      strcpy(outfile, argv[i+1]);
      i++;
      continue;
    }

    if(!strncmp(argv[i],"--gsexec=",strlen("--gsexec="))){
      gsexec=argv[i]+strlen("--gsexec=");
      continue;
    }


    if(argv[i][0]!='-'){
      if(infile) printf("\nCan process only one input file\n");
      else       infile=argv[i];
      continue;
    }

    if(!strncmp(argv[i],"--enlarge=",strlen("--enlarge="))){
      sscanf(argv[i]+strlen("--enlarge="),"%lf",&enlarge);
      continue;
    }

    usage();
    fprintf(stderr,"\nunknown option >>%s<<\n",argv[i]);
    status=1;
    goto EXIT;
  }



  /******************************************************************

								     check arguments and files

  ******************************************************************/


  if(filter) debug=0;
  if(filter) wait =0;

  if(BBexact && BBhires){
    fprintf(stderr,"\nOptions --hires and --exact cannot be used together\n");
    status=1;
    goto EXIT;
  }

  if     (BBexact) BBname="%%ExactBoundingBox:";
  else if(BBhires) BBname="%%HiresBoundingBox:";
  else             BBname="%%BoundingBox:";  

  if(!filter) {

    if(!infile) {
      usage();
      fprintf(stderr,"no input file specified!\n");
      status=1;
      goto EXIT;
    }

    if((in=fopen(infile,"rb")) == NULL) {
      usage();
      fprintf(stderr,"%s: File not found!\n",infile);
      status=1;
      goto EXIT;
    }
    fclose(in);

  }else{

    if(infile) {
      fprintf(stderr,"Input file cannot be used with filter option!\n");
      status=1;
      goto EXIT;
    }

    infile="epstopdf";   /* dummy input filename to generate tmp-filename */
  }



  /* find a temporary filename that does not exist yet */
  if(usegs && !tmpfile){
    tmpfile=malloc(strlen(infile)+9);
    count=0;
    do{
      if(count>99) {
	fprintf(stderr,"No temporary file available! Try deleting *.tmp.\n\n");
	status=1;
	goto EXIT;
      }
      sprintf(tmpfile,"%s.%d.tmp",infile,count);
      if(debug) printf("checking temporary filename >>%s<<\n",tmpfile);
      out=fopen(tmpfile,"rb");
      if(out) fclose(out);
      count++;
    }while(out);
  }

  if(!filter){
    if(!outfile){
      outfile=malloc(strlen(infile)+6);
      strcpy(outfile,infile);
      ptr=outfile;
      while(strpbrk(ptr,"\\/")) ptr=strpbrk(ptr,"\\/")+1;
      ptr=strrchr(ptr,'.');
      if(ptr) *ptr='\0';
      if(usegs) strcat(outfile,".pdf");
      else      strcat(outfile,"2.eps");
    }
  }else{
    if(outfile) {
      fprintf(stderr,"Output file cannot be used with filter option!\n");
      status=1;
      goto EXIT;
    }
    outfile=malloc(2);
    strcpy(outfile,"-");
  }



  if(!filter) printf("Converting %s to %s ..... ",infile,outfile);


  /******************************************************************

  put the pagesize from the bounding box into the eps file

  ******************************************************************/

	
  if(debug) printf("\nAdding correct pagesize to EPS ... searching for %s ...\n",BBname);

  if     (!filter) in  = fopen(infile, "rb");
  else {
    in  = stdin; 
    setmode(fileno(stdin), O_BINARY);
  }
  if     (usegs)   out = fopen(tmpfile,"wb");
  else if(!filter) out = fopen(outfile,"wb");
  else {
    out = stdout;
    setmode(fileno(stdout), O_BINARY);
  }
  if(!in || !out){
    fprintf(stderr,"cannot open files\n");
    status=1;
    goto EXIT;
  }
  depth=0;

  doseps = dosepsbegin = dosepsend = 0;
  c = getc(in);
  if (c == 'E'+0x80) {
    if ((getc(in)=='P'+0x80) && (getc(in)=='S'+0x80)
	&& (getc(in)=='F'+0x80)) {
      doseps = 1;
      dosepsbegin = getc(in) ;
      dosepsbegin += getc(in) * 256L ;
      dosepsbegin += getc(in) * 65536L ;
      dosepsbegin += getc(in) * 256L * 65536 ;
      dosepsend = getc(in) ;
      dosepsend += getc(in) * 256L ;
      dosepsend += getc(in) * 65536L ;
      dosepsend += getc(in) * 256L * 65536 ;
      fseek(in, dosepsbegin, 0);
      c = getc(in);
      dosepsend-- ;
    }
  }
  rewind(in);
  if (doseps)
    fseek(in, dosepsbegin, SEEK_SET);
  pos = dosepsbegin;
  while (fgets(ch,MAX_IN,in)) {
    if (doseps && ftell(in) > dosepsend + dosepsbegin) {
      ch[dosepsend + dosepsbegin - pos ] = '\0';
      fputs(ch, out); fputc('\n', out);
      break;
    }
    pos = ftell(in);    
    
    if(!strncmp(ch,BEGINDOC,strlen(BEGINDOC))) depth++;   /* count included documents */
    if(!strncmp(ch,ENDDOC,  strlen(ENDDOC)  )) depth--;
    if(!strncmp(ch,BBname,  strlen(BBname)) && depth==0) { /* look for BB comment in main doc only */
      sscanf(ch,"%*s %20s",buf);
      if(!strcmp(buf,"(atend)")){                       /* BB is atended */  
	if(filter){
	  fprintf(stderr,"Cannot look for BoundingBox in the trailer "
		  "with option --filter\n");
	  if(usegs) remove(tmpfile);
	  status=1;
	  goto EXIT;
	}
	if(debug) printf("\n (atend)! ...\n");
	fgetpos(in, &fpos); /* store file position */
	found=0;  
	while (fgets(ch,MAX_IN,in)){
	  if(!strncmp(ch,BEGINDOC,strlen(BEGINDOC))) depth++;   /* count included documents */
	  if(!strncmp(ch,ENDDOC,  strlen(ENDDOC)  )) depth--;
	  if(!strncmp(ch,BBname,strlen(BBname)) && depth==0) { /* look for bounding box in main doc only */
	    found=1;
	    fsetpos(in, &fpos);  /* rewind to (atend) comment */
	    break;
	  }	
	} 
	if(!found){
	  fprintf(stderr,"atended %s not found\n",BBname);
	  if(usegs  && !debug) remove(tmpfile);
	  if(!usegs && !debug) remove(outfile);
	  status=1;
	  goto EXIT;
	} 
      }


      /* No Idea what ExactBoundingBox means. Hope it also works with this code */

      /* I thought Postscript says that the bounding box should be integer.
	 However, some applications (like Corel Draw) use floats and gs has no
	 problem with it. So I use floats for translate that will result in a
	 more exact box. Since gs seems not to be able to use floats in 
	 setpagedevice, these values are converted to integer */   

      if(!BBCorrected){ /* write Bounding box one time only! */
	if(sscanf(ch,"%*s %lf %lf %lf %lf",&bbllx,&bblly,&bburx,&bbury)!=4){
	  fprintf(stderr,"incorrect %s \n",BBname);
	  if(usegs && !debug) remove(tmpfile);
	  status=1;
	  goto EXIT;
	}
	if(debug) printf("BoundingBox: %f %f %f %f\n",bbllx,bblly,bburx,bbury);
	bblly  -= enlarge;
	bbllx  -= enlarge;
	bbury  += enlarge;
	bburx  += enlarge;
	width   = ceil(bburx-bbllx);  /* make papersize integer and enlarge it a little bit */
	height  = ceil(bbury-bblly);
	xoffset =-bbllx;
	yoffset =-bblly;
	fprintf(out,"%s %d %d %d %d\n",BBname,0,0,width,height);
	fprintf(out,"<< /PageSize [%d %d] >> setpagedevice \n",width,height);
	if(isint(xoffset) && isint(yoffset)) fprintf(out,"gsave %d %d translate\n",round(xoffset),round(yoffset));
	else                                 fprintf(out,"gsave %f %f translate\n",xoffset,yoffset);
	if(!filter) printf(" (%dx%d mm) ... ",(int)(25.4/72.*width),(int)(25.4/72.*height));
	did_CR=1;
	BBCorrected=1;
      }
    }else{
      fputs(ch,out);
      if(strpbrk(ch, "\n")) did_CR=1;
      else                  did_CR=0;
    }
  }
  if(BBCorrected){
    if(!did_CR) fprintf(out,"\ngrestore\n");
    else        fprintf(out,  "grestore\n");
  }
  if(in !=stdin ) fclose(in);
  if(out!=stdout) fclose(out);

  if(width && height){
    if      (xpix) res=(72*xpix)/width;
    else if (ypix) res=(72*ypix)/height;
  }


  /******************************************************************

								     do the conversion eps->pdf using gs

  ******************************************************************/


  if(usegs){

#ifdef FPTEX
    char stemp[512];
    char *gs_argv[] = {
      NULL,		/* 0 gsexec */
      "-q",
      "-dNOPAUSE",
      "-dBATCH",
      NULL,		/* 4 copt */
      NULL,		/* 5 device */
      NULL,		/* 6 ropt */
      NULL,		/* 7 gsopt */
      NULL,		/* 8 outfile */
      /*	    "-", "-c", "quit", "-f", */
      NULL		/* 9 tmpfile */,
      NULL
    };
    int gs_argc = sizeof(gs_argv)/sizeof(gs_argv[0]);

    gs_argv[0] = gsexec;
    if(compress) gs_argv[4]="-dUseFlateCompression=true";
    else         gs_argv[4]="-dUseFlateCompression=false";

    if(res)      sprintf(ropt,"-r%d",res);
    else         ropt[0]='\0';

    if(res && debug) printf(" (%d dpi) ... ",res);

    sprintf(stemp, "-sDEVICE=%s", device);
    gs_argv[5] = xstrdup(stemp);
    gs_argv[6] = ropt;
    if (gsopt && *gsopt) {
      gs_argv[7] = gsopt;
      sprintf(stemp, "-sOutputFile=%s", outfile);
      gs_argv[8] = xstrdup(stemp);
      sprintf(stemp, "%s", tmpfile);
      gs_argv[9] = xstrdup(stemp);
    }
    else {
      sprintf(stemp, "-sOutputFile=%s", outfile);
      gs_argv[7] = xstrdup(stemp);
      sprintf(stemp, "%s", tmpfile);
      gs_argv[8] = xstrdup(stemp);
      gs_argc--;
    }
    if(debug) {
      int i;
      printf("running ghostscript ...\n");
      for (i = 0; i < gs_argc - 1; i++)
	fprintf(stderr, "gs_argv[%d] = %s\n", i, gs_argv[i]);
    }

    if (gs_locate() == NULL) {
      fprintf(stderr, "\nCan't locate Ghostscript ! Exiting ...\n");
      return EXIT_FAILURE;
    }

    if (!gs_dll_initialize()) {
      fprintf(stderr, "Can't initialize ghostscript, exiting ...\n");
      return EXIT_FAILURE;
    }
	 
    if ((status = (*pgsdll_init)(gsdll_callback,
				 NULL,
				 gs_argc - 1,
				 gs_argv)) == 0) {
      /* Should not happen : gs should quit right after being initialized. */
      /* fprintf(stderr, "gs exited quickly.\n"); */
      (*pgsdll_exit)();
    }
	 
    if (status != 0 && status != GSDLL_INIT_QUIT)
      fprintf(stderr, "gsdll_init returned %d\n", status);
	 
    gs_dll_release();
	 
    free(gs_argv[5]);

#if 1
    if(!debug) remove(tmpfile);
    else       printf("keeping temporary file >>%s<<\n",tmpfile);
#else
    remove(tmpfile);
#endif

#else
    if(compress) copt="-dUseFlateCompression=true";
    else         copt="-dUseFlateCompression=false";

    if(res)      sprintf(ropt,"-r%d",res);
    else         ropt[0]='\0';

    if(res && debug) printf(" (%d dpi) ... ",res);

    sprintf(command,"%s -q -dNOPAUSE -dBATCH %s -sDEVICE=%s %s %s"
	    " \"-sOutputFile=%s\" -c save pop -f \"%s\"",
	    gsexec,copt,device,ropt,gsopt,outfile,tmpfile);
    if(debug) printf("running ghostscript ...\n");
    if(debug) puts(command);
    status=system(command);
    if(!debug) remove(tmpfile);
    else       printf("keeping temporary file >>%s<<\n",tmpfile);
#endif /* ! FPTEX */
  }
	
  if(!filter) printf("Done\n");



 EXIT:

  free(outfile);
  free(tmpfile);
  free(gsopt);

  if(wait){
    printf("\n<Press a key> ");
    getchar();
    printf("\n");
  }

  return status;
}
