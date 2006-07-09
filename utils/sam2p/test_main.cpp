/* test_main.cpp
 * by pts@fazekas.hu at Sun Feb 24 16:24:38 CET 2002
 */

#include "gensi.hpp"
#include "gensio.hpp"
#include "image.hpp"
#include "error.hpp"
#include "encoder.hpp"
#include "minips.hpp"
#include "mapping.hpp"

#if OBJDEP
#  warning PROVIDES: test_main
#  warning REQUIRES: gensi.o
#  warning REQUIRES: gensio.o
#  warning REQUIRES: image.o
#  warning REQUIRES: error.o
#  warning REQUIRES: encoder.o
#  warning REQUIRES: minips.o
#  warning REQUIRES: mapping.o
#endif

#include <stdio.h>
#include <stdlib.h>

Files::FILEW sout(stdout);

#if OBJDEP
#  warning REQUIRES: in_xpm.o
#  warning REQUIRES: in_lbm.o
#  warning REQUIRES: in_gif.o
#  warning REQUIRES: in_bmp.o
#  warning REQUIRES: in_pnm.o
#  warning REQUIRES: in_tga.o
#endif
extern Image::Loader in_xpm_loader;
extern Image::Loader in_lbm_loader;
extern Image::Loader in_gif_loader;
extern Image::Loader in_bmp_loader;
extern Image::Loader in_pnm_loader;
extern Image::Loader in_tga_loader;
static void init_loader() {
  Image::register0(&in_tga_loader); /* checker not sure; install early */
  Image::register0(&in_xpm_loader);
  Image::register0(&in_lbm_loader);
  Image::register0(&in_gif_loader);
  Image::register0(&in_bmp_loader);
  Image::register0(&in_pnm_loader);
  sout << "Available Loaders:";
  Image::printLoaders(sout);
  sout << ".\n";
}

static void test_data() {
  int i;
  SimBuffer::Static world("World!");
  SimBuffer::Static("-1984").toInteger(i);
  sout << "Hello," << ' ' << world << '\n'
       << i << " luftballons.\n"
       << -99U << " negatives.\n";
  sout << (world >= "FooBar") << '\n'; /// true
  
  { bool b=true;
    SimBuffer::Linked sli; sli << (world>"Zorba");
    sli.toBool(b);
    sout << b << '\n'; /// false
  }

  SimBuffer::Linked li, lj("LJ");
  li << "answer is: " << 42L << '\n';
  li.prepend("The X", 4);
  lj << "uff.";
  lj=li;
  lj.prepend("D.A said: ",10);
  sout << li; /// The answer is 42
  sout << lj; /// D.A said: The answer is 42
  
  { Buffer b="Hello!\n"; /* <= 8 chars */
    sout << b;
  }
  { Buffer b="Hello, World!\n"; /* > 8 chars */
    sout << b;
  }
  { sout << "Hello,"+Buffer(" World2!\n"); }
  { Buffer b; sout << b.format("Hello,%cWorld %d%s", ' ', -42, "!\n"); }
  { Buffer b; sout << b.format("123456789012\n"); }
  { Buffer b; sout << b.format("1234567890123\n"); }
  { Buffer b; sout << b.format("12345678901234\n"); }
  { Buffer b; sout << b.format("123456789012345\n"); }
  { Buffer b; sout << b.format("1234567890123456\n"); }
  { Buffer b; sout << b.format("1234567890%u\n",123456789L); }
  { Buffer b("Hello, World!\n"); memcpy(b.substr_grow(7,5,3), "Bar", 3); sout << b; }
  { Buffer b("Hello, World!\n"); memcpy(b.substr_grow(7,5,6), "FooBar", 6); sout << b; }
  { Buffer b("Hello, World!\n"); memcpy(b.substr_grow(7,15,8), "FooBar!\n", 8); sout << b; }
  { Buffer b("Hello, World!"); memcpy(b.substr_grow(17,15,8), "HowDYD?\n", 8); sout << b; }
  { Buffer b="bena"; Buffer c; c=(SimBuffer::Flat)b; }
  sout << "Done.\n";
}

