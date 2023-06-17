Section 1.

@x
\ifx\undefined\pdfpagewidth
\else
  \pdfpagewidth=\pagewd \advance\pdfpagewidth by 2cm
  \pdfpageheight=\pageht \advance\pdfpageheight by 5cm
  \ifpdftex \pdfhorigin=1cm \pdfvorigin=1cm
  \else \global\hoffset=-1.54cm \global\voffset=-1.54cm \fi
\fi

@y
@z

Section 328.

@x
  strncpy(cb_banner,banner,max_banner-1);
@y
  strncpy(cb_banner,banner,max_banner-1);

@** Index.
If you have read and understood the code for Phase III above, you know what
is in this index and how it got here. All sections in which an identifier is
used are listed with that identifier, except that reserved words are
indexed only when they appear in format definitions, and the appearances
of identifiers in section names are not indexed. Underlined entries
correspond to where the identifier was declared. Error messages, control
sequences put into the output, and a few
other things like ``recursion'' are indexed here too.
@z
