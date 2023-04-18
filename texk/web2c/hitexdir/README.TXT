HINT project home page
----------------------
The home page for the HINT project is at https://hint.userweb.mwn.de/


HiLaTeX
-------
The support of hitex for LaTeX is still in its initial stages.
To communicate with the LaTeX team and to offer up-to-date
LaTeX support files there is a github repository for general
access under: https://github.com/ruckertm/HINT

In the directory hilatex, it features the following subdirectories
- base
  the the basic LaTeX support files.
  Stable versions of these files are mirrored in the hitexdir/tex 
  subdirectory of TeX Live.
 
- tests various short test files

- bin binaries for hitex and hintview for testing

Displaying HINT Files
---------------------
To display HINT files, a viewing applications is required.
While in the future many such applications might exists,
currently there is only one (or one family) of such
applications: hintview.

The hintview application is maintained at https://github.com/ruckertm/HINT
in the hintview subdirectory.

If you do not want to mess with compiling hintview but prefer downloading
an app from an app store you should look at 
https://hint.userweb.mwn.de/hint/hintview.html
for more information and links.

Currently there are versions for Linux, Windows, Android, MacOS and iOS.
All viewers share the same backend to open and process HINT files;
and all do the rendering using OpenGL. The openGL interface is almost 
identical for all these versions.

Libraries that are used on all or some platforms are collected in the
external subdirectory for convenience.

The code that implements the user interface is different for the
different platforms.



