This is a first - and very basic - version of a viewer for HINT files.
HINT files can be produced from TeX sources using hitex.

Build
-----
The present code is tested under Linux, but if you have the
right libraries it should compile under other Linux like systems.

The build process is defined by the Makefile in the src subdirectory.
Just cd src/ and run make - if you are lucky it will produce the 
hintview executable.

You will need the following libraries and associated header files:
libdl, libglew, libglfw3 libfreetype, libX11, libXrandr, libXinerama, 


Documentation
-------------
The command line options, keystrokes, and mouse actions are described
in hintview.1 ind the man/ subdirectory.





