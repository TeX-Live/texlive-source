/* image.hpp -- classes for sampled (indexed, gray and rgb) images
 * by pts@fazekas.hu Wed Feb 27 09:24:47 CET 2002
 */
/* Imp: find memory leaks */
/* Imp: keep future transparency in toIndexed(...) */

#ifdef __GNUC__
#pragma interface
#endif

#ifndef SAMPLED_HPP
#define SAMPLED_HPP 1

#include "config2.h"
#include "gensi.hpp"

class Image {
 public:
  class RGB;
  class Gray;
  class Indexed;
  /** Generic, sampled, rectangular image data. Abstract class.
   * Each sample is 1, 2, 4 or 8 bits. Regions:
   * beg..head-1: comment, ignored (e.g unused part of the indexed palette)
   * headp..rowbeg-1: header: not predicted or compressed (e.g the indexed palette)
   * rowbeg+0*rlen..rowbeg+0*rlen+rlen-1: sample data of the 0th row, compressed and predicted
   * rowbeg+1*rlen..rowbeg+1*rlen+rlen-1: sample data of the 1st row
   * rowbeg+2*rlen..rowbeg+2*rlen+rlen-1: sample data of the 1st row
   * ...
   * rowbeg+(h-1)*rlen..rowbeg+h*rlen-1: sample data of the last row
   * trail..beg+len: trailer, ignored. Its length must be >=bpc.
   */
  class Sampled: public SimBuffer::Flat {
   public:
    /** Can hold 1 component of a sample of a single pixel. */
    typedef unsigned char sample_t;
    /** Can hold a height or depth of the image */
    typedef unsigned int dimen_t;
    /** Can hold a row length (in byte), which isn't greater than 3*(image width). */
    typedef unsigned int rlen_t;
    /** RGB = (red<<16)+(green<<8)+(blue). red, green and blue are 0..255 */
    #if SIZEOF_LONG>4 && SIZEOF_INT>=4
      typedef unsigned int rgb_t;
    #else
      typedef unsigned long rgb_t;
    #endif
    BEGIN_STATIC_ENUM1(unsigned char)
      TY_INDEXED=1, TY_GRAY=2, TY_RGB=3, TY_OTHER=4, TY_BLACKBOX=5
    END_STATIC_ENUM()
    BEGIN_STATIC_ENUM1(unsigned char) // static const unsigned char
        CS_UNKNOWN=0,           /* error/unspecified */
        CS_GRAYSCALE=1,         /* monochrome */
        CS_RGB=2,               /* red/green/blue */
        CS_YCbCr=3,             /* Y/Cb/Cr (also known as YUV) */
        CS_CMYK=4,              /* C/M/Y/K */
        CS_YCCK=5,              /* Y/Cb/Cr/K */
        CS_Indexed_RGB=12
    END_STATIC_ENUM()
    static const unsigned char cs2cpp[6];
    /** @return NULLP on error */
    static char const *cs2devcs(unsigned char cs);
   protected:
    char *headp;
    char *rowbeg;
    char *trail;
    /** Extra offset */
    slen_t xoffs;

