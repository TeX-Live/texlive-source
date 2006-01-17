#ifndef T1LIB_H
#define T1LIB_H
#ifdef __cplusplus
extern "C" {
#endif

#define PFB_MARKER	128
#define PFB_ASCII	1
#define PFB_BINARY	2
#define PFB_DONE	3

struct font_reader {
  void (*output_ascii)(char *, int);
  void (*output_binary)(unsigned char *, int);
  void (*output_end)();
};

void process_pfa(FILE *, const char *filename, struct font_reader *);
void process_pfb(FILE *, const char *filename, struct font_reader *);

struct pfb_writer {
  unsigned char *buf;
  unsigned len;
  unsigned max_len;
  unsigned pos;
  int blocktyp;
  int binary_blocks_written;
  FILE *f;
};

void init_pfb_writer(struct pfb_writer *, int, FILE *);
void pfb_writer_output_block(struct pfb_writer *);
void pfb_writer_grow_buf(struct pfb_writer *);
void pfb_writer_end(struct pfb_writer *);
#define PFB_OUTPUT_BYTE(w, b)	do { \
	if ((w)->pos >= (w)->len) pfb_writer_grow_buf(w); \
	(w)->buf[(w)->pos++] = (b); \
      } while (0)

int crcbuf(int crc, unsigned int len, const char *buf);

/* whoever uses this code must provide a definition for these functions */
extern void error(const char *, ...);
extern void fatal_error(const char *, ...);

#ifdef __cplusplus
}
#endif
#endif
