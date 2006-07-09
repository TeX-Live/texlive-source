// vcsam2p.cpp : Defines the entry point for the application.
//

#include "config2.h"
#include "image.hpp"
#include "error.hpp"
#include "main.hpp" /* init_loader(), init_applier() */
#include "stdafx.h"
#include "resource.h"

#define MAX_LOADSTRING 100

extern "C" int main(int argc, char**argv);

#if 0
extern "C" int getpid(void);

//#include <windows.h> /* OSVERSIONINFO etc. */
//#include <process.h> /* _getpid() */
static unsigned long win32_os_id(void) {
  static OSVERSIONINFO osver;
  static DWORD w32_platform = (DWORD)-1;
  if (w32_platform==-1 || osver.dwPlatformId != w32_platform) {
    memset(&osver, 0, sizeof(OSVERSIONINFO));
    osver.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
    GetVersionEx(&osver);
    w32_platform = osver.dwPlatformId;
  }
  return (unsigned long)w32_platform;
}
static inline bool IsWin95(void) {
  return (win32_os_id() == VER_PLATFORM_WIN32_WINDOWS);
}

// static int win32_getpid(void) {
int getpid(void) {
  int pid;
  pid = _getpid();
  /* Windows 9x appears to always reports a pid for threads and processes
   * that has the high bit set. So we treat the lower 31 bits as the
   * "real" PID for Perl's purposes. */
  if (IsWin95() && pid < 0) pid = -pid;
  return pid;
}
#endif


// Global Variables:
HINSTANCE hInst;								// current instance
// TCHAR szTitle[MAX_LOADSTRING];								// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];								// The title bar text

// Foward declarations of functions included in this code module:
ATOM			MyRegisterClass(HINSTANCE hInstance);
BOOL			InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	dialogRun(HWND, UINT, WPARAM, LPARAM);

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE /*hPrevInstance*/,
                     LPSTR     /*lpCmdLine*/, // !!
                     int       nCmdShow) {
 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	// LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_VCSAM2P, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow)) {
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_VCSAM2P);

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0)) {
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	return msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage is only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance) {
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX); 
	wcex.style		= 0; /*CS_HREDRAW | CS_VREDRAW;*/
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon		= LoadIcon(hInstance, (LPCTSTR)IDI_SAM2P32);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= (LPCSTR)IDC_VCSAM2P;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SAM2P16);
	return RegisterClassEx(&wcex);
}

/* --- */

/** An image that can be rendered onto a window really quickly */
class WinFastImage {
  HDC hmemdc;
  HBITMAP hbitmap;
  slen_t wd, ht;
 public:
  // WinFastImage::WinFastImage(slen_t wd_, slen_t ht_);
  WinFastImage();
  ~WinFastImage();
  /** Constructs a WinFastImage that can be drawn fast onto hdc. */
  WinFastImage(HDC hdc, HBITMAP hsrc);
  /** Constructs a WinFastImage that can be drawn fast onto hwnd and all compatible HDCs. */
  WinFastImage(HWND hwnd, HBITMAP hsrc);
  slen_t getWd() const { return wd; }
  slen_t getHt() const { return ht; }
  /* @param hdc dc on which fast rendering is desired
   * @param hsrc source bitmap containing the data
   */
  void set(HDC hdc, HBITMAP hsrc);
  inline void drawTo(HDC hdc, slen_t dstX, slen_t dstY) const {
    drawTo(hdc, dstX, dstY, wd, ht, 0, 0);
  }
  inline void drawTo(HDC hdc, slen_t dstX, slen_t dstY, slen_t dstWd, slen_t dstHt, slen_t srcX, slen_t srcY) const {
    BitBlt(hdc,dstX,dstY,dstWd,dstHt,hmemdc,srcX,srcY,SRCCOPY);
  }
  // void drawTo(HDC hdc, slen_t dstX, slen_t dstY);
  // void drawTo(HDC hdc, slen_t dstX, slen_t dstY, slen_t dstWd, slen_t dstHt, slen_t srcX, slen_t srcY);
  /** MSDN says hbitmap must be a DDB, but it worked fine for me with a DIB */
  static void getSize(HBITMAP hbitmap, slen_t &wd, slen_t &ht);
};

void WinFastImage::getSize(HBITMAP hbitmap, slen_t &wd, slen_t &ht) {
  /* It took me 1 hour to figure this out. Not documented anywhere! */
  BITMAPINFOHEADER h2;
  h2.biSize=sizeof(h2);
  h2.biBitCount=0;
  // h2.biWidth=11; h2.biHeight=22; h2.biPlanes=1;
  HDC hxdc=CreateDC("DISPLAY",NULL,NULL,NULL);  
  GetDIBits(hxdc, hbitmap, 0, 0, NULL, (BITMAPINFO*)&h2, DIB_RGB_COLORS);
  wd=h2.biWidth; ht=h2.biHeight;
  DeleteDC(hxdc);
}

