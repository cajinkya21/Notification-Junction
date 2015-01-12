# CC variable contains the compiler that we want to use
CC=gcc


all: nj libinotify np_register dummyapp stat
 

nj:	nj.o app_dcll.o np_dcll.o 
	$(CC) nj.o app_dcll.o np_dcll.o -lpthread -o nj -ldl

nj.o:	nj.c nj.h
	$(CC) -c nj.c -o nj.o

app_dcll.o:	app_dcll.c app_dcll.h
	$(CC) -c app_dcll.c -o app_dcll.o

np_dcll.o:	np_dcll.c np_dcll.h
	$(CC) -c np_dcll.c -o np_dcll.o
#Target 2
clean: 
	rm  *.o
	rm nj
	rm libinotify.so
	rm dummyapp
	rm np_register
	rm stat
	

 #Target 3
libinotify: inotify.o
	$(CC) inotify.o -shared -o libinotify.so

inotify.o: inotify.c inotify.h
	$(CC) -c -fPIC inotify.c -o inotify.o

#Target 4
np_register: 
	$(CC) np_reg.c -o np_register


#Target 4 
dummyapp: dummyapp.o nonblocklib.o
	$(CC) dummyapp.o nonblocklib.o -o dummyapp

dummyapp.o: dummyapp.c
	$(CC) -c dummyapp.c -o dummyapp.o

nonblocklib.o: nonblocklib.c lib.h
	$(CC) -c nonblocklib.c -o nonblocklib.o 

stat: stat.o
	$(CC)  stat.o -o stat

stat.o:
	$(CC) -c stat.c -o stat.o 



