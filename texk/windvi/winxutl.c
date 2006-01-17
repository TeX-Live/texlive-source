#include <windows.h>
#include <commdlg.h>
#include "wingui.h"
#include "xdvi-config.h"

#include "winxutl.h"

#define _TRACE 0

RGBQUAD QuadWhite = { 255, 255, 255, 0};
RGBQUAD QuadBlack = {   0,   0,   0, 0};

colormap_t win32_color_map[] = 
{
  {"snow"                      , MYRGB (255,250,250)},
  {"ghost white"               , MYRGB (248,248,255)},
  {"GhostWhite"                , MYRGB (248,248,255)},
  {"white smoke"               , MYRGB (245,245,245)},
  {"WhiteSmoke"                , MYRGB (245,245,245)},
  {"gainsboro"                 , MYRGB (220,220,220)},
  {"floral white"              , MYRGB (255,250,240)},
  {"FloralWhite"               , MYRGB (255,250,240)},
  {"old lace"                  , MYRGB (253,245,230)},
  {"OldLace"                   , MYRGB (253,245,230)},
  {"linen"                     , MYRGB (250,240,230)},
  {"antique white"             , MYRGB (250,235,215)},
  {"AntiqueWhite"              , MYRGB (250,235,215)},
  {"papaya whip"               , MYRGB (255,239,213)},
  {"PapayaWhip"                , MYRGB (255,239,213)},
  {"blanched almond"           , MYRGB (255,235,205)},
  {"BlanchedAlmond"            , MYRGB (255,235,205)},
  {"bisque"                    , MYRGB (255,228,196)},
  {"peach puff"                , MYRGB (255,218,185)},
  {"PeachPuff"                 , MYRGB (255,218,185)},
  {"navajo white"              , MYRGB (255,222,173)},
  {"NavajoWhite"               , MYRGB (255,222,173)},
  {"moccasin"                  , MYRGB (255,228,181)},
  {"cornsilk"                  , MYRGB (255,248,220)},
  {"ivory"                     , MYRGB (255,255,240)},
  {"lemon chiffon"             , MYRGB (255,250,205)},
  {"LemonChiffon"              , MYRGB (255,250,205)},
  {"seashell"                  , MYRGB (255,245,238)},
  {"honeydew"                  , MYRGB (240,255,240)},
  {"mint cream"                , MYRGB (245,255,250)},
  {"MintCream"                 , MYRGB (245,255,250)},
  {"azure"                     , MYRGB (240,255,255)},
  {"alice blue"                , MYRGB (240,248,255)},
  {"AliceBlue"                 , MYRGB (240,248,255)},
  {"lavender"                  , MYRGB (230,230,250)},
  {"lavender blush"            , MYRGB (255,240,245)},
  {"LavenderBlush"             , MYRGB (255,240,245)},
  {"misty rose"                , MYRGB (255,228,225)},
  {"MistyRose"                 , MYRGB (255,228,225)},
  {"white"                     , MYRGB (255,255,255)},
  {"black"                     , MYRGB (  0,  0,  0)},
  {"dark slate gray"           , MYRGB ( 47, 79, 79)},
  {"DarkSlateGray"             , MYRGB ( 47, 79, 79)},
  {"dark slate grey"           , MYRGB ( 47, 79, 79)},
  {"DarkSlateGrey"             , MYRGB ( 47, 79, 79)},
  {"dim gray"                  , MYRGB (105,105,105)},
  {"DimGray"                   , MYRGB (105,105,105)},
  {"dim grey"                  , MYRGB (105,105,105)},
  {"DimGrey"                   , MYRGB (105,105,105)},
  {"slate gray"                , MYRGB (112,128,144)},
  {"SlateGray"                 , MYRGB (112,128,144)},
  {"slate grey"                , MYRGB (112,128,144)},
  {"SlateGrey"                 , MYRGB (112,128,144)},
  {"light slate gray"          , MYRGB (119,136,153)},
  {"LightSlateGray"            , MYRGB (119,136,153)},
  {"light slate grey"          , MYRGB (119,136,153)},
  {"LightSlateGrey"            , MYRGB (119,136,153)},
  {"gray"                      , MYRGB (190,190,190)},
  {"grey"                      , MYRGB (190,190,190)},
  {"light grey"                , MYRGB (211,211,211)},
  {"LightGrey"                 , MYRGB (211,211,211)},
  {"light gray"                , MYRGB (211,211,211)},
  {"LightGray"                 , MYRGB (211,211,211)},
  {"midnight blue"             , MYRGB ( 25, 25,112)},
  {"MidnightBlue"              , MYRGB ( 25, 25,112)},
  {"navy"                      , MYRGB (  0,  0,128)},
  {"navy blue"                 , MYRGB (  0,  0,128)},
  {"NavyBlue"                  , MYRGB (  0,  0,128)},
  {"cornflower blue"           , MYRGB (100,149,237)},
  {"CornflowerBlue"            , MYRGB (100,149,237)},
  {"dark slate blue"           , MYRGB ( 72, 61,139)},
  {"DarkSlateBlue"             , MYRGB ( 72, 61,139)},
  {"slate blue"                , MYRGB (106, 90,205)},
  {"SlateBlue"                 , MYRGB (106, 90,205)},
  {"medium slate blue"         , MYRGB (123,104,238)},
  {"MediumSlateBlue"           , MYRGB (123,104,238)},
  {"light slate blue"          , MYRGB (132,112,255)},
  {"LightSlateBlue"            , MYRGB (132,112,255)},
  {"medium blue"               , MYRGB (  0,  0,205)},
  {"MediumBlue"                , MYRGB (  0,  0,205)},
  {"royal blue"                , MYRGB ( 65,105,225)},
  {"RoyalBlue"                 , MYRGB ( 65,105,225)},
  {"blue"                      , MYRGB (  0,  0,255)},
  {"dodger blue"               , MYRGB ( 30,144,255)},
  {"DodgerBlue"                , MYRGB ( 30,144,255)},
  {"deep sky blue"             , MYRGB (  0,191,255)},
  {"DeepSkyBlue"               , MYRGB (  0,191,255)},
  {"sky blue"                  , MYRGB (135,206,235)},
  {"SkyBlue"                   , MYRGB (135,206,235)},
  {"light sky blue"            , MYRGB (135,206,250)},
  {"LightSkyBlue"              , MYRGB (135,206,250)},
  {"steel blue"                , MYRGB ( 70,130,180)},
  {"SteelBlue"                 , MYRGB ( 70,130,180)},
  {"light steel blue"          , MYRGB (176,196,222)},
  {"LightSteelBlue"            , MYRGB (176,196,222)},
  {"light blue"                , MYRGB (173,216,230)},
  {"LightBlue"                 , MYRGB (173,216,230)},
  {"powder blue"               , MYRGB (176,224,230)},
  {"PowderBlue"                , MYRGB (176,224,230)},
  {"pale turquoise"            , MYRGB (175,238,238)},
  {"PaleTurquoise"             , MYRGB (175,238,238)},
  {"dark turquoise"            , MYRGB (  0,206,209)},
  {"DarkTurquoise"             , MYRGB (  0,206,209)},
  {"medium turquoise"          , MYRGB ( 72,209,204)},
  {"MediumTurquoise"           , MYRGB ( 72,209,204)},
  {"turquoise"                 , MYRGB ( 64,224,208)},
  {"cyan"                      , MYRGB (  0,255,255)},
  {"light cyan"                , MYRGB (224,255,255)},
  {"LightCyan"                 , MYRGB (224,255,255)},
  {"cadet blue"                , MYRGB ( 95,158,160)},
  {"CadetBlue"                 , MYRGB ( 95,158,160)},
  {"medium aquamarine"         , MYRGB (102,205,170)},
  {"MediumAquamarine"          , MYRGB (102,205,170)},
  {"aquamarine"                , MYRGB (127,255,212)},
  {"dark green"                , MYRGB (  0,100,  0)},
  {"DarkGreen"                 , MYRGB (  0,100,  0)},
  {"dark olive green"          , MYRGB ( 85,107, 47)},
  {"DarkOliveGreen"            , MYRGB ( 85,107, 47)},
  {"dark sea green"            , MYRGB (143,188,143)},
  {"DarkSeaGreen"              , MYRGB (143,188,143)},
  {"sea green"                 , MYRGB ( 46,139, 87)},
  {"SeaGreen"                  , MYRGB ( 46,139, 87)},
  {"medium sea green"          , MYRGB ( 60,179,113)},
  {"MediumSeaGreen"            , MYRGB ( 60,179,113)},
  {"light sea green"           , MYRGB ( 32,178,170)},
  {"LightSeaGreen"             , MYRGB ( 32,178,170)},
  {"pale green"                , MYRGB (152,251,152)},
  {"PaleGreen"                 , MYRGB (152,251,152)},
  {"spring green"              , MYRGB (  0,255,127)},
  {"SpringGreen"               , MYRGB (  0,255,127)},
  {"lawn green"                , MYRGB (124,252,  0)},
  {"LawnGreen"                 , MYRGB (124,252,  0)},
  {"green"                     , MYRGB (  0,255,  0)},
  {"chartreuse"                , MYRGB (127,255,  0)},
  {"medium spring green"       , MYRGB (  0,250,154)},
  {"MediumSpringGreen"         , MYRGB (  0,250,154)},
  {"green yellow"              , MYRGB (173,255, 47)},
  {"GreenYellow"               , MYRGB (173,255, 47)},
  {"lime green"                , MYRGB ( 50,205, 50)},
  {"LimeGreen"                 , MYRGB ( 50,205, 50)},
  {"yellow green"              , MYRGB (154,205, 50)},
  {"YellowGreen"               , MYRGB (154,205, 50)},
  {"forest green"              , MYRGB ( 34,139, 34)},
  {"ForestGreen"               , MYRGB ( 34,139, 34)},
  {"olive drab"                , MYRGB (107,142, 35)},
  {"OliveDrab"                 , MYRGB (107,142, 35)},
  {"dark khaki"                , MYRGB (189,183,107)},
  {"DarkKhaki"                 , MYRGB (189,183,107)},
  {"khaki"                     , MYRGB (240,230,140)},
  {"pale goldenrod"            , MYRGB (238,232,170)},
  {"PaleGoldenrod"             , MYRGB (238,232,170)},
  {"light goldenrod yellow"    , MYRGB (250,250,210)},
  {"LightGoldenrodYellow"      , MYRGB (250,250,210)},
  {"light yellow"              , MYRGB (255,255,224)},
  {"LightYellow"               , MYRGB (255,255,224)},
  {"yellow"                    , MYRGB (255,255,  0)},
  {"gold"                      , MYRGB (255,215,  0)},
  {"light goldenrod"           , MYRGB (238,221,130)},
  {"LightGoldenrod"            , MYRGB (238,221,130)},
  {"goldenrod"                 , MYRGB (218,165, 32)},
  {"dark goldenrod"            , MYRGB (184,134, 11)},
  {"DarkGoldenrod"             , MYRGB (184,134, 11)},
  {"rosy brown"                , MYRGB (188,143,143)},
  {"RosyBrown"                 , MYRGB (188,143,143)},
  {"indian red"                , MYRGB (205, 92, 92)},
  {"IndianRed"                 , MYRGB (205, 92, 92)},
  {"saddle brown"              , MYRGB (139, 69, 19)},
  {"SaddleBrown"               , MYRGB (139, 69, 19)},
  {"sienna"                    , MYRGB (160, 82, 45)},
  {"peru"                      , MYRGB (205,133, 63)},
  {"burlywood"                 , MYRGB (222,184,135)},
  {"beige"                     , MYRGB (245,245,220)},
  {"wheat"                     , MYRGB (245,222,179)},
  {"sandy brown"               , MYRGB (244,164, 96)},
  {"SandyBrown"                , MYRGB (244,164, 96)},
  {"tan"                       , MYRGB (210,180,140)},
  {"chocolate"                 , MYRGB (210,105, 30)},
  {"firebrick"                 , MYRGB (178,34, 34)},
  {"brown"                     , MYRGB (165,42, 42)},
  {"dark salmon"               , MYRGB (233,150,122)},
  {"DarkSalmon"                , MYRGB (233,150,122)},
  {"salmon"                    , MYRGB (250,128,114)},
  {"light salmon"              , MYRGB (255,160,122)},
  {"LightSalmon"               , MYRGB (255,160,122)},
  {"orange"                    , MYRGB (255,165,  0)},
  {"dark orange"               , MYRGB (255,140,  0)},
  {"DarkOrange"                , MYRGB (255,140,  0)},
  {"coral"                     , MYRGB (255,127, 80)},
  {"light coral"               , MYRGB (240,128,128)},
  {"LightCoral"                , MYRGB (240,128,128)},
  {"tomato"                    , MYRGB (255, 99, 71)},
  {"orange red"                , MYRGB (255, 69,  0)},
  {"OrangeRed"                 , MYRGB (255, 69,  0)},
  {"red"                       , MYRGB (255,  0,  0)},
  {"hot pink"                  , MYRGB (255,105,180)},
  {"HotPink"                   , MYRGB (255,105,180)},
  {"deep pink"                 , MYRGB (255, 20,147)},
  {"DeepPink"                  , MYRGB (255, 20,147)},
  {"pink"                      , MYRGB (255,192,203)},
  {"light pink"                , MYRGB (255,182,193)},
  {"LightPink"                 , MYRGB (255,182,193)},
  {"pale violet red"           , MYRGB (219,112,147)},
  {"PaleVioletRed"             , MYRGB (219,112,147)},
  {"maroon"                    , MYRGB (176, 48, 96)},
  {"medium violet red"         , MYRGB (199, 21,133)},
  {"MediumVioletRed"           , MYRGB (199, 21,133)},
  {"violet red"                , MYRGB (208, 32,144)},
  {"VioletRed"                 , MYRGB (208, 32,144)},
  {"magenta"                   , MYRGB (255,  0,255)},
  {"violet"                    , MYRGB (238,130,238)},
  {"plum"                      , MYRGB (221,160,221)},
  {"orchid"                    , MYRGB (218,112,214)},
  {"medium orchid"             , MYRGB (186, 85,211)},
  {"MediumOrchid"              , MYRGB (186, 85,211)},
  {"dark orchid"               , MYRGB (153, 50,204)},
  {"DarkOrchid"                , MYRGB (153, 50,204)},
  {"dark violet"               , MYRGB (148,  0,211)},
  {"DarkViolet"                , MYRGB (148,  0,211)},
  {"blue violet"               , MYRGB (138, 43,226)},
  {"BlueViolet"                , MYRGB (138, 43,226)},
  {"purple"                    , MYRGB (160, 32,240)},
  {"medium purple"             , MYRGB (147,112,219)},
  {"MediumPurple"              , MYRGB (147,112,219)},
  {"thistle"                   , MYRGB (216,191,216)},
  {"gray0"                     , MYRGB (  0,  0,  0)},
  {"grey0"                     , MYRGB (  0,  0,  0)},
  {"dark grey"                 , MYRGB (169,169,169)},
  {"DarkGrey"                  , MYRGB (169,169,169)},
  {"dark gray"                 , MYRGB (169,169,169)},
  {"DarkGray"                  , MYRGB (169,169,169)},
  {"dark blue"                 , MYRGB (  0,  0,139)},
  {"DarkBlue"                  , MYRGB (  0,  0,139)},
  {"dark cyan"                 , MYRGB (  0,139,139)},
  {"DarkCyan"                  , MYRGB (  0,139,139)},
  {"dark magenta"              , MYRGB (139,  0,139)},
  {"DarkMagenta"               , MYRGB (139,  0,139)},
  {"dark red"                  , MYRGB (139,  0,  0)},
  {"DarkRed"                   , MYRGB (139,  0,  0)},
  {"light green"               , MYRGB (144,238,144)},
  {"LightGreen"                , MYRGB (144,238,144)}
};