WinFastImage::WinFastImage(): hmemdc(NULL), hbitmap(NULL), wd(0), ht(0) {}
WinFastImage::WinFastImage(HDC hdc, HBITMAP hsrc): hmemdc(NULL), hbitmap(NULL) { set(hdc, hsrc); }
WinFastImage::WinFastImage(HWND hwnd, HBITMAP hsrc): hmemdc(NULL), hbitmap(NULL) {
  HDC hdc=GetDC(hwnd);
  set(hdc, hsrc);
  ReleaseDC(hwnd, hdc);
}

WinFastImage::~WinFastImage() {
  if (hmemdc!=NULL) DeleteDC(hmemdc);
  if (hbitmap!=NULL) DeleteObject(hbitmap);
}
void WinFastImage::set(HDC hdc, HBITMAP hsrc) {
  if (hmemdc!=NULL) DeleteDC(hmemdc);
  if (hbitmap!=NULL) DeleteObject(hbitmap);
  /* Imp: error handling */
  getSize(hsrc, wd, ht);
  hmemdc=CreateCompatibleDC(hdc);
  hbitmap=CreateCompatibleBitmap(hdc, wd, ht); /* BUGFIX at 22:07 */
  DeleteObject(SelectObject(hmemdc, hbitmap));
  HDC hsrcdc=CreateCompatibleDC(hmemdc);
  SelectObject(hsrcdc, hsrc);
  BitBlt(hmemdc,0,0,wd,ht,hsrcdc,0,0,SRCCOPY);
  DeleteDC(hsrcdc);
}

/* --- */

class CopyableImage { public:
  virtual slen_t vi_getWd() const =0;
  virtual slen_t vi_getHt() const =0;
  /** Copies an row in RGB PPM8 format from (this) to `to' */
  virtual void copyRGBRow(char *to, slen_t whichRow) const =0;
};

/** A rectangular RGB image, suitable for displaying in Win32 API */
class WinImageRGB {
  /** Number of bytes per scanline */
  slen_t rlen;
  HBITMAP hbitmap;
  /** data[0] is blue of upper left pixel, data[1] is green, data[2] is red. Rows
   * are aligned to 4-byte boundary
   */
  char *data;
  BITMAPINFOHEADER header;
  void init(slen_t wd_, slen_t ht_);
 public:
  inline WinImageRGB(slen_t wd_, slen_t ht_) { init(wd_, ht_); }
  WinImageRGB(CopyableImage const&);
  ~WinImageRGB();
  inline slen_t getWd() const { return header.biWidth; }
  inline slen_t getHt() const { return -header.biHeight; }
  virtual inline slen_t vi_getWd() const { return header.biWidth; }
  virtual inline slen_t vi_getHt() const { return -header.biHeight; }
  virtual void copyRGBRow(char *to, slen_t whichRow) const;
  inline slen_t getRlen() const { return rlen; }
  inline char* getData() const { return data; }
  inline HBITMAP getHbitmap() const { return hbitmap; }
  void drawTo(HDC hdc, slen_t dstX, slen_t dstY) const;
  void drawTo(HDC hdc, slen_t dstX, slen_t dstY, slen_t dstWd, slen_t dstHt, slen_t srcX, slen_t srcY) const;
  /** Copies image data from an other image in the PPM8 format: ppm8_data[0]
   * is red, ppm8_data[1] is green, pp8_data[2] is blue. Rows are aligned to
   * byte boundary.
   */
  void fromPPM8(char const*ppm8_data);
  void fill(char r, char g, char b);
  void putPixel(slen_t x, slen_t y, char r, char g, char b);
};

void WinImageRGB::init(slen_t wd_, slen_t ht_) {
  rlen=(wd_*3+3)&~3;
  header.biSize=sizeof(header);
  header.biWidth=wd_;
  header.biHeight=0-ht_;
  header.biPlanes=1;
  header.biBitCount=24;
  header.biCompression=BI_RGB;
  header.biSizeImage=0;
  header.biXPelsPerMeter=1000; /* returned by GetBitmapDimensionEx */
  header.biXPelsPerMeter=1000;
  header.biClrUsed=0;
  header.biClrImportant=0;
  hbitmap=CreateDIBSection(NULL, (CONST BITMAPINFO*)&header, DIB_RGB_COLORS, (void**)&data, NULL, 0);
//  slen_t xx=GetDeviceCaps(hbitmap, HORZRES);
}

WinImageRGB::WinImageRGB(CopyableImage const& other) {
  init(other.vi_getWd(), other.vi_getHt());
  char *buf=new char[rlen];
  slen_t pad=(header.biWidth)&3, x, y=0, ym=header.biHeight;
  char *p=data;
  char const *ppm8_data;
  while (ym++!=0) {
    other.copyRGBRow(buf, y++);  ppm8_data=buf;
    x=header.biWidth;
    while (x--!=0) { p[2]=*ppm8_data++; p[1]=*ppm8_data++; p[0]=*ppm8_data++; p+=3; }
    p+=pad;
  }
  delete buf;
}

WinImageRGB::~WinImageRGB() {
  DeleteObject(hbitmap);
}

void WinImageRGB::copyRGBRow(char *to, slen_t whichRow) const {
  if (whichRow<0U-header.biHeight) {
    char *p=data+rlen*whichRow, *pend=p+rlen;
    while (p!=pend) { to[0]=p[2]; to[1]=p[1]; to[2]=p[0]; p+=3; to+=3; }
  }
}

