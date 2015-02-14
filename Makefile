#CC variable contains the compiler that we want to use
CC=gcc

all: nj libinotify np_register dummyapp  app_register app_unregister np_unregister app_getnotify dummyapp2

nj:	nj.o app_dcll.o np_dcll.o 
	$(CC) -g -Wall nj.o app_dcll.o np_dcll.o -lpthread -o nj -ldl

nj.o: nj.c nj.h
	$(CC) -g -Wall -c nj.c -o nj.o

app_dcll.o:	app_dcll.c app_dcll.h
	$(CC) -g -Wall -c app_dcll.c -o app_dcll.o

np_dcll.o: np_dcll.c np_dcll.h
	$(CC) -g -Wall -c np_dcll.c -o np_dcll.o

#Target 2

clean: 
#commands
	rm  *.o
	rm nj
	rm libnpinotify.so
	rm dummyapp
	rm np_register
	
	rm app_getnotify
	rm app_register
	rm app_unregister
	rm np_unregister
#socket files
	rm app_reg_sock
	rm app_unreg_sock
	rm np_reg_sock
	rm np_unreg_sock
	rm app_getn_sock
	rm statsock
#other files
	rm File_PIDS.txt
	rm dummyapp2
	rm *.txt

#Target 3
libinotify: inotify.o
	$(CC) -Wall -g inotify.o -shared -o libnpinotify.so

inotify.o: inotify.c inotify.h
	$(CC) -Wall -g -c -fPIC inotify.c -o inotify.o

#Target 4
np_register: 
	$(CC) -Wall -g np_register.c -o np_register

#Target 4 
dummyapp: dummyapp.o nj_nonblocklib.o
	$(CC) -Wall -g dummyapp.o nj_nonblocklib.o -o dummyapp

dummyapp.o: dummyapp.c
	$(CC) -Wall -c -g dummyapp.c -o dummyapp.o

nj_nonblocklib.o: nj_nonblocklib.c lib.h
	$(CC) -Wall -c -g nj_nonblocklib.c -o nj_nonblocklib.o 

#Target 5
dummyapp2: dummyapp2.o nj_nonblocklib.o
	$(CC) -Wall -g  dummyapp2.c nj_nonblocklib.o -o dummyapp2

dummyapp2.o:dummyapp2.c 
	$(CC) -Wall -g -c dummyapp2.c -o dummyapp2.o

#Target 5

#Target 6
app_register: app_register.c 
	$(CC) -Wall -g app_register.c -o app_register

#Target 7
app_unregister: app_unregister.c
	$(CC) -Wall -g app_unregister.c -o app_unregister

#Target 8
np_unregister: np_unregister.c
	$(CC) -Wall -g np_unregister.c -o np_unregister

#Target 9
app_getnotify: app_getnotify.c
	$(CC) -Wall -g app_getnotify.c -o app_getnotify
