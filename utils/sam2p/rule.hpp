/* rule.hpp -- Output rules
 * by pts@fazekas.hu Fri Mar 15 15:11:38 CET 2002
 */

#ifdef __GNUC__
#pragma interface
#endif

#ifndef RULE_HPP
#define RULE_HPP 1

#include "config2.h"
#include "gensio.hpp"
#include "image.hpp"
#include "minips.hpp"

/** a namespace */
class Rule { public:
  struct Cache {
    bool WarningOK;
    BEGIN_STATIC_ENUM(unsigned char,ff_t)
      FF_default=0, /* only for one-liner */
      FF_eps=71, /* only for one-liner, temporary */
      FF_pdf=72, /* only for one-liner, temporary */
      FF_pdfb=73, /* only for one-liner, temporary */
      FF_PSL1=110,
      FF_PSLC=115,
      FF_PSL2=120,
      FF_PSL3=130,
      FF_PDFB10=90,
      FF_PDFB12=92,
      FF_PDF10=80,
      FF_PDF12=82,
      FF_GIF89a=1,
      FF_PNM=2,
      FF_PAM=3,
      FF_PIP=4, /* Portable Indexed file format by pts */
      FF_Empty=5,
      FF_Meta=6,
      FF_JPEG=7,
      FF_TIFF=8,
      FF_PNG=9,
      FF_XPM=10,
      FF_BMP=11,
      FF_XWD=12,
      FF_X11=13
    END_STATIC_ENUM()
    ff_t FileFormat;
    Image::sf_t SampleFormat;
    /** Used only in appliers.cpp when _check_rule() changes SampleFormat so
     * setSampleFormat() called before _work() would set that dummy
     * SampleFormat.
     */
    Image::sf_t origSampleFormat;
    BEGIN_STATIC_ENUM(unsigned char,te_t)
      TE_default=0, /* only for one-liner */
      TE_Binary=1,
      TE_ASCII=2,
      TE_Hex=3,
      TE_A85=4,
      TE_MSBfirst=5,
      TE_LSBfirst=6
    END_STATIC_ENUM()
    te_t TransferEncoding;
   
    BEGIN_STATIC_ENUM(unsigned char,co_t)
      CO_default=0, /* only for one-liner */
      CO_None=1,
      CO_LZW=2,
      CO_ZIP=3,
      CO_RLE=4,
      CO_Fax=5,
      CO_DCT=6,
      CO_IJG=7,
      CO_JAI=8,
      CO_max=8
    END_STATIC_ENUM()
    co_t Compression;

    BEGIN_STATIC_ENUM(unsigned char,pr_t)
      PR_None=1,
      PR_TIFF2=2,
      PR_PNGNone=10, PR_PNG=10,
      PR_PNGSub=11,
      PR_PNGUP=12,
      PR_PNGAverage=13,
      PR_PNGPaeth=14,
      PR_PNGAuto=15,
      PR_PNGAutoUnsigned=45
    END_STATIC_ENUM()
    pr_t Predictor;

    Image::Sampled::rgb_t Transparent;
    
    bool isOneBit() const;
    bool isPDF() const;
    bool isPDFB() const;
    bool isPS() const;
    bool isPSL2() const;
    bool isPSL3() const;
    bool hasPredictor() const;
    bool isIndexed() const;
    bool isTransparentM() const;
    bool isGray() const;
    bool isRGB() const;
    bool isDCTE() const;
    bool isBinSB() const;
    /** /Compression/ZIP not OK with /PSL1, /PSLC, /PSL2, /PDF10, /PDFB10 */
    bool isZIPOK() const;
    // bool is8() const;

    /** @param s is case insensitive, leading `/' is optional.
     * @return SF_max if not found, or a valid SF_... constant
     */
    static Image::sf_t parseSampleFormat(char const*s, slen_t slen);
    static char const* dumpFileFormat(ff_t FileFormat, co_t Compression=CO_default);
    static char const* dumpTransferEncoding(te_t TransferEncoding);
    static char const* dumpSampleFormat(Image::sf_t SampleFormat);
    static char const* dumpCompression(co_t Compression);
  };
  
