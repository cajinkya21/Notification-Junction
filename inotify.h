#ifndef ERRNO_H
#define ERRNO_H
#include<errno.h>
#endif

#ifndef SYSINOTIFY_H
#define SYSINOTIFY_H
#include<sys/inotify.h>
#endif

#ifndef SYSSTAT_H
#define SYSSTAT_H
#include<sys/stat.h>
#endif


#ifndef FCNTL_H
#define FCNTL_H
#include<fcntl.h>
#endif

#ifndef STRING_H
#define STRING_H
#include<string.h>
#endif

#ifndef SYSTYPES_H
#define SYSTYPES_H
#include<sys/types.h>
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

#ifndef ALREXST
#define ALREXST -1
#endif

#ifndef NOTFND
#define NOTFND -2
#endif

#define EVENT_SIZE  (sizeof(struct inotify_event))
#define EVENT_BUF_LEN     (1024 * (EVENT_SIZE + 16))

typedef struct getnotify_threadArgs {
		char argssend[1024];//getnotify thread will receive string of required notification request from this field 
		char argsrecv[1024];//getnotify of np will fill the notification response in this field
}getnotify_threadArgs;


void getmask(long long unsigned int *, char *);
void getnotify(struct getnotify_threadArgs*);

