/*This is header file for nj.c (notification juncitoncode)*/
#ifndef _NJ_H
#define _NJ_H	1	


#include<sys/types.h>
#include<signal.h>
#include<sys/socket.h>
#include<sys/un.h>
#include<pthread.h>
#include<sys/types.h>
#include<sys/stat.h>
#include <fcntl.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include <dlfcn.h>
#define ALREXST -1
#define NOTFND -2
#define ERROR -1
#define SUCCESS 1

#include "app_dcll.h"
#include "np_dcll.h"

/* Create 2 sockets - app_reg, app_unreg*/
/* Fork 2 threads - app_reg, app_unreg*/
#define AppGetnotify "app_getnotify"
#define AppReg "app_reg"
#define AppUnReg "app_unreg"
#define NpReg "np_reg"
#define NpUnReg "np_unreg"
#define StatSocket "statsock"

#define QLEN 32
/*FUNCTION DECLARATIONS*/

void print_stat();
int register_app(char *);
int unregister_app(char *);
int register_np(char *);
int unregister_np(char *);

void *PrintStat(void *);
void *AppRegMethod(void *);
void *AppUnRegMethod(void *);
void *NpRegMethod(void *);
void *NpUnRegMethod(void *);
void *AppGetNotifyMethod(void *);
void *NpGetNotifyMethod(void *);
/*STRUCTURE FOR ARGUMENTS TO BE PASSED TO THREAD*/
typedef struct threadArgs {
                int sock, msgsock, rval;
                struct sockaddr_un server;
                char buf[1024];

}threadArgs;
/*STRUCTURE FOR ARGUMENTS TO BE PASSED TO GETNOTIFY THREAD*/
typedef struct getnotify_threadArgs {
		char argssend[1024];/*getnotify thread will receive string of required notification request from this field */
		char argsrecv[1024];/*getnotify of np will fill the notification response in this field*/
}getnotify_threadArgs;

/* function returns negative value if error in registering app This function will add node in the app list  for first registration of app with particular NP*/
int register_app(char *); 

/*function returns negative value if error in registering np this function will add node in the np list for np registration of np */
int register_np(char *); 

/* function returns negative value if error in unregistering app eg(app not found in list)*/
int unregister_app(char *);

/*function returns negative valuse if error in unregistering np eg(np not found in list)*/
int unregister_np(char *); 

/*function to get notification */
char* getnotify_app(char *); 

#endif /*nj.h*/