int win32_color_map_size = sizeof(win32_color_map)/sizeof(colormap_t);

int check_386()
{
  OSVERSIONINFO os;
  SYSTEM_INFO si;
  int is_nt = 0;
  os.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
  if (GetVersionEx(&os) == 0)
    Win32Error("GetVersionEx");
  is_nt = (os.dwPlatformId == VER_PLATFORM_WIN32_NT);
  GetSystemInfo(&si);
  if (is_nt)
    return si.wProcessorLevel <= 3;
  else
    return si.dwProcessorType == PROCESSOR_INTEL_386;
}

/*
  swap count bytes starting at address p
  */

void bswap_asm(void *p, unsigned count)
{
    __asm {
      mov	edx, DWORD PTR p;
      mov	ecx, count;
      /*	shl     ecx, 2; */
    $L1:
      mov     eax, DWORD PTR [edx];
      bswap   eax;
      mov     DWORD PTR [edx], eax;
      inc     edx;
      inc     edx;
      inc     edx;
      inc     edx;
      loopnz  $L1;
    }
}

void bswap_c(void *p, unsigned count)
{
  unsigned char *q;
  unsigned char temp;
    for (q = p;
	 q < (unsigned char *)p + 4*count;
	 q += 4) {
      	temp = *(q);   *(q)   = *(q+3); *(q+3) = temp;
	temp = *(q+1); *(q+1) = *(q+2); *(q+2) = temp;
    }
}

