#========================================================================
#
# cmake-xpdf.txt
#
# CMake script for the Xpdf package.
#
# Copyright 2021 Glyph & Cog, LLC
#
#========================================================================

cmake_minimum_required(VERSION 2.8.12)

project(xpdf)

include(cmake-config.txt)

add_subdirectory(goo)
add_subdirectory(fofi)
add_subdirectory(splash)
add_subdirectory(xpdf)
add_subdirectory(xpdf-qt)

if (NOT HAVE_FREETYPE_H)
  message(WARNING "Couldn't find FreeType -- will not build pdftoppm, pdftopng, pdftohtml, or xpdf.")
endif ()

if (NOT PNG_FOUND)
  message(WARNING "Couldn't find libpng -- will not build pdftopng or pdftohtml.")
endif ()

if (NOT QT4_FOUND AND NOT Qt5Widgets_FOUND AND NOT Qt6Widgets_FOUND)
  message(WARNING "Couldn't find Qt4/5/6 -- will not build xpdf.")
endif ()

if (NOT MULTITHREADED)
  message(WARNING "Multithreading is disabled -- will not build xpdf.")
endif ()

if (NOT USE_EXCEPTIONS)
  message(WARNING "C++ exceptions are disabled -- will not build xpdf.")
endif ()
