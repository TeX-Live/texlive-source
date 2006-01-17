This is the change file for CWEB's COMMON on the Amiga
(Contributed by Tomas Rokicki, June 1993)

With Lattice C 5.1, use compilation switches -b0 -r0, and ignore the
compiler warnings. With SAS 6.0, use compilation switches Code=far Data=far.

@x section 69
An omitted change file argument means that |"/dev/null"| should be used,
@y
An omitted change file argument means that |"nil:"| should be used,
@z

@x section 70
  if (found_change<=0) strcpy(change_file_name,"/dev/null");
@y
  if (found_change<=0) strcpy(change_file_name,"nil:");
@z
