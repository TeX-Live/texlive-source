/* prototypes used in epdf.c and pdftoepdf.cc  */

/* epdf.c */
#ifdef __cplusplus
# define fd_entry struct fd_entry
# define fm_entry struct fm_entry
# define integer int
#endif
    extern int is_subsetable(fm_entry *);
    extern fd_entry *epdf_create_fontdescriptor(fm_entry *, int);
    extern void epdf_mark_glyphs(fd_entry *, char *);
    extern integer get_fd_objnum(fd_entry *);
    extern integer get_fn_objnum(fd_entry *);
    extern void embed_whole_font(fd_entry *);
#undef fd_entry
#undef fm_entry
#undef integer

/* pdftoepdf.cc */
    extern void epdf_check_mem(void);