FILE *
XsraSelFile(Widget toplevel, char *prompt, char *ok, char *cancel, 
	    char *failed, char *init_path, char *mode,
	    int (*show_entry)(), char **name_return)
{
  OPENFILENAME ofn;       /* common dialog box structure */
  char szFile[260];       /* buffer for filename */
  FILE *fret = NULL;
  char currentDir[260];

  *name_return = NULL;
  GetCurrentDirectory(sizeof(currentDir), currentDir);

  szFile[0] = '\0';
  /* Initialize OPENFILENAME */
  ZeroMemory(&ofn, sizeof(OPENFILENAME));
  ofn.lStructSize = sizeof(OPENFILENAME);
  ofn.hwndOwner = toplevel;
  ofn.lpstrFile = szFile;
  ofn.nMaxFile = sizeof(szFile);
  ofn.lpstrFilter = "Dvi Files\0*.dvi\0All Files\0*.*\0\0";
  ofn.nFilterIndex = 1;
  ofn.lpstrFileTitle = NULL;
  ofn.nMaxFileTitle = 0;
  ofn.lpstrInitialDir = NULL; /* FIXME : we should use init_path */
  ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

  /* Display the Open dialog box.  */
  
  if (GetOpenFileName(&ofn)==TRUE) {
    *name_return = xstrdup(szFile);
    fret = fopen(*name_return, mode);
  }
  else {
    SetCurrentDirectory(currentDir);
    if (CommDlgExtendedError() != 0)
      MessageBox(toplevel, failed, NULL, MB_OK|MB_ICONERROR||MB_APPLMODAL);
  }
  return fret;
}