void WinImageRGB::drawTo(HDC hdc, slen_t dstX, slen_t dstY) const {
  drawTo(hdc, dstX, dstY, header.biWidth, -header.biHeight, 0, 0);
}

void WinImageRGB::drawTo(HDC hdc, slen_t dstX, slen_t dstY, slen_t dstWd, slen_t dstHt, slen_t srcX, slen_t srcY) const {
  HDC hmemdc=CreateCompatibleDC(hdc);
  DeleteObject(SelectObject(hmemdc, hbitmap));
  /* ^^^ set the bitmap for hmemdc that Rectangle (hmemdc etc.) will draw to, to (this) */

  // slen_t xx=GetDeviceCaps(hdc, HORZRES);
  // HPEN hpen=CreatePen(PS_SOLID, 0, RGB(255,0,0));
  // HANDLE hold=SelectObject(hmemdc, hpen);
  // Rectangle(hmemdc,10,10,200,100);
  // SelectObject(hmemdc, hold);
  // DeleteObject(hpen);

  BitBlt(hdc,dstX,dstY,dstWd,dstHt,hmemdc,srcX,srcY,SRCCOPY);
  DeleteDC(hmemdc);
}

void WinImageRGB::fromPPM8(char const* ppm8_data) {
  /* Dat: biWidth*3 is considered when calculating pad */
  slen_t pad=(header.biWidth)&3, x, y=header.biHeight;
  char *p=data;
  while (y++!=0) {
    x=header.biWidth;
    while (x--!=0) { p[2]=*ppm8_data++; p[1]=*ppm8_data++; p[0]=*ppm8_data++; p+=3; }
    p+=pad;
  }
}

void WinImageRGB::fill(char r, char g, char b) {
  /* Dat: biWidth*3 is considered when calculating pad */
  slen_t pad=(header.biWidth)&3, x, y=header.biHeight;
  char *p=data;
  while (y++!=0) {
    x=header.biWidth;
    while (x--!=0) { *p++=b; *p++=g; *p++=r; }
    p+=pad;
  }  
}

void WinImageRGB::putPixel(slen_t x, slen_t y, char r, char g, char b) {
  if (x<(slen_t)header.biWidth && y<(slen_t)-header.biHeight) {
    char *p=data+rlen*y+x;
    p[0]=b; p[1]=g; p[2]=r;
  }
}

/* --- */


/** Implements the CopyableImage interface for the sam2p Image::Sampled class. */
class CopyableAdapter: public CopyableImage {
  Image::Sampled const& img;
 public:
  inline CopyableAdapter(Image::Sampled const& img_): img(img_) {}
  inline virtual slen_t vi_getHt() const { return img.getHt(); }
  inline virtual slen_t vi_getWd() const { return img.getWd(); }
  inline virtual void copyRGBRow(char *to, slen_t whichRow) const { img.copyRGBRow(to, whichRow); }
};

/* --- */

/** Size of previous open image */
static slen_t wd_last=0, ht_last=0;
/** Size of window including decorations !! exclude decorations */
static const unsigned wd_def=150, ht_def=90;
static WinImageRGB *imgr=NULL;
static WinFastImage *imgf=NULL;
static HWND hwnd_main;
/** Changed since last save? */
static bool dirty=false;
/** File loaded. NULL if no/new; begins with "* " for a dirty file, ": " for a normal file */
static char *filename2=NULL;
static bool first_paint_p=true;
// static char szHello[MAX_LOADSTRING];
/* OK: not resizable: WM_... */
/* Imp: clipboard copy-paste image */
/* !! about editbox non-grey disabled */
/* !! open rxvt_bug.bmp wrong window size */
/* !! test larger image than screen */
/* !! SetWindowText() LineScroll() */
/* !! bongeszo ne irjon felul ablakot */
/* !! transparent images */
/* void CHistoryEdit::AppendString(CString str) 
                      { SendMessage(EM_SETSEL,0xFFFFFFFF,-1); 
                        SendMessage(EM_REPLACESEL,FALSE,(LPARAM)(LPCTSTR)str); 
                      }
*/

//   PURPOSE: Saves instance handle and creates main window
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow) {
   init_sam2p_engine("vcsam2p");
   hInst = hInstance; // Store instance handle in our global variable
   hwnd_main = CreateWindow(szWindowClass, Error::banner0,
      WS_OVERLAPPED| WS_CAPTION| WS_SYSMENU| WS_MINIMIZEBOX| WS_BORDER| WS_THICKFRAME,
      // WS_THICKFRAME: resizable (has sizing border)
      // WS_MAXIMIZEBOX, WS_OVERLAPPEDWINDOW
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);
   if (!hwnd_main) return FALSE;
   SetWindowPos(hwnd_main, NULL, 0, 0, wd_def, ht_def, SWP_NOCOPYBITS|SWP_NOMOVE|SWP_NOOWNERZORDER|SWP_NOZORDER);
   #if 0
     imgr=new WinImageRGB(226, 33);
     if (imgr==NULL) return FALSE;
     imgr->fill(0,0,(char)255);
     HDC hdc_main=GetDC(hwnd_main);
     imgf=new WinFastImage(hdc_main, imgr->getHbitmap());
   #endif
   ShowWindow(hwnd_main, nCmdShow);
   // ReleaseDC(hwnd_main, hdc_main);
   UpdateWindow(hwnd_main);
   // imgr->fill((char)255,0,0);
   return TRUE;
}

