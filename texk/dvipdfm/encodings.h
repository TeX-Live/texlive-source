#ifndef ENCODINGS_H

#define ENCODINGS_H

extern void encoding_set_verbose(void);
extern pdf_obj *find_encoding_differences (pdf_obj *encoding);
extern pdf_obj *make_differences_encoding (pdf_obj *encoding);
extern int get_encoding (const char *enc_name);
extern void encoding_flush_all (void);
extern char *encoding_glyph (int encoding_id, unsigned code);
extern pdf_obj *encoding_ref (int encoding_id);

#endif