static void test_image() {
  // Image::Sampled *img=Image::load("at-logo.xpm");
  // Image::Sampled *img=Image::load("pts.xpm");
  // Image::Sampled *img=Image::load("tex5.xpm");
  // Image::Sampled *img=Image::load("at-logo.lbm");
  // Image::Sampled *img=Image::load("ptest.lbm");
  // Image::Sampled *img=Image::load("chessbrd.lbm");
  // Image::Sampled *img=Image::load("at-logo.gif");
  // Image::Sampled *img=Image::load("ptsbanner.gif");
  // Image::Sampled *img=Image::load("ptsbanner.bmp");
  // Image::Sampled *img=Image::load("pts.bmp");
  // Image::Sampled *img=Image::load("t.cpp");
  // Image::Sampled *img=Image::load("ptsbanner.tga");
  // Image::Sampled *img=Image::load("mixing.xpm");
  Image::Sampled *img=Image::load("mixing0.xpm", SimBuffer::B());
  Files::FILEW f(fopen("t.ppm","wb")); /* Imp: autoclose... */

  Image::Sampled *bak=img;
  img=img->toIndexed(); if (bak!=img) delete bak;
  assert(img->getTy()==img->TY_INDEXED);
  sout << "ncol0=" << PTS_dynamic_cast(Image::Indexed*,img)->getNcols() << '\n';
  PTS_dynamic_cast(Image::Indexed*,img)->packPal();
  sout << "ncols=" << PTS_dynamic_cast(Image::Indexed*,img)->getNcols() << '\n';
  PTS_dynamic_cast(Image::Indexed*,img)->setBpc(0);
  sout << "bpc=" << (int)img->getBpc() << '\n';
  // Files::tmpRemoveCleanup("t.dump");
  // Files::tmpRemoveCleanup("t.dump"); /* twice: OK */
  /* Files::tmpRemoveCleanup("/bin"); */
  Files::FILEW f2(fopen("t.dump","wb"));
  PTS_dynamic_cast(Image::Indexed*,img)->dumpDebug(f2);
  f2.close();

  f << *img;
  f.close();
  // delete img;
}

static void test_encoder() {
  // Files::tmpRemove=false; /* Imp: make this a command-line option */
  
  { Filter::FILEE a("t.a85");
    PSEncoder *bp=PSEncoder::newASCII85Encode(a,78);
    // FlateEncode c(b,5);
    // LZWEncode c(b);
    // PipeEncode c(b,"gs -dNODISPLAY -q -c '/i(%stdin)(r)file def/o(%stdout)(w)file/LZWEncode filter def/s 4096 string def{i s readstring exch o exch writestring not{exit}if}loop o closefile quit'");
    // GSEncode c(b,"xFlateEncode");
    // PSEncoder *cp=PSEncoder::newLZWEncode(*bp);
    #if 1
      PSEncoder *cp=PSEncoder::newRunLengthEncode(*bp);
      // delete [] cp; exit(0); /* Cannot free a GSEncode with multiple inheritance :-(( -> SegFault. virtual inheritance SUXX */
      // PSEncoder *cp=PSEncoder::newRunLengthEncode(a);
      
      // FILE *fin=fopen("t55","rb");
      FILE *fin=fopen("t.dump","rb");
      // FILE *fin=fopen("t.xin","rb");
      
      static char mybuf[2033];
      unsigned long sum=0;
      while (1) {
        unsigned size=fread(mybuf, 1, sizeof(mybuf), fin);
        if (size<1) { fprintf(stderr,"nosiz=%u\n",size); cp->vi_write(0,0); break; }
        fprintf(stderr, "sending size=%u ofs=%lu\n", size, sum);
        sum+=size;
        cp->vi_write(mybuf, size);
      }
      assert(feof(fin));
      assert(!ferror(fin));
      fprintf(stderr, "ftell=%lu\n", ftell(fin));
      fclose(fin);
    #else
      img->to8();
      //img->setBpc(0);
      //sout << "bpc2=" << (int)img->getBpc() << '\n';
      //Files::FILEW f2(fopen("t.dump2","wb"));
      //PTS_dynamic_cast(Image::Indexed*,img)->dumpDebug(f2);
      //f2.close();
      PSEncoder *cp=PSEncoder::newDCTEncode(*bp, img->getWd(), img->getHt(), 1);
      cp->vi_write(img->getRowbeg(), img->getWd()*img->getHt());
      cp->vi_write(0,0);
    #endif
    
    delete cp;
    delete bp;
  }
}