    /** Length of one row, in bytes. Each row must begin on a byte boundary, so
     * extra bits are appended after the rightmost pixel. These bits are
     * arbitrary, and are ignored by the PostScript interpreter.
     */
    rlen_t rlen;
    /** Image height, in pixels. */
    dimen_t ht;
    /** Image width, in pixels. */
    dimen_t wd;
    /** Color space. */
    unsigned char cs;
    /** Components per pixel. (number of planes, image depth). 1 for indexed,
     * 1 for gray, 3 for RGB
     */
    unsigned char cpp;
    /** BitsPerComponent: 1, 2, 4 or 8. PostScript allows 12 too. */
    unsigned char bpc;
    /** Transparent color value. Imp: ... */
    rgb_t transpc;
    /** Image type, TY_... */
    unsigned char ty;
    /** Initializes various fields, allocates memory. Called from descendants'
     * constructors.
     */
    void init(slen_t l_comment, slen_t l_header, dimen_t wd_, dimen_t ht_,
      unsigned char bpc_, unsigned char ty_, unsigned char cpp_);
    /** Convert samples, make bpc=8, multiplication. */
    void to8mul();
    /** Convert samples, make bpc=8, no multiplication. */
    void to8nomul();
    /** Calls copyRGBRow.
     * @return an Image::Indexed version of (this) iff the number of
     *         colors<=256. Otherwise, returns NULLP.
     */
    Indexed* toIndexed0()/* const*/;
    /** No averaging is done, only the red component is extracted */
    Gray* toGray0(unsigned char bpc_);
    RGB * toRGB0(unsigned char bpc_);
    /** @return if any pixels are not gray: false. otherwise: true or false. */
   public:
    inline bool hasTransp() const { return transpc!=0x1000000UL; }
    virtual bool canGray() const =0;
    /** @return an RGB BitsPerComponent number (1,2,4 or 8) to which the image
     * could be converted without any loss. The default implementation calls
     * copyRGBRow().
     */
    virtual unsigned char minRGBBpc() const;
    inline virtual ~Sampled() { delete [] const_cast<char*>(beg); }
    /** Copies whichrow as wd*3 bytes (R0,G0,B0,R1,G1,B1...) to `to' */
    virtual void copyRGBRow(char *to, dimen_t whichrow) const =0;
    virtual bool hasPixelRGB(Image::Sampled::rgb_t rgb) const;
    inline char *getRowbeg() const { return rowbeg; }
    inline dimen_t getWd() const { return wd; }
    inline dimen_t getHt() const { return ht; }
    inline unsigned char getTy() const { return ty; }
    inline unsigned char getBpc() const { return bpc; }
    inline unsigned char getCpp() const { return cpp; }
    inline unsigned char getCs() const { return cs; }
    inline slen_t getXoffs() const { return xoffs; }
    inline rlen_t getRlen() const { return rlen; }
    inline rgb_t getTranspc() const { return transpc; }
    inline char *getHeadp() const { return headp; }
    /** Convert samples, make bpc=8. */
    virtual void to8() =0;
    /** @return NULLP if too many colors for indexed; otherwise a new Image::Indexed.
     * The caller should `delete' (this) if toIndexed()==NULLP.
     */
    virtual /*Image::*/Indexed* toIndexed() =0;
    virtual /*Image::*/RGB*  toRGB(unsigned char bpc_) =0;
    virtual /*Image::*/Gray* toGray(unsigned char bpc_) =0;
    // virtual void setBpc(unsigned char bpc_) =0;
    friend GenBuffer::Writable& operator<<(GenBuffer::Writable&, /*Image::*/Sampled const&);
    /** @return address of static buffer: "#RRGGBB" */
    static char *rgb2webhash(rgb_t);
    /** @return (this) or an image containing (this) composed with alpha
     * channel `al'
     */
    virtual Sampled* addAlpha(/*Image::*/Gray *al) =0;
    /** assert(al.bpp=8) etc. Imp: document this */
    static Indexed* addAlpha0(Sampled *img, Gray *al);
  };

