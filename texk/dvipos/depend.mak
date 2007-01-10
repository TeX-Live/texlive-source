dvipos.o: dvipos.c dvicore.h utils.h
dvicore.o: dvicore.c dvicore.h tfm.h utils.h
tfm.o: tfm.c tfm.h dvicore.h utils.h
utils.o: utils.c utils.h