  struct CacheHints {
    BEGIN_STATIC_ENUM(unsigned char,sc_t) /* Fri Aug 16 16:33:47 CEST 2002 */
      SC_default=0,
      SC_None=1,
      SC_OK=2,
      SC_RotateOK=3
    END_STATIC_ENUM()
    sc_t Scale;

    /* Image dimensions for encoding filters (may be different from real
     * image dimensions, but _really_ disrecommended).
     * slen_t important becasuse of separate().
     */
    slen_t EncoderColumns, EncoderBPL, EncoderRows;
    /** Number of color components (channels) per pixel. */
    unsigned char EncoderColors;
    slen_t PredictorColumns;
    unsigned char PredictorBPC, PredictorColors;
    unsigned ColorTransform;
    signed Effort;
    slen_t RecordSize;
    slendiff_t K;
    /* JPEG quality for the IJG encoding filter */
    unsigned char Quality;
    unsigned TransferCPL;
    MiniPS::String *Comment, *Title, *Subject, *Author, *Creator, *Producer;
    MiniPS::String *Created, *Produced;
    MiniPS::Dict *DCT;
    /** MiniPS::Real* or integer */
    MiniPS::VALUE TopMargin, BottomMargin, LeftMargin, RightMargin;
    /** MiniPS::Real* or integer. Specifies the dimension the image should
     * be lowered below the baseline for (PostScript or PDF) output with
     * /Scale/None. Default to 0.
     */
    MiniPS::VALUE LowerMargin;
    /** 72 by default. */
    MiniPS::VALUE ImageDPI;
    static char const* dumpScale(sc_t Scale);
  };

  struct OutputRule {
    Cache cache;
    CacheHints cacheHints;
    MiniPS::Dict* dict;
    /* /Hints entry in dict */
    MiniPS::Dict* dictHints;
    /* Number of this output rule, as appeared in .job. Imp: name */
    unsigned c;
    inline bool isEOL() const { return dictHints==NULLP; }
    /** In: dict, dictHints, cache and cacheHints is uninitialized. Out: they
     * get inititalized from param. Errors might occur and error messages
     * might get printed.
     */
    void fromDict(MiniPS::VALUE);
    /** Calls info->setSampleFormat(), and sets cacheHints.PredictorColumns
     * and some other fields if they are zero (unspecified in the .job file)
     */
    void doSampleFormat(Image::SampledInfo *info, bool separate=false);
    /** Appends "" when CO_None,
     * "<</K ... /Columns ...>>/CCITTFaxDecode filter" when CO_Fax
     * "/...Decode filter" otherwise
     * Calls appendPredictorSpec() if necessary.
     * For PDF (not PS!), implies appendTransferSpec().
     */
    void appendDecoderSpec(GenBuffer::Writable &out) const;
    /** Appends "" when PR_None or
     * appends "<</BitsPerComponent.../Colors.../Columns.../Predictor 2>>" or
     * appends "<</BitsPerComponent.../Colors.../Columns.../Predictor 10>>"
     * appends "<</K ...>>"
     */
    void appendPredictorSpec(GenBuffer::Writable &out) const;
    /** Appends "" or "/ASCII85Decode filter" or "/ASCIIHexDecode filter"
     */
    void appendTransferSpec(GenBuffer::Writable &out) const;
    bool hasPredictorSpec() const;
  };
  
  struct Applier {
    BEGIN_STATIC_ENUM(unsigned,cons_t)
      DONT_KNOW=0, BAD=1, MAYBE=2, OK=3
    END_STATIC_ENUM()
    /** Checks the specified OutputRule for internal consistency, updates
     * the `or->cache', emits warnings, and returns consistency status
     * information.
     *
     * -- If (this) Applier doesn't know how to handle the OutputRule, returns
     *    DONT_KNOW immediately.
     * -- If (this) Applier knows that the OutputRule is internally
     *    inconsistent or proved to be unsupported (i.e printing JPEG images
     *    on PostScript Level1 printers), regardless of the image itself, it
     *    returns BAD immediately and emits a warning about the reason.
     * -- If (this) Applier knows that the OutputRule may or may not be
     *    processed, depending on the actual image, then MAYBE is returned.
     * -- If (this) Applier knows that the OutputRule can be processed on
     *    any image, then OK is returned.
     */
    typedef cons_t(*check_rule_t)(OutputRule* or_);
    