COLORREF string_to_colorref(char *p)
{
  COLORREF c;
  float cyan, magenta, yellow, black, red, green, blue, gray;

  while ( *p <= ' ' ) p++ ;
  if (memicmp(p, "cmyk", 4) == 0) {
    sscanf(p+4, "%f %f %f %f", &cyan, &magenta, &yellow, &black);
    cyan += black;
    magenta += black;
    yellow += black;

    red = (float)fabs(magenta-yellow);
    blue = (float)fabs(magenta - cyan);
    green = (float)fabs(yellow - cyan);

    c = MYRGB((unsigned)(red*255), (unsigned)(green*255), (unsigned)(blue*255));
  }
  else if (memicmp(p, "gray", 4) == 0) {
    sscanf(p+4, "%f", &gray);
    c = MYRGB((unsigned)(255*gray), (unsigned)(255*gray), (unsigned)(255*gray));
  }
  else if (memicmp(p, "rgb", 3) == 0) {
    sscanf(p+3, "%f %f %f", &red, &green, &blue);
    c = MYRGB((unsigned)(red*255), (unsigned)(green*255), (unsigned)(blue*255));
  }
  else
    c = x_to_win32_color(p);

  return c;
}

#ifdef TRANSFORM
HRGN hClipRgn = NULL;
#endif

/*
  FP: 22/07/99
  Time to clean up this routine.
  Should handle clipping automatically.
  Should handle transformations.
  Grey scaling for colors is handled by complex rop code.
*/

