set CC=cl
set CXX=cl
set FT2DIR=..\freetype-2.5.3
rem *** Set PNGDIR and ZLIBDIR to build pdftohtml
rem set PNGDIR=..\libpng-1.5.12
rem set ZLIBDIR=..\zlib-1.2.7
set CFLAGS=/I.. /I..\goo /I..\fofi /I..\splash /O2 /nologo /I%FT2DIR%\include
set CXXFLAGS=%CFLAGS% /TP
set LIBPROG=lib
set LINKFLAGS=/MT /nologo

copy aconf-win32.h aconf.h

cd goo
%CXX% %CXXFLAGS% /c GHash.cc
%CXX% %CXXFLAGS% /c GList.cc
%CXX% %CXXFLAGS% /c GString.cc
%CXX% %CXXFLAGS% /c gmem.cc
%CXX% %CXXFLAGS% /c gmempp.cc
%CXX% %CXXFLAGS% /c gfile.cc
%CC% %CFLAGS% /c parseargs.c
%LIBPROG% /nologo /out:Goo.lib GHash.obj GList.obj GString.obj gmempp.obj gfile.obj gmem.obj parseargs.obj

cd ..\fofi
%CXX% %CXXFLAGS% /c FoFiBase.cc
%CXX% %CXXFLAGS% /c FoFiEncodings.cc
%CXX% %CXXFLAGS% /c FoFiIdentifier.cc
%CXX% %CXXFLAGS% /c FoFiTrueType.cc
%CXX% %CXXFLAGS% /c FoFiType1.cc
%CXX% %CXXFLAGS% /c FoFiType1C.cc
%LIBPROG% /nologo /out:fofi.lib FoFiBase.obj FoFiEncodings.obj FoFiIdentifier.obj FoFiTrueType.obj FoFiType1.obj FoFiType1C.obj

cd ..\splash
%CXX% %CXXFLAGS% /c Splash.cc
%CXX% %CXXFLAGS% /c SplashBitmap.cc
%CXX% %CXXFLAGS% /c SplashClip.cc
%CXX% %CXXFLAGS% /c SplashFTFont.cc
%CXX% %CXXFLAGS% /c SplashFTFontEngine.cc
%CXX% %CXXFLAGS% /c SplashFTFontFile.cc
%CXX% %CXXFLAGS% /c SplashFont.cc
%CXX% %CXXFLAGS% /c SplashFontEngine.cc
%CXX% %CXXFLAGS% /c SplashFontFile.cc
%CXX% %CXXFLAGS% /c SplashFontFileID.cc
%CXX% %CXXFLAGS% /c SplashPath.cc
%CXX% %CXXFLAGS% /c SplashPattern.cc
%CXX% %CXXFLAGS% /c SplashScreen.cc
%CXX% %CXXFLAGS% /c SplashState.cc
%CXX% %CXXFLAGS% /c SplashXPath.cc
%CXX% %CXXFLAGS% /c SplashXPathScanner.cc
%LIBPROG% /nologo /out:splash.lib Splash.obj SplashBitmap.obj SplashClip.obj SplashFTFont.obj SplashFTFontEngine.obj SplashFTFontFile.obj SplashFont.obj SplashFontEngine.obj SplashFontFile.obj SplashFontFileID.obj SplashPath.obj SplashPattern.obj SplashScreen.obj SplashState.obj SplashXPath.obj SplashXPathScanner.obj

