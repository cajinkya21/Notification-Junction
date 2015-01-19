#ifndef _APPLIB_H
#define _APPLIB_H	1

#include<sys/types.h>
#include<sys/socket.h>
#include<sys/un.h>
#include<pthread.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<signal.h>

#define AppGetnotify "app_getn"
#define AppReg "app_reg"
#define AppUnReg "app_unreg"

int app_register(char *key_val_string);
int app_unregister(char *key_val_string);
int app_getnotify(int pid, char *key_val_string, char choice);	/*choice is 1 for blocking and 0 for non_blocking...... */

#endif				/*lib.h */