static void test_predictor() {
  /* by pts@fazekas.hu at Tue Mar 12 10:51:27 CET 2002 */
  Image::SampledInfo info(Image::load("mixing1.xpm", SimBuffer::B()));
  info.setSampleFormat(Image::SF_Indexed2, false, false, 0x1000000L);
  Image::Sampled *img=info.getImg();

  /* Imp: test predictors with rgb-4 */
  unsigned char pred=11;
  Files::FILEW f2(fopen("predictor_gen.eps","wb"));
  f2 << "%!PS-Adobe-3.0 EPSF-3.0\n%%BoundingBox: 0 0 " << img->getWd() << ' ' << img->getHt() << "\n%%EndComments\n"
     << "% `*image' forces grayscale :-(\nsave 99 dict begin\n"
        "{[ /Indexed /DeviceRGB " << ((img->getRowbeg()-img->getHeadp())/3-1) << "  currentfile " << (img->getRowbeg()-img->getHeadp()) << " string readstring pop ] setcolorspace\n"
        "/f currentfile <</Predictor " << (((unsigned)pred)%30) << " /Columns " << img->getWd() << " /Colors " << (unsigned)img->getCpp() << " /BitsPerComponent " << (unsigned)img->getBpc() << ">> /FlateDecode filter def\n"
//        "/f currentfile /FlateDecode filter def\n"
     << img->getWd() << ' ' << img->getHt() << ' ' << (unsigned)img->getBpc()
     << "[1 0 0 -1 0 " << img->getHt() << "]f image\nf closefile}\n"
        "%%BeginData:\nexec\n";
  f2.vi_write(img->getHeadp(), img->getRowbeg()-img->getHeadp()); /* Write palette */
  PSEncoder *bp=PSEncoder::newFlateEncode(f2);
  // Encoder *cp=Encoder::newTIFFPredictor2(*bp, img->getBpc(), img->getWd(),  img->getCpp());
  // Encoder *cp=Encoder::newPNGPredictor11(*bp, img->getBpc(), img->getWd(),  img->getCpp());
  Encoder *cp=PSEncoder::newPredictor(*bp, pred, img->getBpc(), img->getWd(),  img->getCpp());
  cp->vi_write(img->getRowbeg(), img->getRlen()*img->getHt());
  cp->vi_write(0,0);
  f2 << "\n%%EndData\nend\nrestore showpage\n%%Trailer\n%%EOF\n";
  f2.close();

  /* destructor of info frees all memory */
}

