#CC variable contains the compiler that we want to use
CC=gcc


all: nj libinotify np_register dummyapp stat app_register app_unregister np_unregister app_getnotify
 

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
	rm app_getnotify
	rm app_register
	rm app_unregister
	rm np_unregister


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

#Target 5
stat: stat.o
	$(CC)  stat.o -o stat

stat.o:
	$(CC) -c stat.c -o stat.o 


#Target 6
app_register: app_reg.c 
	$(CC) app_reg.c -o app_register

#Target 7
app_unregister: app_unreg.c
	$(CC) app_unreg.c -o app_unregister

#Target 8
np_unregister: np_unreg.c
	$(CC) np_unreg.c -o np_unregister

#Target 9
app_getnotify: app_getnotify.c
	$(CC) app_getnotify.c -o app_getnotify
