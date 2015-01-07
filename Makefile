nj:	nj.o app_dcll.o np_dcll.o 
	gcc nj.o app_dcll.o np_dcll.o -lpthread -o nj -ldl

nj.o:	nj.c nj.h
	gcc -c nj.c -o nj.o

app_dcll.o:	app_dcll.c app_dcll.h
	gcc -c app_dcll.c -o app_dcll.o

np_dcll.o:	np_dcll.c np_dcll.h
	gcc -c np_dcll.c -o np_dcll.o