cd ..\xpdf
%CXX% %CXXFLAGS% /c AcroForm.cc
%CXX% %CXXFLAGS% /c Annot.cc
%CXX% %CXXFLAGS% /c Array.cc
%CXX% %CXXFLAGS% /c BuiltinFont.cc
%CXX% %CXXFLAGS% /c BuiltinFontTables.cc
%CXX% %CXXFLAGS% /c CMap.cc
%CXX% %CXXFLAGS% /c Catalog.cc
%CXX% %CXXFLAGS% /c CharCodeToUnicode.cc
%CXX% %CXXFLAGS% /c Decrypt.cc
%CXX% %CXXFLAGS% /c Dict.cc
%CXX% %CXXFLAGS% /c Error.cc
%CXX% %CXXFLAGS% /c FontEncodingTables.cc
%CXX% %CXXFLAGS% /c Form.cc
%CXX% %CXXFLAGS% /c Function.cc
%CXX% %CXXFLAGS% /c Gfx.cc
%CXX% %CXXFLAGS% /c GfxFont.cc
%CXX% %CXXFLAGS% /c GfxState.cc
%CXX% %CXXFLAGS% /c GlobalParams.cc
%CXX% %CXXFLAGS% /c ImageOutputDev.cc
%CXX% %CXXFLAGS% /c JArithmeticDecoder.cc
%CXX% %CXXFLAGS% /c JBIG2Stream.cc
%CXX% %CXXFLAGS% /c JPXStream.cc
%CXX% %CXXFLAGS% /c Lexer.cc
%CXX% %CXXFLAGS% /c Link.cc
%CXX% %CXXFLAGS% /c NameToCharCode.cc
%CXX% %CXXFLAGS% /c Object.cc
%CXX% %CXXFLAGS% /c OptionalContent.cc
%CXX% %CXXFLAGS% /c Outline.cc
%CXX% %CXXFLAGS% /c OutputDev.cc
%CXX% %CXXFLAGS% /c PDFDoc.cc
%CXX% %CXXFLAGS% /c PDFDocEncoding.cc
%CXX% %CXXFLAGS% /c PSOutputDev.cc
%CXX% %CXXFLAGS% /c PSTokenizer.cc
%CXX% %CXXFLAGS% /c Page.cc
%CXX% %CXXFLAGS% /c Parser.cc
%CXX% %CXXFLAGS% /c PreScanOutputDev.cc
%CXX% %CXXFLAGS% /c SecurityHandler.cc
%CXX% %CXXFLAGS% /c SplashOutputDev.cc
%CXX% %CXXFLAGS% /c Stream.cc
%CXX% %CXXFLAGS% /c TextOutputDev.cc
%CXX% %CXXFLAGS% /c TextString.cc
%CXX% %CXXFLAGS% /c UnicodeMap.cc
%CXX% %CXXFLAGS% /c UnicodeTypeTable.cc
%CXX% %CXXFLAGS% /c XFAForm.cc
%CXX% %CXXFLAGS% /c XRef.cc
%CXX% %CXXFLAGS% /c Zoox.cc
%CXX% %CXXFLAGS% /c pdftops.cc
%CXX% %CXXFLAGS% /c pdftotext.cc
%CXX% %CXXFLAGS% /c pdftoppm.cc
%CXX% %CXXFLAGS% /c pdfinfo.cc
%CXX% %CXXFLAGS% /c pdffonts.cc
%CXX% %CXXFLAGS% /c pdfdetach.cc
%CXX% %CXXFLAGS% /c pdfimages.cc

%CXX% %LINKFLAGS% /Fepdftops.exe AcroForm.obj Annot.obj Array.obj BuiltinFont.obj BuiltinFontTables.obj Catalog.obj CharCodeToUnicode.obj CMap.obj Decrypt.obj Dict.obj Error.obj FontEncodingTables.obj Form.obj Function.obj Gfx.obj GfxFont.obj GfxState.obj GlobalParams.obj JArithmeticDecoder.obj JBIG2Stream.obj JPXStream.obj Lexer.obj Link.obj NameToCharCode.obj Object.obj OptionalContent.obj Outline.obj OutputDev.obj Page.obj Parser.obj PDFDoc.obj PDFDocEncoding.obj PSOutputDev.obj PSTokenizer.obj PreScanOutputDev.obj SecurityHandler.obj SplashOutputDev.obj Stream.obj TextString.obj UnicodeMap.obj XFAForm.obj XRef.obj Zoox.obj pdftops.obj ..\splash\splash.lib ..\fofi\fofi.lib ..\goo\Goo.lib %FT2DIR%\freetype2.lib shell32.lib user32.lib gdi32.lib advapi32.lib

%CXX% %LINKFLAGS% /Fepdftotext.exe AcroForm.obj Annot.obj Array.obj BuiltinFont.obj BuiltinFontTables.obj Catalog.obj CharCodeToUnicode.obj CMap.obj Decrypt.obj Dict.obj Error.obj FontEncodingTables.obj Form.obj Function.obj Gfx.obj GfxFont.obj GfxState.obj GlobalParams.obj JArithmeticDecoder.obj JBIG2Stream.obj JPXStream.obj Lexer.obj Link.obj NameToCharCode.obj Object.obj OptionalContent.obj Outline.obj OutputDev.obj Page.obj Parser.obj PDFDoc.obj PDFDocEncoding.obj PSTokenizer.obj SecurityHandler.obj Stream.obj TextOutputDev.obj TextString.obj UnicodeMap.obj UnicodeTypeTable.obj XFAForm.obj XRef.obj Zoox.obj pdftotext.obj ..\fofi\fofi.lib ..\goo\Goo.lib shell32.lib user32.lib gdi32.lib advapi32.lib

