Change file for wmerge, for OS/2 and WinNT and MSVC++ 2.2
contributed May 1996 by Andreas Scherer, Aaxen University of TeXnology
(scherer@physik.rwth-aachen.de)

@x Line 594
An omitted change file argument means that |'/dev/null'| should be used,
@y
An omitted change file argument means that |'NUL'| should be used,
@z

@x Line 620
        else if (*s=='/') dot_pos=NULL,++s;
@y
        else if (*s==':' || *s=='\\' || *s=='/') dot_pos=NULL,++s;
@z

@x Line 630
  if (!found_change) strcpy(change_file_name,"/dev/null");
@y
  if (!found_change) strcpy(change_file_name,"NUL");
@z
