rem file to build ttf2pt1 with Visual C++

cl -DWINDOWS -c ttf2pt1.c
cl -DWINDOWS -c pt1.c
cl -DWINDOWS -c ttf.c
cl -DWINDOWS -c t1asm.c
cl -o ttf2pt1 ttf2pt1.obj pt1.obj t1asm.obj ttf.obj 
cl -o t1asm -DWINDOWS -DSTANDALONE t1asm.c