    /** Outputs (writes) the PostScript or PDF presentation of the image
     * specified in param `sf' to the output stream in param `out', using
     * the Output Rule specified in param `or'.
     * @return DONT_KNOW if doesn't know how to handle the OutputRule;
     *         BAD if it is sure that the image cannot be written with the
     *             specified OutputRule;
     *         OK if writing was successful     
     */
    typedef cons_t(*work_t)(GenBuffer::Writable& out, OutputRule*or_, Image::SampledInfo *sf);
    
    /** A short, informal description of this Applier. */
    char const*format;
    check_rule_t check_rule;
    /** May be NULLP if `check_rule' never returns OK or MAYBE */
    work_t work;
    /** Null or next Applier to probe. Filled by register0(); */
    Applier *next;
  };
  
  /* Registers a new type of output Applier, i.e a new output file
   * format/method. The new output format will be put in front of all others,
   * and will be checked first by apply_best().
   */
  static void register0(Applier*);
  /* Loads the image contained in te file `filename'. */
  
  /** Builds the list of available profiles from the Profile element read
   * from a .job file. To do the task, calls check_rule() of all registered (!)
   * Appliers. Also calls OutputRule::fromDict for each profile element.
   * All returned OutputRules have an Applier which returned OK or MAYBE.
   * @return a dict==NULLP-terminated list of OutputRules
   */
  static OutputRule* buildProfile(MiniPS::VALUE Profile, bool quiet=false);

  /** Applies the Profile specified in param `rule_list' to the image
   * specified in param `sf', and writes the output to param `out'.
   */
  static void applyProfile(GenBuffer::Writable& out, OutputRule*rule_list, Image::SampledInfo *sf);
  
  /** Frees memory associated with a profile returned by buildProfile() */
  static void deleteProfile(OutputRule*);

  static unsigned printAppliers(GenBuffer::Writable &out);


  typedef void (*stream_writer_t)(GenBuffer::Writable&out, GenBuffer::Writable&outstream, Image::SampledInfo *sf);

  /** Writes a PS/PDF file to `out' according to `template_': substitutes
   * actual image (pixel) data from `sf->getImg()', and substitutes image
   * metadata from `sf'. `outstream' should be a prepared stream (top of chain
   * of Encode filters).
   *   See a .tte file for an example template.
   */
  static void writeTTE(
    GenBuffer::Writable& out,
    GenBuffer::Writable& outpal,
    GenBuffer::Writable& outstream,
    char const*template_,
    Rule::OutputRule*or_,
    Image::SampledInfo *sf,
    stream_writer_t stream_writer,
    char const*const*strings=(char**)NULLP);

  /** Doesn't invoke MiniPS::delete0(chunkArray); */
  static void writeTTM(
   Filter::VerbatimE &outve,
   GenBuffer::Writable&outpal,
   GenBuffer::Writable&outstream,
   MiniPS::Array *chunkArray,
   Rule::OutputRule*or_,
   Image::SampledInfo *sf,
   stream_writer_t stream_writer,
   char const*const*strings=(char**)NULLP);

  /** Used by writeTTT() to find template specification by name */
  static MiniPS::Dict* Templates;
  /** @param out must be of type Filter::VerbatimE if the template is a TTM
   * array.
   * @param template_key a key in file bts.ttt
   */
  static void writeTTT(
   GenBuffer::Writable&out,
   GenBuffer::Writable&outpal,
   GenBuffer::Writable&outstream,
   char const *template_key,
   Rule::OutputRule*or_,
   Image::SampledInfo *sf,
   stream_writer_t stream_writer,
   char const*const*strings=(char**)NULLP);

  static void writeData(GenBuffer::Writable& out, GenBuffer::Writable&outstream, Image::SampledInfo *sf);
  static void writePalData(GenBuffer::Writable& out, GenBuffer::Writable&outstream, Image::SampledInfo *sf);

};

#endif
