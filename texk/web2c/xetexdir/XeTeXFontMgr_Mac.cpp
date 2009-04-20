/****************************************************************************\
 Copyright (C) 2009 Peter Breitenlohner <tex-live@tug.org>

 This file is free software; the copyright holder
 gives unlimited permission to copy and/or distribute it,
 with or without modifications, as long as this notice is preserved.

 Automake (1.10.2) does not generate a rule to compile
 the Objective C++ file XeTeXFontMgr_Mac.mm.
 Since that file is to be compiled with '$(CXX) -ObjC++' we provide
 this C++ wrapper XeTeXFontMgr_Mac.cpp including the Objective C++ file.
\****************************************************************************/
#include "XeTeXFontMgr_Mac.mm"
