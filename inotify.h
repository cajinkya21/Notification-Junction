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

typedef struct getnotify_threadArgs {
	char argssend[1024];	//getnotify thread will receive string of required notification request from this field 
	char argsrecv[1024];	//getnotify of np will fill the notification response in this field
} getnotify_threadArgs;

void getmask(long long unsigned int *maskval, char *mask);
void getnotify(struct getnotify_threadArgs *args);

#endif				/*inotify.h */
