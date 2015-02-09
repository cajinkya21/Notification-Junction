#ifndef _INOTIFY_H
#define _INOTIFY_H	1

#include<errno.h>
#include<sys/inotify.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<string.h>
#include<sys/types.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#define ALREXST -1
#define NOTFND -2

#define EVENT_SIZE  (sizeof(struct inotify_event))
#define EVENT_BUF_LEN     (1024 * (EVENT_SIZE + 16))

typedef struct getnotify_thread_args {
	char argssend[1024];	
	char argsrecv[1024];	
	/*
	* argssend is to send string of required notification request to getnotify thread
	* argsrecv  tois fill the notification response by getnotify thread
	*/

} getnotify_thread_args;

void getmask(long long unsigned int *maskval, char *mask);
void getnotify(struct getnotify_thread_args *args);
char* getValFromArgs(char *usage, char* key);
char* extractVal(char *key_val);

#endif				/*inotify.h */