/* trial version : uses SetDIBits(), no temp DIB */
void XPutImage(void *scrn, HWND w, HDC hdc, XImage *img, 
	       int src_x, int src_y,
	       int dest_x, int dest_y,
	       unsigned int width, unsigned int height)
{
  RGBQUAD c1,c2;
  COLORREF fc, bc;
#ifdef TRANSFORM
  POINT lppt[4];
  HRGN hNewClipRgn = NULL;
  XFORM xfrm, xfrm_temp;
  extern BOOL bTransfInUse;
#endif

  if (!allowDrawingChars) {
#if 0
    fprintf(stderr, "Not allowing chars drawing\n");
#endif
    return;
  }

#if 0
    fprintf(stderr, "Back color is %-8x, mode is %s\n",
	    GetBkColor(hdc),
	    (GetBkMode(hdc) == OPAQUE ? "opaque" :
	     (GetBkMode(hdc) == TRANSPARENT ? "transparent" : "failed")));
#endif

#if _TRACE
  if (isPrinting) {
    fprintf(stderr, "fore = %x\n", fore_Pixel);
    fprintf(stderr, "src_x = %d, src_y = %d, dest_x = %d, dest_y = %d\n",
	    src_x, src_y, dest_x, dest_y);
  }
#endif

  if (!img->width || !img->height) {
    /* Nothing to do ! */
    return;
  }

#ifdef WIN32
  if (img->endian_permuted !=NULL && *(img->endian_permuted) == 0) {
    bswap((BMUNIT*)img->data, 
		(img->bytes_per_line * height)/BMBYTES);
    *(img->endian_permuted) = 1;
  }
#endif

#if TRANSFORM
  if (resource.use_xform && bTransfInUse) {
    RECT rTemp;
    BOOL bClipped1, bClipped2;
    extern XFORM current_transf;
    rTemp.left = dest_x;
    rTemp.top = dest_y;
    rTemp.right = dest_x + width;
    rTemp.bottom = dest_y + height;
    if (SetWorldTransform(hdc, &current_transf) == 0)
      Win32Error("XPutImage/SetWorldTransform/current");
    
    lppt[0].x = dest_x;
    lppt[0].y = dest_y;
    lppt[1].x = dest_x + width;
    lppt[1].y = dest_y;
    lppt[2].x = dest_x + width;
    lppt[2].y = dest_y + height;
    lppt[3].x = dest_x;
    lppt[3].y = dest_y + height;
    LPtoDP(hdc, lppt, 4);
    if ((hNewClipRgn = CreatePolygonRgn(lppt, 4, ALTERNATE)) == NULL) {
      Win32Error("XPutImage/CreatePolygonRgn");
    }
    if (SelectClipRgn(hdc, hNewClipRgn) == ERROR) {
      Win32Error("XPutImage/SelectClipRgn");
    }
    bClipped2 = RectVisible(hdc, &rTemp);
    if (!bClipped2)
      goto nothing;
  }
  else
#endif
    {
      RECT rTemp;
      rTemp.left = dest_x;
      rTemp.top = dest_y;
      rTemp.right = dest_x + width;
      rTemp.bottom = dest_y + height;
      if (!RectVisible(hdc, &rTemp))
	goto nothing;
    }

#if _TRACE
    fprintf(stderr, "XPutImage at (%d %d) of (%d, %d, w=%d x h=%d)\n",
	    dest_x, dest_y, src_x, src_y, width, height);
#endif
  switch (img->depth) {
  case 1:
    bmi1.bmiHeader.biWidth    = img->width;
    bmi1.bmiHeader.biHeight   = (img->height == 1 ? img->height : -img->height);
    bmi1.bmiHeader.biSizeImage = 
      WIDTHBYTES((DWORD)img->width * img->depth) 
      * (img->height > 0 ? img->height : -img->height);
#if _TRACE
    fprintf(stderr, "XPutImage at (%d %d) of (%d, %d, w=%d x h=%d)\n",
	    dest_x, dest_y, src_x, src_y, width, height);
    fprintf(stderr, "bmiColors: 0 = %x, 1 = %x\n",
	    bmi1.bmiColors[0], bmi1.bmiColors[1]);
#endif

    c1 = bmi1.bmiColors[0];
    c2 = bmi1.bmiColors[1];

    if (isPrinting && !(bColorPage || psToDisplay)) {
      HBRUSH oldBrush;

      bmi1.bmiColors[0] = QuadWhite;
      bmi1.bmiColors[1] = QuadBlack;

      oldBrush = SelectObject(hdc, foreBrush);

      if (StretchDIBits(hdc, dest_x, dest_y, width, height,
			src_x, src_y, width, height, 
			img->data, (LPBITMAPINFO)&bmi1, 
			DIB_RGB_COLORS, SRCAND /* 0x00B8074A  0x00E61CE8 */  ) == GDI_ERROR)
	Win32Error("XPutImage/SetDIBits");
      bmi1.bmiColors[0] = c1;
      bmi1.bmiColors[1] = c2;

      SelectObject(hdc, oldBrush);
    }
#if 1
	else  {
	    HBRUSH oldBrush;
	    init_pix( MYRGB(255,255,255), MYRGB(0,0,0));
	    oldBrush = SelectObject(hdc, foreBrush);
	    if (StretchDIBits(hdc, dest_x, dest_y, width, height,
			      src_x, src_y, width, height, 
			      img->data, (LPBITMAPINFO)&bmi1, DIB_RGB_COLORS, 0x00B8074A ) == GDI_ERROR)
	      Win32Error("XPutImage/SetDIBits");
	    SelectObject(hdc, oldBrush);
	  }
#else
    else {
      /* this is the 2 passes solution, but does not seem to work ok
	 on the printer dc */
      bmi1.bmiColors[0] = QuadWhite;
      bmi1.bmiColors[1] = QuadBlack;
      
      if (StretchDIBits(hdc, dest_x, dest_y, width, height,
			src_x, src_y, width, height, 
			img->data, (LPBITMAPINFO)&bmi1, DIB_RGB_COLORS, SRCAND ) == GDI_ERROR)
	Win32Error("XPutImage/SetDIBits");
      
      bmi1.bmiColors[0] = QuadBlack;
      bmi1.bmiColors[1] = Color2Quad(fore_Pixel);
      
      if (StretchDIBits(hdc, dest_x, dest_y, width, height,
			src_x, src_y, width, height, 
			img->data, (LPBITMAPINFO)&bmi1, DIB_RGB_COLORS, SRCPAINT ) == GDI_ERROR)
	Win32Error("XPutImage/SetDIBits");
      bmi1.bmiColors[0] = c1;
      bmi1.bmiColors[1] = Color2Quad(fore_Pixel);
    }
#endif
    break;

  case 4: {
    POINT pt;
    RECT rcClip, rcGlyph, rcDest;

    bmi4.bmiHeader.biWidth    = img->width;
    bmi4.bmiHeader.biHeight   = (img->height == 1 ? img->height : -img->height);
    bmi4.bmiHeader.biSizeImage = 
      WIDTHBYTES((DWORD)img->width * img->depth) 
      * (img->height > 0 ? img->height : -img->height);

#if 0
    fprintf(stderr, "XPutImage @ (%d %d) w %d h %d fore %-8x back %-8x\n",
	    dest_x, dest_y, width, height, fore_Pixel, back_Pixel);
#endif

#if 0
	fprintf(stderr, "This is%s a color page!\n", bColorPage ? "" : " not");
#endif
    /* If this is a monochrome page, no antialiasing */
    if (!bColorPage) {
	/* Don't bother with color antialiasing in this case ! */
	fc = fore_Pixel;
	bc = back_Pixel;
	init_pix(bc, fc);
#if 0
	{
	  int pixel;
	  for (pixel = 0; pixel < 16; pixel++)
	    fprintf(stderr, "color %d = %-8x %-8x %-8x\n", pixel, 
		    pal_rgb[pixel], pal_bw[pixel], pal_mask[pixel]);
	}
#endif
	if (StretchDIBits(hdc, dest_x, dest_y, width, height,
			  src_x, src_y, width, height, 
			  img->data, (LPBITMAPINFO)&bmi4, DIB_RGB_COLORS, SRCAND ) == GDI_ERROR) {
	  Win32Error("XPutImage/SetDIBits");
	}
      }      
      else {
	  {
	    HBRUSH oldBrush;
	    init_pix( MYRGB(255,255,255), MYRGB(0,0,0));
	    oldBrush = SelectObject(hdc, foreBrush);
	    if (StretchDIBits(hdc, dest_x, dest_y, width, height,
			      src_x, src_y, width, height, 
			      img->data, (LPBITMAPINFO)&bmi4, DIB_RGB_COLORS, 0x00B8074A ) == GDI_ERROR)
	      Win32Error("XPutImage/SetDIBits");
	    SelectObject(hdc, oldBrush);
	  }
	  
	}
      }

    break;
    
  case 8:
    bmi8.bmiHeader.biWidth    = img->width;
    bmi8.bmiHeader.biHeight   = (img->height == 1 ? img->height : -img->height);
    bmi8.bmiHeader.biSizeImage = 
      WIDTHBYTES((DWORD)img->width * img->depth) 
      * (img->height > 0 ? img->height : -img->height);

    if (StretchDIBits(hdc, dest_x, dest_y, width, height,
		      src_x, src_y, width, height, 
		      img->data, (LPBITMAPINFO)&bmi8, DIB_RGB_COLORS, SRCPAINT ) == GDI_ERROR)
      Win32Error("XPutImage/SetDIBits");
    break;

  default:
    fprintf(stderr, "XPutImage: image depth is %d, non implemented.\n",
	    img->depth);
  }
 nothing:
#if TRANSFORM
  if (resource.use_xform && bTransfInUse) {
    SelectClipRgn(hdc, NULL);
    if (hNewClipRgn) DeleteObject(hNewClipRgn);
    ModifyWorldTransform(hdc, &xfrm_temp, MWT_IDENTITY);
  }
#endif
}