static void do_ask_save_as() {
  // !! implement this
}

/** @return true iff save successful */
static bool do_save(bool ask_save_as) {
  /* !! implement this */
  if (!dirty) return true;
  if (filename2==NULL && ask_save_as) do_ask_save_as();
  if (filename2==NULL) return false;
  if (IDYES==MessageBox(hwnd_main, "Saving... Success?", NULL, MB_YESNO)) {
    dirty=false;
    return true;
  }
  return false;
}

static void do_save_as(void) {
  char *old_filename2=filename2;
  filename2=NULL;
  do_ask_save_as();
  if (filename2!=NULL) {
    if (do_save(false)) { delete [] old_filename2; return; }
    delete [] filename2;
  }
  filename2=old_filename2;
}

/** no@@return true iff should exit */
static void do_exit(void) {
  if (dirty) {
    int i=MessageBox(hwnd_main, "Save changes before exit?", "Confirm exit",
      MB_YESNOCANCEL|MB_ICONEXCLAMATION|MB_DEFBUTTON1);
    if (i==IDCANCEL || (i==IDYES && !do_save(true))) return;
  }
  DestroyWindow(hwnd_main);
}

static void getFrameSize(HWND hwnd, LONG &wd_ret, LONG &ht_ret) {
  RECT wr; GetWindowRect(hwnd, &wr);
  RECT cr; GetClientRect(hwnd, &cr);
  wd_ret=(wr.right-wr.left)-(cr.right-cr.left);
  ht_ret=(wr.bottom-wr.top)-(cr.bottom-cr.top);
}

extern Image::Loader in_bmp_loader;

struct SamArgs {
  typedef void* (WINAPI*samfun_t)(SamArgs const*);
};

struct SamImageLoad: public SamArgs {
  char const *filename;
  SimBuffer::Flat *loadHints;
};

void* WINAPI do_image_load(SamImageLoad const* args) {
  // Error::sev((Error::level_t) 2) << "a	lma" << (Error*)0;
  return Image::load(args->filename, *(args->loadHints));
}

void* do_samfun(SamArgs::samfun_t samfun, SamArgs const* args) {
  SimBuffer::B log;
  GenBuffer::Writable *old_serr=Error::serr;
  Error::pushPolicy((Error::level_t)0, (Error::level_t)-99, (Error::level_t)-199, &log); // Error::serr=&log;
  HANDLE hthread;
  DWORD res=0, tid;
  /* printf("Creating thread.\n"); */
  if (NULL==(hthread=CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)samfun, (void*)args, CREATE_SUSPENDED, &tid))) {
    log << "CreateThread() failed\n";
   on_error:
    Error::serr=old_serr;
    log.term0();
    MessageBox(hwnd_main, log(), Error::banner0, 0);
    return NULL;
  }
  /* printf("Resuming thread (h=0x%x tid=0x%x).\n", hthread, tid); */
  if (0==1+ResumeThread(hthread)) {
    log << "ResumeThread() failed\n";
   on_thread_error:
    TerminateThread(hthread, 0);
    CloseHandle(hthread);
    goto on_error;
  }
  /* printf("Waiting for thread.\n"); */
  if (WAIT_FAILED==WaitForSingleObject(hthread, INFINITE)) { log << "WaitForSingleObject() failed\n"; goto on_thread_error; }
  if (0==GetExitCodeThread(hthread, &res)) { log << "GetExitCodeThread() failed\n"; goto on_thread_error; }
  Error::popPolicy();
  CloseHandle(hthread);
  Error::serr=old_serr;
  // !! vvv 0 == NULL, exit(1), exit(2) is
  if (res+0U<10U) { /* log << "return(NULL);\n1\n"; */ goto on_error; }
  /* printf("Done, thread has returned=%u\n", res); */
  return (void*)res;
}


/** Load image from disk, update imgr and imgf
 * @return true on success
 */
