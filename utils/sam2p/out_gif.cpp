/* out_gif.cpp
 * by pts@fazekas.hu at Sat Mar 23 11:02:36 CET 2002
 */

#include "image.hpp"

#if USE_OUT_GIF
#include <string.h>
#include <stdio.h>

/**** pts ****/
#ifdef __cplusplus
#  define AALLOC(var,len,itemtype) var=new itemtype[len]
#  define AFREE(expr) delete [] (expr)
#else
#  include <stdlib.h> /* malloc(), free() */
#  define AALLOC(var,len,itemtype) var=(itemtype*)malloc(len*sizeof(itemtype));
#  define AFREE(expr) free((expr))
#endif
#define HasLZW HAVE_LZW /* Imp: or USE_BUILTIN_LZW? */
#define True true
#define False false

/* The code of GIFEncodeImage is based on an early version of ImageMagick. */
static unsigned int GIFEncodeImage(GenBuffer::Writable& out, char const*ppbeg, register char const*ppend, const unsigned int data_size)
{
#define MaxCode(number_bits)  ((1 << (number_bits))-1)
#define MaxHashTable  5003
#define MaxGIFBits  12
#if defined(HasLZW)
#define MaxGIFTable  (1 << MaxGIFBits)
#else
#define MaxGIFTable  max_code
#endif
#define GIFOutputCode(code) \
{ \
  /*  \
    Emit a code. \
  */ \
  if (bits > 0) \
    datum|=((long) code << bits); \
  else \
    datum=(long) code; \
  bits+=number_bits; \
  while (bits >= 8) \
  { \
    /*  \
      Add a character to current packet. \
    */ \
    packet[byte_count++]=(unsigned char) (datum & 0xff); \
    if (byte_count >= 254) \
      { \
        packet[-1]=byte_count; \
        out.vi_write((char*)packet-1, byte_count+1); \
        byte_count=0; \
      } \
    datum>>=8; \
    bits-=8; \
  } \
  if (free_code > max_code)  \
    { \
      number_bits++; \
      if (number_bits == MaxGIFBits) \
        max_code=MaxGIFTable; \
      else \
        max_code=MaxCode(number_bits); \
    } \
}

  int
    bits,
    byte_count,
    i,
    next_pixel,
    number_bits;

  long
    datum;

  register int
    displacement,
    k;
    
  register char const*pp;

  short
    clear_code,
    end_of_information_code,
    free_code,
    *hash_code,
    *hash_prefix,
    index,
    max_code,
    waiting_code;

  unsigned char
    *packet,
    *hash_suffix;

  /*
    Allocate encoder tables.
  */
  AALLOC(packet,257,unsigned char);
  AALLOC(hash_code,MaxHashTable,short);
  AALLOC(hash_prefix,MaxHashTable,short);
  AALLOC(hash_suffix,MaxHashTable,unsigned char);
  if ((packet == (unsigned char *) NULL) || (hash_code == (short *) NULL) ||
      (hash_prefix == (short *) NULL) ||
      (hash_suffix == (unsigned char *) NULL))
    return(False);
  packet++;
  /* Now: packet-1 == place for byte_count */
  /*
    Initialize GIF encoder.
  */
  number_bits=data_size;
  max_code=MaxCode(number_bits);
  clear_code=((short) 1 << (data_size-1));
  end_of_information_code=clear_code+1;
  free_code=clear_code+2;
  byte_count=0;
  datum=0;
  bits=0;
  for (i=0; i < MaxHashTable; i++)
    hash_code[i]=0;
  GIFOutputCode(clear_code);
  /*
    Encode pixels.
  */
  /**** pts ****/
  pp=ppbeg;
  waiting_code=*(unsigned char const*)pp++; /* unsigned char BUGFIX at Sun Dec  8 13:17:00 CET 2002 */

  while (pp!=ppend) {
      /*
        Probe hash table.
      */
      index=*(unsigned char const*)pp++;
      k=(int) ((int) index << (MaxGIFBits-8))+waiting_code;
      if (k >= MaxHashTable)
        k-=MaxHashTable;
#if defined(HasLZW)
      if (hash_code[k] > 0)
        {
          if ((hash_prefix[k] == waiting_code) && (hash_suffix[k] == index))
            {
              waiting_code=hash_code[k];
              continue;
            }
          if (k == 0)
            displacement=1;
          else
            displacement=MaxHashTable-k;
          next_pixel=False;
          for ( ; ; )
          {
            k-=displacement;
            if (k < 0)
              k+=MaxHashTable;
            if (hash_code[k] == 0)
              break;
            if ((hash_prefix[k] == waiting_code) && (hash_suffix[k] == index))
              {
                waiting_code=hash_code[k];
                next_pixel=True;
                break;
              }
          }
          if (next_pixel != False) /* pacify VC6.0 */
            continue;
        }
#endif
      GIFOutputCode(waiting_code);
      // printf("wc=%u\n", waiting_code);
      if (free_code < MaxGIFTable)
        {
          hash_code[k]=free_code++;
          hash_prefix[k]=waiting_code;
          hash_suffix[k]=index;
        }
      else
        {
          /*
            Fill the hash table with empty entries.
          */
          for (k=0; k < MaxHashTable; k++)
            hash_code[k]=0;
          /*
            Reset compressor and issue a clear code.
          */
          free_code=clear_code+2;
          GIFOutputCode(clear_code);
          number_bits=data_size;
          max_code=MaxCode(number_bits);
        }
      waiting_code=index;
#if 0 /**** pts ****/
      if (QuantumTick(i,image) && (image->previous == (Image2 *) NULL))
        ProgressMonitor(SaveImageText,i,image->packets);
#endif
  }
  /*
    Flush out the buffered code.
  */
  GIFOutputCode(waiting_code);
  GIFOutputCode(end_of_information_code);
  if (bits > 0)
    {
      /*
        Add a character to current packet.
      */
      packet[byte_count++]=(unsigned char) (datum & 0xff);
      if (byte_count >= 254)
        {
          packet[-1]=byte_count;
          out.vi_write((char*)packet-1, byte_count+1);
          byte_count=0;
        }
    }
  /*
    Flush accumulated data.
  */
  if (byte_count > 0)
    {
      packet[-1]=byte_count;
      out.vi_write((char*)packet-1, byte_count+1);
    }
  /*
    Free encoder memory.
  */
  AFREE(hash_suffix);
  AFREE(hash_prefix);
  AFREE(hash_code);
  AFREE(packet-1);
  return pp==ppend;
}