/* Trial version : assumes 16 colors DIBS, computes the nibble to be
   changed */
void XPutPixel(XImage *img, int x, int y, COLORREF c)
{
  unsigned pixel;
  unsigned char *pos;
  if (x < 0 || x > img->width || y < 0 || y > img->height) {
    fprintf(stderr, 
	    "XPutPixel error : x = %d y = %d width = %d height = %d\n",
	    x, y, img->width, img->height);
  } else {
    switch (img->depth) {
    case 4:
#if 0
      for (pixel = 0; pixel < 16; pixel++)
	fprintf(stderr, "color %d = %-8x\n", pixel, palette[pixel]);
      fprintf(stderr, "looked for %-8x\n", c);
#endif
      for (pixel = 0; (palette[pixel] != c) && (pixel < 16); pixel++);
      if (pixel == 16)
	Win32Error("XPutPixel/Color not found");
#if 0      
      else
	fprintf(stderr, "Found pixel = %d color = %-8x\n", pixel, palette[pixel]);
#endif
      pos = (unsigned char *)img->data + y * img->bytes_per_line + x / 2;
#if 0
      fprintf(stderr, "img w = %d h = %d b/l = %d data = %x\nx = %d y = %d pos = %x\n", img->width , img->height, img->bytes_per_line, img->data, x, y, pos);
#endif
      if (x % 2 == 0) {
	/* msb */
	*pos = (*pos & 0x0F) | ((pixel & 0x0F) << 4);
      }
      else {
	/* lsb */
	*pos = (*pos & 0xF0) | (pixel & 0x0F);
      }
      break;
    default:
      Win32Error("XPutPixel/not a 16 colors DIB");
    }
  }
}

XImage* XCreateImage(void* d, HDC hdc, unsigned int depth, 
		     int format, int offset, char* data, 
		     unsigned int width, unsigned int height, 
		     int bitmap_pad, int bytes_per_line)
{
  XImage *img;
  img = (XImage *)xmalloc(sizeof(XImage));
  img->width = width;
  img->height = height;
  img->depth = depth;
  if (! bytes_per_line) {
    img->bytes_per_line = 
      WIDTHBYTES((DWORD)img->width * img->depth) ;
  }
  else
    img->bytes_per_line = bytes_per_line;
  img->data = NULL;
  return img;
}

void XClearWindow(void *disp, HWND win)
{
  if (! isPrinting) {
    if (debug & DBG_EVENT) {
      fprintf(stderr, "Clearing window %-8x\n", win);
    }
    SendMessage(win, WM_ERASEBKGND,  (WPARAM)GetDC(win), 0);
    UpdateWindow(win);
  }
}

void XClearArea(void *disp, HWND win, int x, int y, int width, int height, 
		int b)
{
  HDC maneGC = GetDC(win);
  RECT r;

  r.left = x; r.top = y;
  r.right = x + width;
  r.bottom = y + height;

  /* Blank the bitmap */
#if 0  
  if (!PatBlt(maneGC, x, y, x+width, y+height, PATCOPY))
    Win32Error("XClearArea: PatBlt");
#else
  r.right++;
  r.bottom++;
  if (!FillRect(maneGC, &r, backBrush))
    Win32Error("XClearArea: PatBlt");
#endif
  ReleaseDC(win, maneGC);
}

void XFlush(void *disp)
{
  GdiFlush();
}

void XDestroyImage(XImage *img)
{
  /* An XImage is a DIB basically */
  if (img) {    
    if (img->data) free(img->data);
    free (img);
  }
}