static bool do_open_image(char const* filename) {
  // MessageBox(NULL, filename, "opening image", 0);
  if (imgf!=NULL) {
    if (imgr!=NULL) { delete imgr; imgr=NULL; }
    delete imgf; imgf=NULL;
    if (filename2!=NULL) filename2[0]='-'; /* mark file closed */
  }

  // !! free
  // !! check for errors
  // !! generalize

  SimBuffer::B loadHints;
  #if 0
  Image::Loader::reader_t reader=in_bmp_loader.checker(
    "BM\0\0\0\0" "\0\0\0\0"
    "\0\0\0\0" "\0\0\0\0", "", opt);
  FILE *f=fopen(filename, "rb");
  if (f==NULL) return false;
  Image::Sampled *img=reader((Image::filep_t)f, loadHints);
  fclose(f);
  #endif

  init_loader();
  SamImageLoad args;
  args.filename=filename;
  args.loadHints=&loadHints;
  Image::Sampled *img=(Image::Sampled*)do_samfun((SamArgs::samfun_t)do_image_load, &args);
  if (img==NULL) return false;
  // Image::load(filename, loadHints);

  imgr=new WinImageRGB(CopyableAdapter(*img));
  // Image::Sampled *imgrgb=img->toRGB(8);
  // imgr=new WinImageRGB(img->getWd(), img->getHt());
  // imgr->fill(0,0,(char)255);
  // imgr->fromPPM8(imgrgb->getRowbeg());
  assert(imgr!=NULL); /* `operator new' never returns NULL */
  // HDC hdc_main=GetDC(hwnd_main);
  imgf=new WinFastImage(hwnd_main, imgr->getHbitmap());
  wd_last=imgf->getWd(); ht_last=imgf->getHt();
  // ReleaseDC(hwnd_main, hdc_main);

  slen_t len=strlen(filename);
  if (filename2!=NULL) delete [] filename2;
  strcpy(filename2=new char[len+3], ": ");
  strcpy(filename2+2, filename);
  SetWindowText(hwnd_main, filename2);
  SetWindowLong(hwnd_main, GWL_STYLE, GetWindowLong(hwnd_main, GWL_STYLE)&~WS_THICKFRAME);
  /* ^^^ The Win32 API way to say hwnd_main.setResizable(false); :-) */
  InvalidateRect(hwnd_main, NULL, TRUE);
#if 0  
  LONG fr_wd, fr_ht;
  getFrameSize(hwnd_main, fr_wd, fr_ht);
  // !! handle minimum window size
  SetWindowPos(hwnd_main, NULL, 0, 0, fr_wd+imgf->getWd(), fr_ht+imgf->getHt(), SWP_NOCOPYBITS|SWP_NOMOVE|SWP_NOOWNERZORDER|SWP_NOZORDER);
  GdiFlush();
  { /* Dat: strange, two iterations of getFrameSize + SetWindowPos is needed !! still bad */
    getFrameSize(hwnd_main, fr_wd, fr_ht);
    SetWindowPos(hwnd_main, NULL, 0, 0, fr_wd+imgf->getWd(), fr_ht+imgf->getHt(), SWP_NOCOPYBITS|SWP_NOMOVE|SWP_NOOWNERZORDER|SWP_NOZORDER);
  }
BOOL CYourWndOrDialog::ResizeClient (int nWidth,  int nHeight, BOOL bRedraw) {
                       RECT rcWnd;
                       GetClientRect (&rcWnd);
                       if(nWidth != -1)
                        rcWnd.right = nWidth;
                       if(nHeight != -1)
                        rcWnd.bottom = nHeight;
                       if(!::AdjustWindowRectEx(&rcWnd, 
                                                GetStyle(), 
                                                (!(GetStyle() & WS_CHILD) 
                       && GetMenu() != NULL)), GetExStyle()))
                        return FALSE;
                       UINT uFlags = SWP_NOZORDER | SWP_NOMOVE;
                       if(!bRedraw)
                        uFlags |= SWP_NOREDRAW;
                       return SetWindowPos(NULL, 
                                           0, 0, 
                                           rcWnd.right - rcWnd.left, 
                                           rcWnd.bottom - rcWnd.top, 
                                           uFlags);
                      } // CYourWndOrDialog::ResizeClient
#endif
  RECT rt;
  rt.left=0; rt.right=imgf->getWd();
  rt.top=0;  rt.bottom=imgf->getHt();
  AdjustWindowRect(&rt, GetWindowLong(hwnd_main, GWL_STYLE), TRUE); // !! true
  SetWindowPos(hwnd_main, NULL, 0,0, rt.right-rt.left, rt.bottom-rt.top,
    SWP_NOCOPYBITS|SWP_NOMOVE|SWP_NOOWNERZORDER|SWP_NOZORDER);
  return true;
}

static char const* loadImageFilter=
  "all loadable images\0*.tiff;*.tif;*.jpeg;*.jpg;*.gif;*.png;*.pnm;*.pgm;*.ppm;*.xpm;*.lbm;*.tga;*.pcx;*.bmp\0"
  "TIFF images\0*.tiff;*.tif\0"
  "JPEG images\0*.jpeg;*.jpg\0"
  "GIF images\0*.gif\0"
  "PNG images\0*.png\0"
  "PNM images\0*.pnm;*.pbm;*.pgm;*.ppm\0"
  "PBM images\0*.pbm\0"
  "PGM images\0*.pgm\0"
  "PPM images\0*.ppm\0"
  "XPM images\0*.xpm\0"
  "IFF ILBM images\0*.lbm\0"
  // "EPS figures\0*.eps\0"
  // "PostScript documents\0*.ps\0"
  // "PDF figures\0*.pdf\0"
  // "XWD screen shots\0*.xwd\0"
  // "PAM images\0*.pam\0"
  // "Image meta information\0*.meta\0"
  "Targa TGA images\0*.tga\0"
  "PCX images\0*.pcx\0"
  "Windows BMP bitmaps\0*.bmp\0"
  "All files (*)\0*\0";