  class Indexed: public Sampled {
   public:
    /** @param ncols_ must be >= the colors used */
    Indexed(dimen_t wd_, dimen_t ht_, unsigned short ncols_, unsigned char bpc_);
    inline unsigned short getNcols() const { return (rowbeg-headp)/3; }
    /** Destroys the color table, and creates one with ncols_ colors.
     * @param ncols_ must be <= the ncols_ specified in the constructor
     */
    void setNcols(unsigned short ncols_);
    /** Decreases the size of the palette (forgets last colors) to the
     * specified amount.
     */
    void setNcolsMove(unsigned short ncols_);
    void setPal(unsigned char coloridx, rgb_t rgb);
    rgb_t getPal(unsigned char coloridx) const;
    /** @param coloridx must be >=0, transp must be -1 */
    void setTransp(unsigned char coloridx);
    /** @return new hasTransp */
    bool setTranspc(rgb_t color);
    virtual void copyRGBRow(char *to, dimen_t whichrow) const;
    /* virtual bool hasPixelRGB(Image::Sampled::rgb_t rgb) const; */
    /** Packs (compresses) the palette so that it will be continuous in
     * 0..ncols-1, and each color will be used exactly once. The
     * transparent color (if present) will become black. As a side-effect,
     * packPal() may set (this)->bpc=8.
     */
    void packPal();
    virtual void to8();
    virtual /*Image::*/Indexed* toIndexed();
    virtual /*Image::*/RGB*  toRGB(unsigned char bpc_);
    virtual /*Image::*/Gray* toGray(unsigned char bpc_);
    virtual bool canGray() const;
    inline signed short getTransp() const { return transp; }
    inline signed short getClearTransp() { signed short ret=transp; transp=-1; return ret; }
    /** if (transp>0) transp=0;, converts image data. Does not change bpc. */
    void makeTranspZero();
    virtual unsigned char minRGBBpc() const;
    /** Separates the current image into Indexed1 images. The caller is
     * recommended to call packPal() first to reduce the number of required
     * images.
     * As a side-effect,
     * separate() may set (this)->bpc=8. 
     * @return the array of images after full color separation: that is
     *   a dynamically allocated array of `getNcols()-(getTransp()!=-1)'
     *   Indexed images: each image is Indexed1, color 0 is opaque (with the
     *   color obtained from (this)), color 1 is transparent. The returned
     *   array is NULLP-terminated.
     */
    Indexed **separate();
    /** Also calls packPal(). As a side effect, changes all transparent
     * pixels to color index 0.
     * @return NULLP if no transparent pixels.
     */
    Indexed *calcAlpha();
    /** Deletes all elements of p, but not p itself.
     * @param p a NULLP-terminated list of (Indexed*)s.
     */
    static void delete_separated(Indexed **p);
    /** Reorganizes the image so it will have the specified bpc. Issues a
     * runtime error if the specified bpc cannot be achieved.
     * @param bpc_ the desired bpc, or 0: the best achievable.
     */
    virtual void setBpc(unsigned char bpc_);
    void dumpDebug(GenBuffer::Writable& gw);
   protected:
    /* Index of the transparent color, or -1. */
    signed short transp;
    virtual /*Image::*/Sampled* addAlpha(/*Image::*/Gray *al);
  };
  class Gray: public Sampled {
   public:
    Gray(dimen_t wd_, dimen_t ht_, unsigned char bpc_);
    virtual void copyRGBRow(char *to, dimen_t whichrow) const;
    virtual bool hasPixelRGB(Image::Sampled::rgb_t rgb) const;
    virtual void to8();
    virtual /*Image::*/Indexed* toIndexed();
    virtual bool canGray() const;
    // virtual void setBpc(unsigned char bpc_);
    virtual /*Image::*/RGB    * toRGB(unsigned char bpc_);
    virtual /*Image::*/Gray   * toGray(unsigned char bpc_);
    virtual /*Image::*/Sampled* addAlpha(/*Image::*/Gray *al);
    /** Calls to8(). */
    void calcExtrema(unsigned char &lightest, unsigned char &darkest);
  };
  class RGB: public Sampled {
   public:
    RGB(dimen_t wd_, dimen_t ht_, unsigned char bpc_);
    virtual void copyRGBRow(char *to, dimen_t whichrow) const;
    /* virtual bool hasPixelRGB(Image::Sampled::rgb_t rgb) const; */
    virtual void to8();
    virtual /*Image::*/Indexed* toIndexed();
    virtual bool canGray() const;
    // virtual void setBpc(unsigned char bpc_);
    virtual /*Image::*/RGB    * toRGB(unsigned char bpc_);
    virtual /*Image::*/Gray   * toGray(unsigned char bpc_);
    virtual /*Image::*/Sampled* addAlpha(/*Image::*/Gray *al);
  };
  
  /** Avoid including <stdio.h> */
  typedef void *filep_t;
  
  /** Describes a driver that can load a specific image file format. */
  struct Loader {
    /** Filter::UngetFILED */
    class UFD;
    /** A function that can (allocate and) load a sampled image. Never
     * returns NULL. On error, it calls Error::.... The filep_t argument
     * should be really cast back to FILE*. The reader must fclose the FILE*.
     */
    // typedef Sampled*(*reader_t)(filep_t, SimBuffer::Flat const& loadHints);
    typedef Sampled*(*reader_t)(UFD* ufd, SimBuffer::Flat const& loadHints);
    BEGIN_STATIC_ENUM1(unsigned) MAGIC_LEN=64 END_STATIC_ENUM()
    /** A function that checks the magic numbers at the beginning of a file
     * (already read into buf), and returns NULL if it cannot load an image
     * of that type, or a reader_t that will load the image. If (and only if!)
     * file is shorter than 64 bytes, the buf is padded with '\000' bytes.
     * @param f may read from freely if necessary (MAGIC_LEN is short), but
     *   has to call rewind(f) before reading
     */
    typedef reader_t(*checker_t)(char buf[MAGIC_LEN], char bufend[MAGIC_LEN], SimBuffer::Flat const& loadHints, UFD* ufd);
    /** A null-terminated, compact string describing (not defining!) the image
     * file format.
     * Examples: "GIF", "XPM", "PNM"
     */
    char const*format;
    checker_t checker;
    /** Null or next loader. */
    Loader *next;
  };
  