%CXX% %LINKFLAGS% /Fepdftoppm.exe AcroForm.obj Annot.obj Array.obj BuiltinFont.obj BuiltinFontTables.obj Catalog.obj CharCodeToUnicode.obj CMap.obj Decrypt.obj Dict.obj Error.obj FontEncodingTables.obj Form.obj Function.obj Gfx.obj GfxFont.obj GfxState.obj GlobalParams.obj JArithmeticDecoder.obj JBIG2Stream.obj JPXStream.obj Lexer.obj Link.obj NameToCharCode.obj Object.obj OptionalContent.obj Outline.obj OutputDev.obj Page.obj Parser.obj PDFDoc.obj PDFDocEncoding.obj PSTokenizer.obj SecurityHandler.obj SplashOutputDev.obj Stream.obj TextString.obj UnicodeMap.obj UnicodeTypeTable.obj XFAForm.obj XRef.obj Zoox.obj pdftoppm.obj ..\splash\splash.lib ..\fofi\fofi.lib ..\goo\Goo.lib %FT2DIR%\freetype2.lib shell32.lib user32.lib gdi32.lib advapi32.lib

%CXX% %LINKFLAGS% /Fepdfinfo.exe AcroForm.obj Annot.obj Array.obj BuiltinFont.obj BuiltinFontTables.obj Catalog.obj CharCodeToUnicode.obj CMap.obj Decrypt.obj Dict.obj Error.obj FontEncodingTables.obj Form.obj Function.obj Gfx.obj GfxFont.obj GfxState.obj GlobalParams.obj JArithmeticDecoder.obj JBIG2Stream.obj JPXStream.obj Lexer.obj Link.obj NameToCharCode.obj Object.obj OptionalContent.obj Outline.obj OutputDev.obj Page.obj Parser.obj PDFDoc.obj PDFDocEncoding.obj PSTokenizer.obj SecurityHandler.obj Stream.obj TextString.obj UnicodeMap.obj XFAForm.obj XRef.obj Zoox.obj pdfinfo.obj ..\fofi\fofi.lib ..\goo\Goo.lib shell32.lib user32.lib gdi32.lib advapi32.lib

%CXX% %LINKFLAGS% /Fepdffonts.exe AcroForm.obj Annot.obj Array.obj BuiltinFont.obj BuiltinFontTables.obj Catalog.obj CharCodeToUnicode.obj CMap.obj Decrypt.obj Dict.obj Error.obj FontEncodingTables.obj Form.obj Function.obj Gfx.obj GfxFont.obj GfxState.obj GlobalParams.obj JArithmeticDecoder.obj JBIG2Stream.obj JPXStream.obj Lexer.obj Link.obj NameToCharCode.obj Object.obj OptionalContent.obj Outline.obj OutputDev.obj Page.obj Parser.obj PDFDoc.obj PDFDocEncoding.obj PSTokenizer.obj SecurityHandler.obj Stream.obj TextString.obj UnicodeMap.obj XFAForm.obj XRef.obj Zoox.obj pdffonts.obj ..\fofi\fofi.lib ..\goo\Goo.lib shell32.lib user32.lib gdi32.lib advapi32.lib

%CXX% %LINKFLAGS% /Fepdfdetach.exe AcroForm.obj Annot.obj Array.obj BuiltinFont.obj BuiltinFontTables.obj Catalog.obj CharCodeToUnicode.obj CMap.obj Decrypt.obj Dict.obj Error.obj FontEncodingTables.obj Form.obj Function.obj Gfx.obj GfxFont.obj GfxState.obj GlobalParams.obj JArithmeticDecoder.obj JBIG2Stream.obj JPXStream.obj Lexer.obj Link.obj NameToCharCode.obj Object.obj OptionalContent.obj Outline.obj OutputDev.obj Page.obj Parser.obj PDFDoc.obj PDFDocEncoding.obj PSTokenizer.obj SecurityHandler.obj Stream.obj TextString.obj UnicodeMap.obj XFAForm.obj XRef.obj Zoox.obj pdfdetach.obj ..\fofi\fofi.lib ..\goo\Goo.lib shell32.lib user32.lib gdi32.lib advapi32.lib