static char const* saveImageFilter=
  "all saveable images\0*.tiff;*.tif;*.jpeg;*.jpg;*.gif;*.png"
    ";*.pnm;*.pgm;*.ppm;*.xpm;*.lbm;*.tga;*.pcx;*.bmp;*.eps;*.ps;*.pdf;*.xwd;*.pam;*.meta\0"
  "TIFF images\0*.tiff;*.tif\0"
  "JPEG images\0*.jpeg;*.jpg\0"
  "GIF images\0*.gif\0"
  "PNG images\0*.png\0"
  "PNM images\0*.pnm;*.pbm;*.pgm;*.ppm\0"
  "PBM images\0*.pbm\0"
  "PGM images\0*.pgm\0"
  "PPM images\0*.ppm\0"
  "XPM images\0*.xpm\0"
  "IFF ILBM images\0*.lbm\0"
  "EPS figures\0*.eps\0"
  "PostScript documents\0*.ps\0"
  "PDF figures\0*.pdf\0"
  "XWD screen shots\0*.xwd\0"
  // "PAM images\0*.pam\0"
  "Image meta information\0*.meta\0"
  "Targa TGA images\0*.tga\0"
  "PCX images\0*.pcx\0"
  "Windows BMP bitmaps\0*.bmp\0"
  "All files (*)\0*\0";

static void do_open(void) {
  OPENFILENAME ofn;
  // ZeroMemory(&ofn, sizeof(ofn));
  char szFileName[MAX_PATH];
  szFileName[0]='\0';
  ofn.lpstrTitle="Open image with sam2p";
  ofn.lStructSize = sizeof(ofn); // SEE NOTE BELOW
  ofn.hwndOwner = NULL; // hwnd;
  ofn.lpstrFilter=loadImageFilter;
  ofn.lpstrDefExt=NULL;
  ofn.lpstrFile = szFileName;
  ofn.nMaxFile = MAX_PATH;
  ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST /*|OFN_HIDEREADONLY*/
    |OFN_EXTENSIONDIFFERENT /*|OFN_NODEREFERENCELINKS*/ |OFN_READONLY;
  ofn.lpstrDefExt = "txt";
  ofn.hInstance=NULL;
  ofn.lpstrCustomFilter=NULL; // !!
  ofn.nMaxCustFilter=0; // !! 
  ofn.nFilterIndex=0; // use Custom Filter
  ofn.lpstrFileTitle=NULL;
  ofn.nMaxFileTitle=0;
  ofn.lpstrInitialDir=NULL; // use current directory
  ofn.nFileOffset=0;
  ofn.nFileExtension=0;
  ofn.lCustData=NULL;
  ofn.lpfnHook=NULL;
  ofn.lpTemplateName=NULL;
  // MessageBox(NULL, "hello", "world", 0);
  if (GetOpenFileName(&ofn)) {
    do_open_image(szFileName);
  }
}

static void do_new(void) {
  if (imgf!=NULL) {
    if (filename2!=NULL) filename2[0]='-';
    if (imgr!=NULL) { delete imgr; imgr=NULL; }
    delete imgf; imgf=NULL;
    #if 0
      if (filename2!=NULL) { delete [] filename2; filename2=NULL; }
      SetWindowText(hwnd_main, Error::banner0);
    #endif
    SetWindowLong(hwnd_main, GWL_STYLE, GetWindowLong(hwnd_main, GWL_STYLE)|WS_THICKFRAME);
    /* ^^^ The Win32 API way to say hwnd_main.setResizable(true); :-) */
    InvalidateRect(hwnd_main, NULL, TRUE);
    // SetWindowPos(hwnd_main, NULL, 0, 0, wd_def, ht_def, SWP_NOCOPYBITS|SWP_NOMOVE|SWP_NOOWNERZORDER|SWP_NOZORDER);
    /* ^^^ Do _not_ revert to default window size */
    SendMessage(hwnd_main, WM_SIZE, 0, 0); /* calls updateBgText() */
  } else if (wd_last!=0 || ht_last!=0) {
    if (filename2!=NULL) { delete [] filename2; filename2=NULL; }
    SetWindowText(hwnd_main, Error::banner0);
    wd_last=0; ht_last=0;
    SendMessage(hwnd_main, WM_SIZE, 0, 0); /* calls updateBgText() */
  }
}

/** Rectangle to draw the background text into */
static RECT bgRect={0,0,0,0};
static char bgText[128];

/** @param hwnd hwnd_main */
static void updateBgText(HWND hwnd, slen_t x, slen_t y) {
  RECT rt;
  HDC hdc=GetDC(hwnd);
  GetClientRect(hwnd, &rt); bgRect=rt;
  sprintf(bgText, "%s\n%lu\327%lu", Error::banner0, x+0UL, y+0UL);
  // DrawText(hdc, msg, strlen(msg), &rt, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
  int textht=DrawText(hdc, bgText, -1, &rt, DT_CENTER|DT_CALCRECT);
  bgRect.top+=(bgRect.bottom-bgRect.top-textht)/2;
  ReleaseDC(hwnd, hdc);
}

//void CDECL AfxTrace(LPCTSTR lpszFormat, ...);

