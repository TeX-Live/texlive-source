Copyright:

The originally 'runscript' program was written by in 2009 by T.M.Trzeciak and is
public domain. This derived mtxrun program is an adapted version by Hans Hagen and
Luigi Scarso.

Comment:

In ConTeXt MkIV we have two core scripts: luatools.lua and mtxrun.lua where the
second one is used to launch other scripts. The mtxrun.exe program calls luatex.exe.

Normally a user will use a call like:

    mtxrun    --script font --reload

Here mtxrun is a lua script. In order to avoid the usage of a cmd file on windows this
runner will start texlua directly. In TeXlive a runner is added for each cmd file but
we don't want that overhead (and extra files). By using an exe we can call these
scripts in batch files without the need for using call.

The mtxrun.exe file can be copied to a mtxrunjit.exe file in which case luajittex.exe
is called.

    mtxrunjit --script font --reload

We also don't want to use other runners, like those that use kpse to locate the script
as this is exactly what mtxrun itself is doing already. Therefore the runscript program
is adapted to a more direct approach suitable for mtxrun.

Compilation:

with gcc (size optimized):

    gcc -Os -s -shared -o mtxrun.dll mtxrun_dll.c
    gcc -Os -s -o mtxrun.exe mtxrun_exe.c -L./ -lmtxrun

with tcc (ver. 0.9.24), extra small size

    tcc -shared -o runscript.dll runscript_dll.c
    tcc -o runscript.exe runscript_exe.c runscript.def
