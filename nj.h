#ifndef SYSTYPES_H
#define SYSTYPES_H
#include<sys/types.h>
#endif

#ifndef SIGNAL_H
#define SIGNAL_H
#include<signal.h>
#endif

#ifndef SYSSOCKET_H
#define SYSSOCKET_H
#include<sys/socket.h>
#endif

#ifndef SYSUN_H
#define SYUN_H
#include<sys/un.h>
#endif


#ifndef PTHREAD_H
#define PTHREAD_H
#include<pthread.h>
#endif

#ifndef SYSTYPES_H
#define SYSTYPES_H
#include<sys/types.h>
#endif

#ifndef SYSSTAT_H
#define SYSSTAT_H
#include<sys/stat.h>
#endif

#ifndef FCNTL_H
#define FCNTL_H
#include <fcntl.h>
#endif


#ifndef STDIO_H
#define STDIO_H
#include<stdio.h>
#endif

#ifndef STDLIB_H
#define STDLIB_H
#include<stdlib.h>
#endif

#ifndef UNISTD_H
#define UNISTD_H
#include<unistd.h>
#endif

#ifndef DLFCN_H
#define DLFCN_H
#include <dlfcn.h>
#endif

#ifndef ALREXST
#define ALREXST -1
#endif

#ifndef NOTFND
#define NOTFND -2
#endif


#define ERROR -1
#define SUCCESS 1

#include "app_dcll.h"
#include "np_dcll.h"

// Create 2 sockets - app_reg, app_unreg
// Fork 2 threads - app_reg, app_unreg
#define AppGetnotify "app_getnotify"
#define AppReg "app_reg"
#define AppUnReg "app_unreg"
#define NpReg "np_reg"
#define NpUnReg "np_unreg"

#define QLEN 32

int register_app(char *);
int unregister_app(char *);
int register_np(char *);
int unregister_np(char *);

void *AppRegMethod(void *);
void *AppUnRegMethod(void *);
void *NpRegMethod(void *);
void *NpUnRegMethod(void *);
void *AppGetNotifyMethod(void *);
void *NpGetNotifyMethod(void *);

typedef struct threadArgs {
                int sock, msgsock, rval;
                struct sockaddr_un server;
                char buf[1024];

}threadArgs;

typedef struct getnotify_threadArgs {
		char argssend[1024];//getnotify thread will receive string of required notification request from this field 
		char argsrecv[1024];//getnotify of np will fill the notification response in this field
}getnotify_threadArgs;


int register_app(char *) ; // function returns negative value if error in registering app 
// this function will add node in the app list  for first registration of app with particular NP

int register_np(char *) ; //function returns negative value if error in registering np
// this function will add node in the np list for np registration of np 

int unregister_app(char *);// function returns negative value if error in unregistering app eg(app not found in list)
int unregister_np(char *); // function returns negative valuse if error in unregistering np eg(np not found in list)
char* getnotify_app(char *) ; // function that returns the notification strinf to app