/** Processes messages for the main window. */
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
  PAINTSTRUCT ps;
  HDC hdc;
  RECT *rp, rt;
  int i;
  switch (message) {
   case WM_PAINT:
    if (first_paint_p) {
      first_paint_p=false;
      // LoadString(hInst, IDS_HELLO, szHello, MAX_LOADSTRING);
    }
    hdc = BeginPaint(hwnd, &ps);
    if (imgf==NULL) { /* no image is loaded yet */
      DrawText(hdc, bgText, -1, &bgRect, DT_CENTER);
    } else {
      GetClientRect(hwnd, &rt);
      bool xbad=0U+rt.right-rt.left>imgf->getWd(), ybad=0U+rt.bottom-rt.top>imgf->getHt();
      if (ps.fErase && (xbad || ybad)) {
        /* Erase only region not part of the image; to avoid flicker */
        // HDC hdc=GetDC(hwnd);
        // HBRUSH hbrush=(HBRUSH)GetClassLong(hwnd, GCL_HBRBACKGROUND);
	HBRUSH hbrush=(HBRUSH)GetStockObject(WHITE_BRUSH);
        HGDIOBJ holdbrush=SelectObject(hdc, hbrush);
	HPEN hpen=(HPEN)GetStockObject(WHITE_PEN); /* always used */
        HGDIOBJ holdpen=SelectObject(hdc, hpen);
        Rectangle(hdc, imgf->getWd(), rt.top, rt.right, rt.bottom);
        Rectangle(hdc, rt.left, imgf->getHt(), imgf->getWd(), rt.bottom);
        SelectObject(hdc, holdpen);
        SelectObject(hdc, holdbrush);
        // ReleaseDC(hwnd, hdc);
      }
      // if (imgf==NULL) imgf=new WinFastImage(hdc, imgr->getHbitmap());
      // rt.right-=rt.left; rt.bottom-=rt.top;
      imgf->drawTo(hdc, rt.left, rt.top);
      if (xbad) { /* show the borders of the image inside the window */
        MoveToEx(hdc, rt.left+imgf->getWd(), rt.top, NULL);
	LineTo(hdc, rt.left+imgf->getWd(), rt.bottom-1);
	LineTo(hdc, rt.right-1, rt.top);
      }
      if (ybad) { /* show the borders of the image inside the window */
        MoveToEx(hdc, rt.left, imgf->getHt()+rt.top, NULL);
	LineTo(hdc, rt.left+imgf->getWd(), rt.bottom-1);
	LineTo(hdc, rt.left, imgf->getHt()+rt.top);
      }
    }
    EndPaint(hwnd, &ps);
    return 0;
   case WM_COMMAND:
    /* int wmId=LOWORD(wParam), wmEvent=HIWORD(wParam); */
    switch (LOWORD(wParam)) {
     case IDM_SAVE:
      do_save(true); return 0;
     case IDM_SAVE_AS:
      {
        char *argv[]={"(progname)","pts.ppm","pts.ps",NULL};
	int argc=3;
	char result[30];
	int ret=main(argc, argv);
	// int ret=getpid();
	sprintf(result, "main run: %d", ret);
	MessageBox(hwnd, result, NULL, 0); /* unreached !! */
      }
      do_save_as(); return 0;
     case IDM_WEBSITE:
      /* Dat: it seems to be impossible to force the browser to open the URL
       * in a new window (instead of overwriting the current window). There
       * are OLE/DDE solutions for Netscape4 and Internet Explorer, but no
       * generic solution.
       */
      ShellExecute(hwnd, "open", "http://www.inf.bme.hu/~pts/sam2p/", 0, 0, SW_SHOWDEFAULT);
      return 0;
     case IDM_ABOUT:
      DialogBox(hInst, (LPCTSTR)IDD_ABOUTBOX, hwnd, (DLGPROC)About);
      return 0;
     case IDM_OPEN:
      do_open(); return 0;
     case IDM_NEW:
      do_new(); return 0;
     case IDM_PARAMETERS:
      DialogBox(hInst, (LPCTSTR)IDD_RUN, hwnd, (DLGPROC)dialogRun);
      return 0;
     case IDM_EXIT:
      do_exit(); return 0;
     default:
      MessageBox(hwnd,"Unimplemented.", Error::banner0, 0);
      return 0;
    }
   case WM_SIZING: /* possibly override resize requests */
    if (imgf!=NULL) { /* resize the window to perfectly fit the image */
      rp=(RECT*)lParam;
      // RECT wr; GetWindowRect(hwnd, &wr);
      // RECT cr; GetClientRect(hwnd, &cr);
      // rp->right=rp->left+imgf->getWd()+(wr.right-wr.left)-(cr.right-cr.left);
      // rp->bottom=rp->top+imgf->getHt()+(wr.bottom-wr.top)-(cr.bottom-cr.top);
      getFrameSize(hwnd, rp->right, rp->bottom);
      rp->right+=rp->left+imgf->getWd();
      rp->bottom+=rp->top+imgf->getHt();
      return TRUE;
    } else return FALSE;
   case WM_ERASEBKGND:
    if (imgf!=NULL) return FALSE;
    break;
   /* Dat: upon window resize, WM_SIZING, WM_SIZE, WM_ERASEBKGND and WM_PAINT is sent in this order */
   case WM_SIZE: /* we have been resized; let's invalidate the window contents */
    GetClientRect(hwnd, &rt);
    if (wd_last==0 && ht_last==0) updateBgText(hwnd, rt.right-rt.left, rt.bottom-rt.top);
                             else updateBgText(hwnd, wd_last, ht_last);
    InvalidateRect(hwnd, NULL, TRUE);
    return 0;
   case WM_CLOSE: /* user has clicked button X on the title bar; confirm exit first */
    do_exit(); return 0;
   case WM_DESTROY: /* exit already confirmed */
    PostQuitMessage(0);
    return 0;
  }
  return DefWindowProc(hwnd, message, wParam, lParam);
}