static void test_dump() {
  MiniPS::VALUE e=(MiniPS::VALUE)new MiniPS::Dict();
  MiniPS::VALUE d=(MiniPS::VALUE)new MiniPS::Dict();
  MiniPS::VALUE a=(MiniPS::VALUE)new MiniPS::Array();
  MiniPS::RARRAY(a)->push(MiniPS::Qinteger(3));
  MiniPS::VALUE b=(MiniPS::VALUE)new MiniPS::Array();
  MiniPS::RARRAY(b)->push(MiniPS::Qinteger(4));
  MiniPS::RARRAY(b)->push(MiniPS::Qinteger(5));
  MiniPS::RARRAY(b)->push(MiniPS::Qinteger(6));
  MiniPS::RARRAY(b)->push((MiniPS::VALUE)new MiniPS::String("Hel\téna \n\n\n\n\n\n\n",15));
#if 0
  MiniPS::RDICT(d)->push((MiniPS::VALUE)new MiniPS::Sname("/alma",5), MiniPS::Qinteger(42));
  MiniPS::RDICT(d)->push((MiniPS::VALUE)new MiniPS::Sname("/korte",6), MiniPS::Qnull);
  MiniPS::RDICT(d)->push((MiniPS::VALUE)new MiniPS::Sname("/szilva",7), MiniPS::Qtrue);
  MiniPS::RDICT(d)->push((MiniPS::VALUE)new MiniPS::Sname("/narancs",8), MiniPS::Qfalse);
  MiniPS::RDICT(d)->push((MiniPS::VALUE)new MiniPS::Sname("/naranc2",8), (MiniPS::VALUE)new MiniPS::Array());
  MiniPS::RDICT(d)->push((MiniPS::VALUE)new MiniPS::Sname("/naranc3",8), a);
  MiniPS::RDICT(d)->push((MiniPS::VALUE)new MiniPS::Sname("/naranc4",8), b);
  MiniPS::RDICT(d)->push((MiniPS::VALUE)new MiniPS::Sname("/naranc5",8), e);
#else
  MiniPS::RDICT(d)->push("/alma",5, MiniPS::Qinteger(42));
  MiniPS::RDICT(d)->push("/korte",6, MiniPS::Qnull);
  MiniPS::RDICT(d)->push("/szilva",7, MiniPS::Qtrue);
  MiniPS::RDICT(d)->push("/narancs",8, MiniPS::Qfalse);
  MiniPS::RDICT(d)->push("/naranc2",8, (MiniPS::VALUE)new MiniPS::Array());
  MiniPS::RDICT(d)->push("/naranc3",8, a);
  MiniPS::RDICT(d)->push("/naranc4",8, b);
  MiniPS::RDICT(d)->push("/naranc5",8, e);
#endif
  MiniPS::dump(sout, d);
  MiniPS::delete0(d);
}

static void test_tokenizer() {
  Files::FILER sin(stdin);
  MiniPS::Tokenizer pst(sin);
  int i;
  while ((i=pst.yylex())!=pst.EOFF) {
    putchar(i);
    putchar('\n');
  }
}

static void test_parser() {
  { MiniPS::Parser p("-"); /* STDIN */
    MiniPS::VALUE d=p.parse1();
    MiniPS::dump(sout, d);
    MiniPS::delete0(d);
  }
}


static void test_hash() {
  {Mapping::H h(6);
  h.set("alma",4,"apple");
  printf("alma=%s.\n", h.get("alma",4));
  printf("korte=%s.\n", h.get("korte",5));
  h.set("korte",5,"bear_");
  printf("alma=%s.\n", h.get("alma",4));
  printf("kort=%s.\n", h.get("korte",4));
  printf("korte=%s.\n", h.get("korte",5));
  }

  /* Dat: this might take a long time to run */  
  /* Imp: srand, but that would make the test less predictable */
  char tmp[2];
  unsigned i, j;
  Mapping::H h(sizeof(tmp));
  for (i=0; i<10000000; i++) {
    // printf("%u\n", i);
    for (j=0; j<sizeof(tmp); ) tmp[j++]=rand();
    if ((rand()&3)!=0) h.set(tmp, sizeof(tmp), tmp);
                  else h.deletee(tmp, sizeof(tmp));
    // putchar('.');
    // h.rehash();                   
  }
  printf("OK!\n");
}

static void test_decoder() {
  SimBuffer::B sb("Hello, World!\n");
  char const*p=sb();
  Filter::BufD bd(sb);
  Filter::PipeD pd(bd, "tr A-Z a-z <%S");
  int i;
  while ((i=pd.vi_getcc())>=0) {
    putchar(':');
    putchar(i);
    if (i!=(*p>='A' && *p<='Z'?*p+'a'-'A':*p)) abort();
    p++;
  }
  pd.vi_read(0,0);
  /* Dat: the destructors do their jobs; freeing is not necessary */
}

int main(int argc, char **argv) {
  (void)argc;
  (void)argv;
  
  Error::argv0=argc>=1 ? argv[0] : "test_main"; /* Imp: update this */
  Error::tmpargv0="test_main";
  Files::tmpRemove=0;
  init_loader();

  test_data();

  sout << "Success (test_main).\n";
  fflush(stdout);
  Error::cexit(0);
  assert(0);
  return 0; /*notreached*/
}
