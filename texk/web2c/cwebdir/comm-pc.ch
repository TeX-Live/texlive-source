This is the change file for CWEB's COMMON under DOS
(Contributed by Lee Wittenberg, March 1993)

Changes necessary for compiling with Borland C/C++
Use compilation switches -mc -w-pro -Ff=5000

Note: The change to section 16 is not necessary if using a compiler
that allows >64K arrays.

@x section 16
@d max_bytes 100000 /* the number of bytes in identifiers,
@y
@d max_bytes (unsigned)60000 /* the number of bytes in identifiers,
@z

@x section 75
An omitted change file argument means that |"/dev/null"| should be used,
@y
An omitted change file argument means that |"NUL"| should be used,
@z

@x section 75
  strcpy(change_file_name,"/dev/null");
@y
  strcpy(change_file_name,"NUL");
@z

@x section 75 (this change copied from comm-bs.ch, July 94)
        else if (*s=='/') dot_pos=NULL,name_pos=++s;
@y
        else if (*s == ':' || *s == '\\' || *s == '/')
	  dot_pos=NULL,name_pos=++s;
@z
