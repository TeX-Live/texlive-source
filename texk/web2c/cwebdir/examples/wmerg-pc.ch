Change file for wmerge, for MSDOS and Borland C++ 3.1.
To compile the C file:  bcc -w-pro wmerge.c

(This file contributed by Barry Schwartz, trashman@crud.mn.org, 28 Jun 94.)
(Updated 17 Jul 06 by Don Knuth.)

@x Section 19.
    cur_file_name[l]='/'; /* \UNIX/ pathname separator */
@y
    cur_file_name[l]='/'; /* A valid {\mc MSDOS} pathname separator */
@z


@x Section 33.
        if (*s=='.') dot_pos=s++;
        else if (*s=='/') dot_pos=NULL,++s;
        else s++;
@y
        if (*s=='.') dot_pos=s++;
        else if (*s==':' || *s=='\\' || *s=='/') dot_pos=NULL,++s;
        else s++;
@z
