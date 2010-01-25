/* prototypes used in image/pdftoepdf.cc and in C code  */

/* image/epdf.c */
    extern void epdf_mark_glyphs(fd_entry *, char *);
    extern fd_entry *epdf_create_fontdescriptor(fm_entry *, int);

/* font/writeenc.c */
    extern void epdf_write_enc(char **, integer);