#if 0
char *s="1111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111"
        "1111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111"
        "1111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111"
        "1111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111";
#endif

// Mesage handler for about box.
LRESULT CALLBACK About(HWND hdlg, UINT message, WPARAM wParam, LPARAM /*lParam*/) {
  switch (message) {
   case WM_INITDIALOG:
    { HFONT hfont=CreateFont(10, 0, 0, 0, 700, 0, 0, 0, 0, 0, 0, 0, 0, /*"System"*/ "MS Sans Serif");
      SendMessage(GetDlgItem(hdlg, IDC_PRODUCT), WM_SETFONT, (WPARAM)hfont, MAKELPARAM(TRUE, 0));
      // !! DeleteObject
      SetDlgItemText(hdlg, IDC_PRODUCT, Error::banner0);
      SetDlgItemText(hdlg, IDC_INFO,
"This is the Win32 GUI of sam2p.\r\n"
"  sam2p is a UNIX command line utility written in ANSI C++ that converts many "
"raster (bitmap) image formats into Adobe PostScript or PDF files and several "
"other formats. The images "
"are not vectorized. sam2p gives full control to the user to specify "
"standards-compliance, compression, and bit depths. In some cases sam2p can "
"compress an image 100 times smaller than the PostScript output of many other "
"common image converters. sam2p provides ZIP, RLE and LZW (de)compression "
"filters even on Level1 devices.\r\n");
    }
    return TRUE;
   case WM_COMMAND:
    if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) {
      { CHOOSEFONT cf;
        ZeroMemory(&cf, sizeof(cf));
        cf.lStructSize=sizeof(cf);
	cf.rgbColors=0; /* black */
	cf.Flags=CF_EFFECTS|CF_FORCEFONTEXIST|CF_NOSIMULATIONS|CF_NOVERTFONTS|CF_SCREENFONTS;
	cf.hwndOwner=NULL;
	cf.hDC=NULL;
        ChooseFont(&cf);
      }
      EndDialog(hdlg, LOWORD(wParam));
      return TRUE;
    }
    break;
  }
  return FALSE;
}

LRESULT CALLBACK dialogRun(HWND hdlg, UINT message, WPARAM wParam, LPARAM /*lParam*/) {
  switch (message) {
   case WM_INITDIALOG:
    { HWND hwnd=GetDlgItem(hdlg, IDC_ELOG);
      HFONT hfont=CreateFont(10, 0, 0, 0, 400, 0, 0, 0, 0, 0, 0, 0, 0,"Fixedsys");
      SendMessage(hwnd, WM_SETFONT, (WPARAM)hfont, MAKELPARAM(TRUE, 0));
      // DeleteObject(hfont); /* Imp: where to do DeleteObject()? */
      /* PostMessage doesn't work, either */
      unsigned i;
      char buf[222];
      for (i=0;i<100;i++) {
        SendMessage(hwnd,EM_SETSEL,0U-1,0U-1); 
	sprintf(buf, "%d Hello, World! 0123456789 1111111111 222222222 333333333\r\n", i);
        SendMessage(hwnd,EM_REPLACESEL,FALSE,(LPARAM)(LPCTSTR)buf);
      }
      SendMessage(hwnd,EM_SCROLLCARET,0,0);
      InvalidateRect(hwnd,NULL,FALSE); /* no effect */
      // !! EM_REPLACESEL doesn't make the end of the edit box text
    }
    return TRUE;
 #if 0
   case WM_ACTIVATE:
    { // HWND hwnd=GetDlgItem(hdlg, IDC_ELOG);
      // MessageBox(NULL,"Hello","",0);
      //SendMessage(hwnd,EM_SETSEL,-1,2000); 
      //SendMessage(hwnd,EM_SCROLLCARET,0,0);
      //InvalidateRect(hwnd,NULL,FALSE); /* no effect */
    }
    return TRUE;
 #endif   
   case WM_COMMAND:
    if (LOWORD(wParam) == IDOK) {
      HWND hwnd=GetDlgItem(hdlg, IDC_ELOG);
      SendMessage(hwnd,EM_SETSEL,0xFFFFFFFF,-1); 
      SendMessage(hwnd,EM_REPLACESEL,FALSE,(LPARAM)(LPCTSTR)"Append.");
      break;
    }
    if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) {
      EndDialog(hdlg, LOWORD(wParam));
      return TRUE;
    }
    break;
  }
  return FALSE;
}
