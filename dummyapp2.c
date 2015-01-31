/* The file contains code for dummy application that registers with NJ, request for notification and then unregisters from NJ using non-block library. */

#include "lib.h"
char str[512];

/* SIGNAL HANDLER FOR SIGUSR1 SINGAL THAT IS SENT FORM NJ WHEN NOTIFICATION ARRIVES  */
void sigusrhandler(int signum) {
	if (signum == SIGUSR1) {
		printf("received SIGUSR1\n\n\n");
	    app_unregister(str);	
	    exit(0);			/* Application unregisters with inotify */
	}	
	
}

int main(int argc, char* argv[]) {
	int i,j;
	int pid;
	char arr[512],arr2[512];

	if(argc < 2) {
	    printf("Kindly enter the directory\n");
	    exit(0);
	}
	
	strcpy(arr, "npname::inotify##dir::");
	strcat(arr, argv[1]);   // The directory to be watched.
	strcat(arr, "##flags::IN_CREATE*IN_DELETE*IN_MODIFY##count::2");
	printf("Arr is %s\n", arr);
	strcpy(arr2, "npname::inotify##dir::");
	strcat(arr2, argv[2]);   // The directory to be watched.
	strcat(arr2, "##flags::IN_CREATE*IN_DELETE*IN_MODIFY##count::2");
	printf("Arr2 is %s\n", arr2);

	pid = getpid();
	signal(SIGUSR1, sigusrhandler);
	sigset_t mask;
	sigemptyset (&mask);
	sigaddset (&mask, SIGUSR1);
	

	printf(" Dummy app  signal handler set\n");
	//sigprocmask (SIG_UNBLOCK, &mask, NULL);

	sprintf(str, "%d", pid );
	strcat(str, "::inotify");

	i = app_register(str);				/* Application registers with inotify */
	printf("Dummy app app register done\n");
	printf("DUMMYAPP : Sending pid = %d\n", pid);
	i = app_getnotify(pid, arr, 'N' );
	printf("return value of getnotify1 : %d\n", i);								/* Application request for notification from inotify */
    	j = app_getnotify(pid, arr2,'N');
    	printf("return value of getnotify2 : %d\n", j);	
    	
    	
	printf("Dummy app getnotify done \n");
	//sigsuspend(&mask);
	while(1) ;							/* Loop to make application wait for notification */
	printf("dummy app sigsuspend recieved\n");

	
	return 0;
}