/** This isn't a complete GIF writer. For example, it doesn't support
 * animation or multiple sub-images. But it supports transparency and
 * compression. Only works when . The user should call
 * packPal() first to ensure img->getBpc()==8, and to get a minimal palette.
 */
void out_gif_write(GenBuffer::Writable& out, Image::Indexed *img) {
  /* Tested and proven to work at Sat Mar 23 13:11:41 CET 2002 */
  unsigned i, c, bits_per_pixel;
  signed transp;
  char hd[19];
  
  assert(img->getBpc()==8); /* 1 palette entry == 8 bits */
  
  transp=img->getTransp();
  memcpy(hd, transp!=-1 ? "GIF89a" : "GIF87a", 6);
  i=img->getWd(); hd[6]=i; hd[7]=i>>8;
  i=img->getHt(); hd[8]=i; hd[9]=i>>8;
  
  // transp=-1; /* With this, transparency will be ignored */
  c=img->getNcols();
  bits_per_pixel=1; while (((c-1)>>bits_per_pixel)!=0) bits_per_pixel++;
  /* ^^^ (c-1) BUGFIX at Mon Oct 20 15:18:24 CEST 2003 */
  /* 63 -> 6, 64 -> 6, 65 -> 7 */
  // if (bits_per_pixel>1) bits_per_pixel--; /* BUGFIX at Wed Apr 30 15:55:27 CEST 2003 */ /* BUGFIX at Mon Oct 20 15:18:14 CEST 2003 */
  // fprintf(stderr, "GIF89 write transp=%d ncols=%d bpp=%d\n", transp, c, bits_per_pixel);
  assert(1<=bits_per_pixel && bits_per_pixel<=8); 
  c=3*((1<<bits_per_pixel)-c);
  /* Now: c is the number of padding bytes */
  
  hd[10]= 0x80 /* have global colormap */
        | ((8-1) << 4) /* color resolution: bpc==8 */
        | (bits_per_pixel-1); /* size of global colormap */
  hd[11]=0; /* background color: currently unused */
  hd[12]=0; /* reversed */
  out.vi_write(hd, 13);

  // out.vi_write("\xFF\x00\x00" "\x00\xFF\x00" "\x00\x00\xFF", 9);
  
  out.vi_write(img->getHeadp(), img->getRowbeg()-img->getHeadp()); /* write colormap */
  if (c!=0) {
    char *padding=new char[(unsigned char)c]; /* BUGFIX at Fri Oct 17 18:05:09 CEST 2003 */
    memset(padding, '\0', (unsigned char)c); /* Not automatic! */
    out.vi_write(padding, (unsigned char)c);
    delete [] padding;
  }

  if (transp!=-1) {
    /* Write Graphics Control extension. Only GIF89a */
    hd[0]=0x21; hd[1]=(char)0xf9; hd[2]=0x04;
    hd[3]=transp!=-1; /* dispose==0 */
    hd[4]=hd[5]=0; /* delay==0 */
    hd[6]=transp; /* transparent color index -- or 255 */
    hd[7]=0;
    out.vi_write(hd, 8);
  }
  
  /* Write image header */
  hd[8]=',';
  hd[ 9]=hd[10]=0;   /* left */
  hd[11]=hd[12]=0; /* top  */
  i=img->getWd(); hd[13]=i; hd[14]=i>>8;
  i=img->getHt(); hd[15]=i; hd[16]=i>>8;
  hd[17]=0; /* no interlace, no local colormap, no bits in local colormap */
  
  if ((c=bits_per_pixel)<2) c=4;
  hd[18]=c; /* compression bits_per_pixel */
  out.vi_write(hd+8, 11);

#if 0
  printf("GIFEncodeImage out r r+%u %u; off=%u\n", img->getRlen()*img->getHt(), c+1, img->getRowbeg()-img->getHeadp());
  FILE *f=fopen("tjo.dat","wb");
  fprintf(f, "P6 %u %u 255\n", img->getWd(), img->getHt());
  // fwrite(img->getRowbeg(), 1, img->getRlen()*img->getHt(), f);
  for (unsigned u=0; u<img->getRlen()*img->getHt(); u++) {
    char *p=img->getHeadp()+3* *(unsigned char*)(img->getRowbeg()+u);
    putc(p[0],f);
    putc(p[1],f);
    putc(p[2],f);
  }
#endif
  
  i=GIFEncodeImage(out, img->getRowbeg(), img->getRowbeg()+img->getRlen()*img->getHt(), c+1);
#if 0
  { char buf[500000];
    FILE *f=fopen("tjo.dat","rb");
    int got=fread(buf, 1, sizeof(buf), f);
    assert(got==486109);
    assert(got==img->getRlen()*img->getHt());
    i=GIFEncodeImage(out, buf, buf+img->getRlen()*img->getHt(), c+1);
  }
#endif
  assert(i!=0);
  
  /* Write trailer */
  hd[0]=0; hd[1]=';';
  out.vi_write(hd, 2);
}
#else
#include <stdlib.h>
void out_gif_write(GenBuffer::Writable&, Image::Indexed *) {
  assert(0);
  abort();
}
#endif
