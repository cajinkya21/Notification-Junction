#ifndef _APPLIB_H
#define _APPLIB_H	1

#include<sys/types.h>
#include<sys/socket.h>
#include<sys/un.h>
#include<sys/stat.h>
#include<pthread.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<signal.h>
#include<errno.h>
#define AppGetnotify "app_getn"
#define AppReg "app_reg"
#define AppUnReg "app_unreg"

int appRegister(char *key_val_string);
int appUnregister(char *key_val_string);
int appGetnotify(int pid, char *key_val_string, char choice);	/*choice is 1 for blocking and 0 for non_blocking...... */

#endif				/*lib.h */
