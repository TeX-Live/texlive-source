/* encoder.hpp -- specific encoding filters
 * by pts@fazekas.hu at Tue Feb 26 13:30:02 CET 2002
 */

#ifdef __GNUC__
#pragma interface
#endif

#ifndef ENCODER_HPP
#define ENCODER_HPP 1

#include "config2.h"
#include "gensio.hpp"
#include <stdio.h>

class PSEncoder: public Encoder {
 public:
  /** @return "Flate", "LZW", "DCT", "RunLength", "CCITTFax", "ASCIIHex", "ASCII85" */
  // inline char const* getLongname() const { return longname; }
  /** @return "Fla", "LZW", "DCT", "RL", "CCF", "AHx", "A85" */
  // inline char const* getShortname() const { return shortname; }
  // inline SimBuffer::Flat const& getFPSname() const { return filter_psname; }
  // virtual void vi_write(char const*buf, slen_t len) =0;
  inline virtual ~PSEncoder() {}
  
  /* The class methods below may return different implementations according to
   * the command line and compile-time options.
   */
  static PSEncoder* newASCIIHexEncode(GenBuffer::Writable &out_,unsigned maxcpl_);
  static PSEncoder* newASCII85Encode(GenBuffer::Writable &out_,unsigned maxcpl_);
  /** Fax (PSL2 CCITTFaxEncode filter, Uncompressed=true! (parts of the data
   *    can be inserted uncompressed, when the decoder decides that this is
   *    advantageous), K=-1,0,positive,
   *    EndOfLine=false, EncodedByteAlign=false, Columns=..., Rows=0,
   *    EndOfBlock=true, BlackIs1=false, DamagedRowsBeforeError=0
   * K must be -1, 0 or positive, see PLRM 3.13.3
   * K==-1: Group 4 pure 2D encoding. TIFF COMPRESSION_CCITTFAX4,
   *   GROUP4OPT_UNCOMPRESSED.
   * K==0: Group 3 pure 1D encoding. TIFF COMPRESSION_CCITTFAX3,
   *   GROUP3OPT_UNCOMPRESSED.
   * K>=1: Group 3 2D (mixed 1D and 2D), a 1D-line can be followed by at most
   *   K-1 2D-line. TIFF COMPRESSION_CCITTFAX3, GROUP3OPT_UNCOMPRESSED,
   *   GROUP3OPT_2DENCODING (K is explicitly raised to image height).
   * @param EndOfLine should be false for PS/PDF, but must be true for TIFF.
   */
  static PSEncoder* newCCITTFaxEncode(GenBuffer::Writable &out_,slendiff_t K, slen_t Columns, bool EndOfLine=false, bool BlackIs1=false);
  /** PSL2 LZWEncode filter EarlyChange=true, UnitLength=8 LowBitFirst=false */
  static PSEncoder* newLZWEncode(GenBuffer::Writable &out_);
  /** PSL3 FlateEncode (ZIP) filter Effort=...: 1..9: 9==highest compression. -1==5  */
  static PSEncoder* newFlateEncode(GenBuffer::Writable &out_, signed Effort=-1);
  /** PSL2 RunLengthEncode filter, similar to TIFF PackBits, recordsize=... */
  static PSEncoder* newRunLengthEncode(GenBuffer::Writable &out_, slen_t RecordSize=0);
  /** PSL2 DCTEncode. Calls the cjpeg utility (in the debian package
   * libjpeg-progs) to do the task. The input must be RGB or Gray with
   * BitsPerComponent==8.
   */
  static PSEncoder* newDCTIJGEncode(GenBuffer::Writable &out_,
    slen_t Columns,
    slen_t Rows,
    unsigned char Colors, /*1..4*/
    unsigned char quality /*libJPEG quality: 0..100 */
  );
  /** @param other_parameters "" or something
   * like: "/HSamples [1 2 1]/ColorTransform 2"
   */
  static PSEncoder* newDCTEncode(GenBuffer::Writable &out_,
    slen_t Columns,
    slen_t Rows,
    unsigned char Colors, /*1..4*/
    unsigned char ColorTransform, /*0,1,2 3=default*/
    SimBuffer::Flat const& other_parameters
  );
  /** PSL2 DCTEncode. Not supported yet. Imp: build support */
  static PSEncoder* newDCTEncode(GenBuffer::Writable &out_,
    slen_t Columns,
    slen_t Rows,
    unsigned char Colors, /*1..4*/
    unsigned char quality=75, /*libJPEG quality: 0..100 */
    unsigned char const*HSamples=(unsigned char*)NULLP, /*all 1..4, NULLP OK*/
    unsigned char const*VSamples=(unsigned char*)NULLP, /*all 1..4, NULLP OK*/
    /* vvv pacify VC6.0; no unsigned char (*QuantTables)[64]=(unsigned char(*)[64])NULLP, */
    unsigned char (*QuantTables)[64]=0, /*NULLP OK*/
    double        QFactor=1.0, /*0..1000000*/
    unsigned      numHuffTables=0,
    unsigned char **HuffTables=(unsigned char**)NULLP, /*NULLP OK*/
    unsigned char ColorTransform=2 /*0,1,2 3=default*/
  );
  // static Encoder* newTIFFPredictor2(GenBuffer::Writable &out_, unsigned char bpc_, slen_t columns_, unsigned char cpp_);
  // static Encoder* newPNGPredictor11(GenBuffer::Writable &out_, unsigned char bpc_, slen_t columns_, unsigned char cpp_);
  /** @param type: 1: no predictor, 2: TIFFPredictor2, 10: PNGPredictor0 (None)
   *         11: PNGPredictor1 (Sub), 12: PNGPredictor2 (Up),
   *         13: PNGPredictor3 (Average), 14: PNGPredictor4 (Paeth),
   *         15: PNGPredictor5 (optimum).
   */
  static Encoder* newPredictor(GenBuffer::Writable &out_, unsigned char type, unsigned char bpc_, slen_t columns_, unsigned char cpp_);
 protected:
  // char const *longname, *shortname;
  // SimBuffer::B filter_psname;
};

#endif /* encoder.hpp */
