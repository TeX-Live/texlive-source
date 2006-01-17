This is the change file for CWEB's COMMON under OS/2
(Contributed by Jim Nutt, jnutt@PrimeNet.Com, May 1995)

These changes tested with WATCOM C v10.0a.
No changes to CTANGLE or CWEAVE are needed with OS/2.

@x section 69
An omitted change file argument means that |"/dev/null"| should be used,
@y
An omitted change file argument means that |"NUL"| should be used,
@z

@x section 70 (this change copied from comm-pc.ch)
        else if (*s=='/') dot_pos=NULL,name_pos=++s;
@y
        else if (*s == ':' || *s == '\\' || *s == '/')
	  dot_pos=NULL,name_pos=++s;
@z

@x section 70
  if (found_change<=0) strcpy(change_file_name,"/dev/null");
@y
  if (found_change<=0) strcpy(change_file_name,"NUL");
@z
