#ifndef SYSTYPES_H
#define SYSTYPES_H
#include<sys/types.h>
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

#ifndef SIGNAL_H
#define SIGNAL_H
#include<signal.h>
#endif

#define AppGetnotify "app_getnotify"
#define AppReg "app_reg"
#define AppUnReg "app_unreg"



int app_register(char *key_val_string);
int app_unregister(char *key_val_string);
int app_getnotify(int pid, char *key_val_string, char choice); /*choice is 1 for blocking and 0 for non_blocking......*/