void XDrawLine(void *d, HWND hwnd, HDC hdc, int x1, int y1, int x2, int y2)
{
  static int xprev = 0, yprev = 0;
  if (xprev != x1 || yprev != y1) {
#if 0
    fprintf(stderr, "Moving to (%d, %d)\n", x1, y1);
#endif
    MoveToEx(hdc, x1, y1, NULL);
  }
#if 0
  fprintf(stderr, "Drawing to (%d, %d)\n", x2, y2);
#endif
  LineTo(hdc, x2, y2);
  xprev = x2;
  yprev = y2;
}

void XDrawPoint(void *w, HWND hwnd, HDC hdc, int x1, int y1)
{
#if 0
  fprintf(stderr, "Drawing point at (%d %d)\n", x1, y1);
#endif
  if (MoveToEx(hdc, x1, y1, NULL) == 0) {
    Win32Error("XDrawPoint/MoveToEx failed!");
  }
  if (LineTo(hdc, x1, y1) == 0) {
    Win32Error("XDrawPoint/LineTo failed!");
  }
}

void XFillRectangle(void *d, HWND w, HDC hdc, int x, int y,
		    unsigned int width, unsigned int height)
{
  RECT r;
  /* Rectangle(hdc, x, y, x+width, y+height); */
  r.left = x; r.top = y;
  r.right = x + width;
  r.bottom = y + height;
#if 0
  fprintf(stderr, "FillRect (%d %d) - (%d %d) hdc %x color %-8x\n",
	  r.left, r.top, r.right, r.bottom, hdc, fore_Pixel);
#endif
  FillRect(hdc, &r, foreBrush);
}


HBITMAP CreateDIB(HDC hdc, int x, int y, int bitCount, 
		  BITMAPINFO_256 *header, void **data)
{
  int i;
  HBITMAP hbm;
  BITMAPINFO_256 bmi;
  LPBITMAPINFO lpbmi;

  LPVOID ppvBits;

  lpbmi = (LPBITMAPINFO)(header ? header : &bmi);

  lpbmi->bmiHeader.biSize     = sizeof(BITMAPINFOHEADER);
  lpbmi->bmiHeader.biPlanes   = 1;
  lpbmi->bmiHeader.biBitCount = bitCount;
  lpbmi->bmiHeader.biCompression        = BI_RGB;
  lpbmi->bmiHeader.biWidth    = x;
  lpbmi->bmiHeader.biHeight   = y;
  lpbmi->bmiHeader.biSizeImage = 
    WIDTHBYTES((DWORD)x * bitCount) * (y > 0 ? y : -y);
  if (lpbmi->bmiHeader.biCompression != BI_RGB)
    lpbmi->bmiHeader.biSizeImage = (lpbmi->bmiHeader.biSizeImage * 3) / 2;
  lpbmi->bmiHeader.biXPelsPerMeter      = 0;
  lpbmi->bmiHeader.biYPelsPerMeter      = 0;
  lpbmi->bmiHeader.biClrUsed            = 0;
  lpbmi->bmiHeader.biClrImportant       = 0;
  
  /* SelectPalette(hdc,hpal,FALSE); */
  if (bitCount == 1) {
    lpbmi->bmiColors[1] = Color2Quad(back_Pixel);
    lpbmi->bmiColors[0] = Color2Quad(fore_Pixel);
  }
#ifdef GREY
  else if (bitCount <= 8) {
    for (i = 0; i < (1 << bitCount); i++) {
      lpbmi->bmiColors[i] = Color2Quad(palette[i]);
    }
  }
#endif

  hbm = CreateDIBSection(hdc, lpbmi, DIB_RGB_COLORS, &ppvBits, NULL, 0);
  /*  fprintf(stderr, "Nb DIBS = %d\n", ++nbDIBS); */
  if (hbm == NULL)
    Win32Error("CreateDIB/CreateDIBSection");
  if (data)
    *data = ppvBits;
  return hbm;
}

/* These ones are taken from NTEmacs */

/*
 *    XParseGeometry parses strings of the form
 *   "=<width>x<height>{+-}<xoffset>{+-}<yoffset>", where
 *   width, height, xoffset, and yoffset are unsigned integers.
 *   Example:  "=80x24+300-49"
 *   The equal sign is optional.
 *   It returns a bitmask that indicates which of the four values
 *   were actually found in the string.  For each value found,
 *   the corresponding argument is updated;  for each value
 *   not found, the corresponding argument is left unchanged. 
 */

static int
read_integer (char *s, char **NextString)
{
  register int Result = 0;
  int Sign = 1;
  
  if (*s == '+')
    s++;
  else if (*s == '-')
    {
      s++;
      Sign = -1;
    }
  for (; (*s >= '0') && (*s <= '9'); s++)
    {
      Result = (Result * 10) + (*s - '0');
    }
  *NextString = s;
  if (Sign >= 0)
    return (Result);
  else
    return (-Result);
}