%CXX% %LINKFLAGS% /Fepdfimages.exe AcroForm.obj Annot.obj Array.obj BuiltinFont.obj BuiltinFontTables.obj Catalog.obj CharCodeToUnicode.obj CMap.obj Decrypt.obj Dict.obj Error.obj FontEncodingTables.obj Form.obj Function.obj Gfx.obj GfxFont.obj GfxState.obj GlobalParams.obj ImageOutputDev.obj JArithmeticDecoder.obj JBIG2Stream.obj JPXStream.obj Lexer.obj Link.obj NameToCharCode.obj Object.obj OptionalContent.obj Outline.obj OutputDev.obj Page.obj Parser.obj PDFDoc.obj PDFDocEncoding.obj PSTokenizer.obj SecurityHandler.obj Stream.obj TextString.obj UnicodeMap.obj XFAForm.obj XRef.obj Zoox.obj pdfimages.obj ..\fofi\fofi.lib ..\goo\Goo.lib shell32.lib user32.lib gdi32.lib advapi32.lib

if x%PNGDIR% == x goto noHTML
if x%ZLIBDIR% == x goto noHTML

%CXX% %CXXFLAGS% /I%PNGDIR% /I%ZLIBDIR% /c pdftopng.cc
%CXX% %LINKFLAGS% /Fepdftopng.exe AcroForm.obj Annot.obj Array.obj BuiltinFont.obj BuiltinFontTables.obj Catalog.obj CharCodeToUnicode.obj CMap.obj Decrypt.obj Dict.obj Error.obj FontEncodingTables.obj Form.obj Function.obj Gfx.obj GfxFont.obj GfxState.obj GlobalParams.obj JArithmeticDecoder.obj JBIG2Stream.obj JPXStream.obj Lexer.obj Link.obj NameToCharCode.obj Object.obj OptionalContent.obj Outline.obj OutputDev.obj Page.obj Parser.obj PDFDoc.obj PDFDocEncoding.obj PSTokenizer.obj SecurityHandler.obj SplashOutputDev.obj Stream.obj TextString.obj UnicodeMap.obj UnicodeTypeTable.obj XFAForm.obj XRef.obj Zoox.obj pdftopng.obj ..\splash\splash.lib ..\fofi\fofi.lib ..\goo\Goo.lib %FT2DIR%\freetype2.lib %PNGDIR%\libpng.lib %ZLIBDIR%\zlib.lib shell32.lib user32.lib gdi32.lib advapi32.lib

echo "building pdftohtml"
%CXX% %CXXFLAGS% /I%PNGDIR% /I%ZLIBDIR% /c HTMLGen.cc
%CXX% %CXXFLAGS% /I%PNGDIR% /I%ZLIBDIR% /c pdftohtml.cc
%CXX% %LINKFLAGS% /Fepdftohtml.exe AcroForm.obj Annot.obj Array.obj BuiltinFont.obj BuiltinFontTables.obj Catalog.obj CharCodeToUnicode.obj CMap.obj Decrypt.obj Dict.obj Error.obj FontEncodingTables.obj Form.obj Function.obj Gfx.obj GfxFont.obj GfxState.obj GlobalParams.obj HTMLGen.obj JArithmeticDecoder.obj JBIG2Stream.obj JPXStream.obj Lexer.obj Link.obj NameToCharCode.obj Object.obj OptionalContent.obj Outline.obj OutputDev.obj Page.obj Parser.obj PDFDoc.obj PDFDocEncoding.obj PSTokenizer.obj SecurityHandler.obj SplashOutputDev.obj Stream.obj TextOutputDev.obj TextString.obj UnicodeMap.obj UnicodeTypeTable.obj XFAForm.obj XRef.obj Zoox.obj pdftohtml.obj ..\splash\splash.lib ..\fofi\fofi.lib ..\goo\Goo.lib %FT2DIR%\freetype2.lib %PNGDIR%\libpng.lib %ZLIBDIR%\zlib.lib shell32.lib user32.lib gdi32.lib advapi32.lib

:noHTML

cd ..
