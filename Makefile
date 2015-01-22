#C variable contains the compiler that we want to use
CC=gcc


all: nj libinotify np_register dummyapp stat app_register app_unregister np_unregister app_getnotify dummyapp2
 

nj:	nj.o app_dcll.o np_dcll.o 
	$(CC) -g nj.o app_dcll.o np_dcll.o -lpthread -o nj -ldl

nj.o:	nj.c nj.h
	$(CC) -g -c nj.c -o nj.o

app_dcll.o:	app_dcll.c app_dcll.h
	$(CC) -g -c app_dcll.c -o app_dcll.o

np_dcll.o:	np_dcll.c np_dcll.h
	$(CC) -g -c np_dcll.c -o np_dcll.o
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
	rm app_reg
	rm np_reg
	rm np_unreg
	rm app_getn
	rm statsock
	rm File_PIDS.txt
	rm dummyapp2
	rm app_unreg
    
 #Target 3
libinotify: inotify.o
	$(CC) -g inotify.o -shared -o libinotify.so

inotify.o: inotify.c inotify.h
	$(CC) -g -c -fPIC inotify.c -o inotify.o

#Target 4
np_register: 
	$(CC) -g np_reg.c -o np_register


#Target 4 
dummyapp: dummyapp.o nonblocklib.o
	$(CC) -g dummyapp.o nonblocklib.o -o dummyapp

dummyapp.o: dummyapp.c
	$(CC) -c -g dummyapp.c -o dummyapp.o

nonblocklib.o: nonblocklib.c lib.h
	$(CC) -c -g nonblocklib.c -o nonblocklib.o 
	
#Target 5
dummyapp2: dummyapp2.o nonblocklib.o
	$(CC) -g  dummyapp2.c nonblocklib.o -o dummyapp2

dummyapp2.o:dummyapp2.c 
	$(CC) -g -c dummyapp2.c -o dummyapp2.o
    
#Target 5
stat: stat.o
	$(CC) -g  stat.o -o stat

stat.o:
	$(CC) -g -c stat.c -o stat.o 


#Target 6
app_register: app_reg.c 
	$(CC) -g app_reg.c -o app_register

#Target 7
app_unregister: app_unreg.c
	$(CC) -g app_unreg.c -o app_unregister

#Target 8
np_unregister: np_unreg.c
	$(CC) -g np_unreg.c -o np_unregister

#Target 9
app_getnotify: app_getnotify.c
	$(CC) -g app_getnotify.c -o app_getnotify