int 
XParseGeometry (char *s, int *x, int *y, 
		unsigned int *width, unsigned int *height)
{
  int mask = NoValue;
  register char *strind;
  unsigned int tempWidth, tempHeight;
  int tempX, tempY;
  char *nextCharacter;
  
  if ((s == NULL) || (*s == '\0')) return (mask);
  if (*s == '=')
    s++;  /* ignore possible '=' at beg of geometry spec */
  
  strind = (char *)s;
  if (*strind != '+' && *strind != '-' && *strind != 'x') 
    {
      tempWidth = read_integer (strind, &nextCharacter);
      if (strind == nextCharacter) 
	return (0);
      strind = nextCharacter;
      mask |= WidthValue;
    }
  
  if (*strind == 'x' || *strind == 'X') 
    {	
      strind++;
      tempHeight = read_integer (strind, &nextCharacter);
      if (strind == nextCharacter)
	return (0);
      strind = nextCharacter;
      mask |= HeightValue;
    }
  
  if ((*strind == '+') || (*strind == '-')) 
    {
      if (*strind == '-') 
	{
	  strind++;
	  tempX = -read_integer (strind, &nextCharacter);
	  if (strind == nextCharacter)
	    return (0);
	  strind = nextCharacter;
	  mask |= XNegative;

	}
      else
	{	
	  strind++;
	  tempX = read_integer (strind, &nextCharacter);
	  if (strind == nextCharacter)
	    return (0);
	  strind = nextCharacter;
	}
      mask |= XValue;
      if ((*strind == '+') || (*strind == '-')) 
	{
	  if (*strind == '-') 
	    {
	      strind++;
	      tempY = -read_integer (strind, &nextCharacter);
	      if (strind == nextCharacter)
		return (0);
	      strind = nextCharacter;
	      mask |= YNegative;

	    }
	  else
	    {
	      strind++;
	      tempY = read_integer (strind, &nextCharacter);
	      if (strind == nextCharacter)
		return (0);
	      strind = nextCharacter;
	    }
	  mask |= YValue;
	}
    }
  
  /* If strind isn't at the end of the string the it's an invalid
     geometry specification. */
  
  if (*strind != '\0') return (0);
  
  if (mask & XValue)
    *x = tempX;
  if (mask & YValue)
    *y = tempY;
  if (mask & WidthValue)
    *width = tempWidth;
  if (mask & HeightValue)
    *height = tempHeight;
  return (mask);
}

COLORREF 
x_to_win32_color (const char *colorname)
{
  colormap_t *tail;
  int i;
  COLORREF ret;

  if (colorname[0] == '#')
    {
      /* Could be an old-style RGB Device specification.  */
      char *color;
      int size;
      color = colorname + 1;
      
      size = strlen(color);
      if (size == 3 || size == 6 || size == 9 || size == 12)
	{
	  UINT colorval;
	  int i, pos;
	  pos = 0;
	  size /= 3;
	  colorval = 0;
	  
	  for (i = 0; i < 3; i++)
	    {
	      char *end;
	      char t;
	      unsigned long value;

	      /* The check for 'x' in the following conditional takes into
		 account the fact that strtol allows a "0x" in front of
		 our numbers, and we don't.  */
	      if (!isxdigit(color[0]) || color[1] == 'x')
		break;
	      t = color[size];
	      color[size] = '\0';
	      value = strtoul(color, &end, 16);
	      color[size] = t;
	      if (errno == ERANGE || end - color != size)
		break;
	      switch (size)
		{
		case 1:
		  value = value * 0x10;
		  break;
		case 2:
		  break;
		case 3:
		  value /= 0x10;
		  break;
		case 4:
		  value /= 0x100;
		  break;
		}
	      colorval |= (value << pos);
	      pos += 0x8;
	      if (i == 2)
		{
		  return MYADJUSTRGB(colorval);
		}
	      color = end;
	    }
	}
    }
  else if (strnicmp(colorname, "rgb:", 4) == 0)
    {
      char *color;
      UINT colorval;
      int i, pos;
      pos = 0;

      colorval = 0;
      color = colorname + 4;
      for (i = 0; i < 3; i++)
	{
	  char *end;
	  unsigned long value;
	  
	  /* The check for 'x' in the following conditional takes into
	     account the fact that strtol allows a "0x" in front of
	     our numbers, and we don't.  */
	  if (!isxdigit(color[0]) || color[1] == 'x')
	    break;
	  value = strtoul(color, &end, 16);
	  if (errno == ERANGE)
	    break;
	  switch (end - color)
	    {
	    case 1:
	      value = value * 0x10 + value;
	      break;
	    case 2:
	      break;
	    case 3:
	      value /= 0x10;
	      break;
	    case 4:
	      value /= 0x100;
	      break;
	    default:
	      value = ULONG_MAX;
	    }
	  if (value == ULONG_MAX)
	    break;
	  colorval |= (value << pos);
	  pos += 0x8;
	  if (i == 2)
	    {
	      if (*end != '\0')
		break;
	      return MYADJUSTRGB(colorval);
	    }
	  if (*end != '/')
	    break;
	  color = end + 1;
	}
    }
  else if (strnicmp(colorname, "rgbi:", 5) == 0)
    {
      /* This is an RGB Intensity specification.  */
      char *color;
      UINT colorval;
      int i, pos;
      pos = 0;

      colorval = 0;
      color = colorname + 5;
      for (i = 0; i < 3; i++)
	{
	  char *end;
	  double value;
	  UINT val;

	  value = strtod(color, &end);
	  if (errno == ERANGE)
	    break;
	  if (value < 0.0 || value > 1.0)
	    break;
	  val = (UINT)(0x100 * value);
	  /* We used 0x100 instead of 0xFF to give an continuous
             range between 0.0 and 1.0 inclusive.  The next statement
             fixes the 1.0 case.  */
	  if (val == 0x100)
	    val = 0xFF;
	  colorval |= (val << pos);
	  pos += 0x8;
	  if (i == 2)
	    {
	      if (*end != '\0')
		break;
	      return MYADJUSTRGB(colorval);
	    }
	  if (*end != '/')
	    break;
	  color = end + 1;
	}
    }
  /* I am not going to attempt to handle any of the CIE color schemes
     or TekHVC, since I don't know the algorithms for conversion to
     RGB.  */
  
  for (tail = win32_color_map, i = 0; 
       i < sizeof(win32_color_map)/sizeof(colormap_t); 
       tail++, i++)
    {
      char *tem = tail->name;

      if (lstrcmpi (tem, colorname) == 0)
	{
	  ret = tail->colorref;
	  break;
	}
    }
  return ret;
}