  /** Registers a new type of image Loader, i.e a new image file format. The
   * new image format will be put in front of all others, and will be checked
   * first
   */
  static void register0(Loader *);
  /** Loads the image contained in te file `filename'.
   * @param format NULLP is unknown (load any format)
   *   or an Image::Loader::format already registered
   */
  static Sampled* load(Loader::UFD* ufd, SimBuffer::Flat const& loadHints, char const* format);
  static Sampled* load(char const *filename, SimBuffer::Flat const& loadHints, filep_t stdin_f=(filep_t*)NULLP, char const* format=(char const*)NULLP);
  /* Prints the list of available Loaders (->format), separated by spaces.
   * Returns the number of available Loaders. Prepends a space if >= loaders.
   */
  static unsigned printLoaders(GenBuffer::Writable &);

  /** SampleFormat constants */
  BEGIN_STATIC_ENUM(unsigned, sf_t)
    SF_None=0, /* no specific sample format */
    SF_Opaque=1,
    SF_Transparent=2,
    SF_Gray1=3,
    SF_Indexed1=4,
    SF_Mask=5,
    SF_Transparent2=6,
    SF_Gray2=7,
    SF_Indexed2=8,
    SF_Transparent4=9,
    SF_Rgb1=10,
    SF_Gray4=11,
    SF_Indexed4=12,
    SF_Transparent8=13,
    SF_Rgb2=14,
    SF_Gray8=15,
    SF_Indexed8=16,
    SF_Rgb4=17,
    SF_Rgb8=18,
    SF_Asis=19,
    SF_Bbox=20,
    SF_max=31
  END_STATIC_ENUM()

  /** Contains (and memory-manages) an image, and optimization information
   * as a cache.
   */
  class SampledInfo {
   public:
    SampledInfo(Sampled *img_);
    ~SampledInfo();
    inline Sampled* getImage() const { return img; }
    /**
     * Source image, may be modified even if TryOnly==false. If
     * modified, the original will be freed.
     * @param sf desired sample format, see Image::SF_* constants
     * @param WarningOK if false: make the conversion fail if it would produces
     *        a Warning
     * @param TryOnly if true: don't do the real conversion (but may do auxilary,
     *        idempontent, helper conversion), assume it has succeeded
     * @param Transparent: as part of the conversion, try to make this RGB color
     *        transparent
     * @return true iff the conversion succeeded. Note that img may be the same
     *         pointer even when true is returned
     */
    bool setSampleFormat(sf_t sf, bool WarningOK, bool TryOnly, Sampled::rgb_t Transparent);
    inline Indexed **getImgs() const { return imgs; }
    inline Sampled *getImg() const { return img; }
    inline unsigned getNncols() const { return nncols; }
    void separate();
    inline bool canGrayy() const { return canGray; }
    inline unsigned char minRGBBpcc() const { return minRGBBpc; }
    inline bool hasTranspp() const { return hasTransp; }
    inline unsigned char minGrayBpcc() const { return canGray ? minRGBBpc : 0; }
    inline void clearTransp() { hasTransp=false; }
   protected:
    bool hasTransp;
    /** Number of non-transparent colors, or 257 if >=257. */
    unsigned nncols;
    /** Has only gray colors. */
    bool canGray;
    unsigned char minRGBBpc;
    Sampled *img;
    /** The array of images after full color separation. May be NULLP (default),
     * or a dynamically allocated array of `nncols' Indexed images: each
     * image is Indexed1, color 0 is opaque (with any value), color 1 
     * is transparent.
     */
    Indexed **imgs;
    sf_t sf;
  };
};

/** Dumps this Image as a rawbits PPM file (plus a comment indicating transparency)
 * @return the Writable.
 */
GenBuffer::Writable& operator<<(GenBuffer::Writable&, Image::Sampled const&);

#endif
