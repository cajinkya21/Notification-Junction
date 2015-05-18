#CC variable contains the compiler that we want to use
CC=gcc
NJDESTDIR=/usr/bin
NJCOMMANDDIR=/usr/local/bin
NJLIBDIR=/usr/lib
NJAPPDIR=/usr/local/nj


all: nj libinotify np_register dummyapp stats app_register app_unregister np_unregister app_getnotify  nj_nonblocklib.so libtcpdump dummyappnonblock libnj_nonblocklib.so

nj:	nj.o app_dcll.o np_dcll.o rdwr_lock.o
	$(CC) -g -Wall nj.o app_dcll.o np_dcll.o rdwr_lock.o -lpthread -o nj -ldl

nj.o: nj.c nj.h
	$(CC) -g -Wall -c nj.c -o nj.o

app_dcll.o:	app_dcll.c app_dcll.h
	$(CC) -g -Wall -c app_dcll.c -o app_dcll.o

np_dcll.o: np_dcll.c np_dcll.h
	$(CC) -g -Wall -c np_dcll.c -o np_dcll.o

rdwr_lock.o : rdwr_lock.c rdwr_lock.h
	$(CC)  -Wall -g -c rdwr_lock.c -o rdwr_lock.o 
#Target 2

clean: 
#commands
	rm  *.o
	rm nj
	rm libnpinotify.so
	rm dummyapp
	rm np_register
	rm stats
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

libtcpdump: tcpdump.o
	$(CC) -Wall -g tcpdump.o -shared -o libnptcpdump.so -lpcap

tcpdump.o: tcpdump.c tcpdump.h
	$(CC) -Wall -g -c -fPIC tcpdump.c -o tcpdump.o -lpcap

#Target 4
np_register: 
	$(CC) -Wall -g np_register.c -o np_register

#Target 4 
dummyappnonblock: dummyappnonblock.o nj_nonblocklib.o
	$(CC) -Wall -g dummyappnonblock.o nj_nonblocklib.o -o dummyappnonblock
	
dummyappnonblock.o: dummyappnonblock.c
	$(CC) -Wall -c -g dummyappnonblock.c -o dummyappnonblock.o

dummyapp: dummyapp.o nj_nonblocklib.o
	$(CC) -Wall -g dummyapp.o nj_nonblocklib.o -o dummyapp

dummyapp.o: dummyapp.c
	$(CC) -Wall -c -g dummyapp.c -o dummyapp.o

nj_nonblocklib.o: nj_nonblocklib.c lib.h
	$(CC) -Wall -c -g -fPIC nj_nonblocklib.c -o nj_nonblocklib.o 

libnj_nonblocklib.so: nj_nonblocklib.o
	$(CC) -Wall -g nj_nonblocklib.o -shared -o libnj_nonblocklib.so
	
nj_nonblocklib.so: nj_nonblocklib.o
	$(CC) -Wall -g nj_nonblocklib.o -shared -o nj_nonblocklib.so
	

#Target 5
stats: stats.o
	$(CC) -g  stats.o -o stats

stats.o:
	$(CC) -g -c stats.c -o stats.o 


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
	
#Target 10 for installation 
install: all
	mkdir /usr/local/nj
	install nj $(NJDESTDIR)
	install np_register $(NJCOMMANDDIR)
	install np_unregister $(NJCOMMANDDIR)
	install app_register $(NJCOMMANDDIR)
	install app_unregister $(NJCOMMANDDIR)
	install stats $(NJCOMMANDDIR)
	install libnpinotify.so $(NJLIBDIR)
	install libnptcpdump.so $(NJLIBDIR)
	install libnj_nonblocklib.so $(NJLIBDIR)
	install dummyapp $(NJAPPDIR)


uninstall: 
	rm -rf /usr/local/nj
	rm $(NJDESTDIR)/nj
	rm $(NJCOMMANDDIR)/np_register
	rm $(NJCOMMANDDIR)/np_unregister
	rm $(NJCOMMANDDIR)/app_register
	rm $(NJCOMMANDDIR)/app_unregister
	rm $(NJCOMMANDDIR)/stats
	rm $(NJLIBDIR)/libnpinotify.so
	rm $(NJLIBDIR)/libnptcpdump.so
	rm $(NJLIBDIR)/libnj_nonblocklib.so
	rm $(NJAPPDIR)/dummyapp
	


