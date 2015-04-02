/*  
    Notification-Junction is an interface between multiple applications and multiple Notification Providers.
    Copyright (C) 2015      Navroop Kaur<watwanichitra@gmail.com>, 
    Shivani Marathe<maratheshivani94@gmail.com>, 
    Kaveri Sangale<sangale.kaveri9@gmail.com>
    All Rights Reserved.

    This program is free software; you can redistribute it and/or modify it under the terms of the 
    GNU General Public License as published by the Free Software Foundation; either version 3 of the
    License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
    without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  
    See the GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along with this program; if not, write to the 
    Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

/*This is header file for nj.c (notification juncitoncode)*/
#ifndef _NJ_H
#define _NJ_H	1

/*Macro*/
#define PRINTF printf 

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
#include<errno.h>
#define ALREXST -1
#define NOTFND -2
#define ERROR -1
#define SUCCESS 1
#include "app_dcll.h"
#include "np_dcll.h"
#define NONBLOCKING 'N'
#define BLOCKING 'B'
#define LOGS "logs"
#define PIDFILE "File_PIDS.txt"
/* Create 2 sockets - app_reg, app_unreg*/
/* Fork 2 threads - app_reg, app_unreg*/
#define AppGetnotifySocket "app_getn_sock"
#define AppRegSocket "app_reg_sock"
#define AppUnRegSocket "app_unreg_sock"
#define NpRegSocket "np_reg_sock"
#define NpUnRegSocket "np_unreg_sock"
#define StatSocket "statsock"
#define StatSocketPrint "statsockprint"
#define APPLIMIT 32
#define QLEN 32
#define HASH 1
#define LIST 2
#define DATASTRUCT HASH
/*Structure for arguments to be passed to thread */
typedef struct thread_args {
	int sock;		/*socket() call return value stored*/
	int msgsock;		/*accept() return value stored*/
	int rval;		/*read() return value stored*/
	struct sockaddr_un server;
	char buf[1024];		/* buffer used for socket reading*/

} thread_args;

typedef struct proceed_getn_thread_args	 {
	char buf[1024];		/*buf is to send key_val pair argument to proceed_getnotify_method thread */
} proceed_getn_thread_args;

/*Structure for arguments to be passed to getnotify thread*/
typedef struct getnotify_thread_args {
	char argssend[1024];	/* argssend is to send string of required notification request to getnotify thread*/
	char argsrecv[1024];	/*argsrecv  tois fill the notification response by getnotify thread*/
} getnotify_thread_args;

/* function returns negative value if error in registering app This function will add node in the app list  for first registration of app with particular NP*/
int register_app(char *buff);

/*function returns negative value if error in registering np this function will add node in the np list for np registration of np */
int register_np(char *buff);

/* function returns negative value if error in unregistering app eg(app not found in list)*/
int unregister_app(char *buff);

/*function returns negative valuse if error in unregistering np eg(np not found in list)*/
int unregister_np(char *buff);

/*function to get notification */
char *getnotify_app(char *buff);

/*function to force writing of logs to skip delayed write*/
void force_logs();

/*thread method  to read socket for stats calls for stats thread*/
void *print_stat_method(void *arguments);

/*thread method for app_registration thread*/
void *app_reg_method(void *arguments);

/*thread method for app_unregistration thread*/
void *app_unreg_method(void *arguments);

/*thread method for np_registration thread*/
void *np_reg_method(void *arguments);

/*thread method for np_unregistration thread*/
void *np_unreg_method(void *arguments);

/*thread method for app_getnotify thread*/
void *app_getnotify_method(void *arguments);

/*thread method for np_getnotify_method which opens the NP library and calls specific function of NP*/
void *np_getnotify_method(void *arguments);

/*thread method for nonblocking getnotify calls */
void *proceed_getnotify_method(void *arguments);

/*thread method for blocking getnotify calls*/
void *block_getnotify_method(void *arguments);

/*function that decriments the npList node count for the app specified by appList*/
void dec_all_np_counts(app_dcll * appList, np_dcll * npList, char *app_name);

/*functin prints statistics on screen */
void print_stat();

/*Handler for SIG_INT */
void sigint_handler(int signum);

/*prints np_list on socket for stats */
void print_list_on_sock_np(np_dcll *l);

/*prints app_list on socket for stats */
void print_list_on_sock_app(app_dcll *l);


void print_np_key_val_stat(app_node * temp);

/*prints np hash table on socket for stats */
void print_hash_on_sock_np(hash_struct_np *hstruct_np);

/*prints app hash table on socket for stats */
void print_hash_on_sock_app(hash_struct_app *hstruct_app);

/*It extracts an array of key::val strings from usage and stores it in array keyVal*/
void extract_key_val(char *usage, char ***keyVal);

/*extracts key from key::val pair*/
char* extract_key(char *key_val);

/*extracts val from key::val pair*/ 
char* extract_val(char *key_val);

/*compares the two arrays of strings to check whether all keys specifed in getnotify call are valid keys for np*/
int compare_array(char *** np_key_val_arr, char *** getn_key_val_arr);

/*converts the getnotify arguments to the way in which np specific library wants , also adds the missing keys with default values */
void forward_convert(char ***np_key_val_arr,char ***getn_key_val_arr, char * fillit);

/*gets value for key from the usage string */
char *get_val_from_args(char *usage, char* key);

/*gets pid.txt filename from usage string*/
char *get_filename(char *argsbuf);

/*counts number of occurences of count_occurence_of in the string usage*/
int count_args(char *usage, char *count_occurence_of);

/*It is called by exit handler when nj exits*/
void nj_exit(void); 

/*It decrements all np counts in hash table with which app_name was registered */ 
void dec_all_np_counts_hash(hash_struct_app *hstruct_app, hash_struct_np *hstruct_np, char *app_name);



#endif				/*nj.h */
