/* The file contains code for dummy application that registers with NJ, request for notification and then unregisters from NJ using non-block library. */

#include "lib.h"
	char str[512];
	
/* SIGNAL HANDLER FOR SIGUSR1 SINGAL THAT IS SENT FORM NJ WHEN NOTIFICATION ARRIVES  */
void sigusrhandler(int signum)
{
	if (signum == SIGUSR1) {
		printf("received SIGUSR1\n\n\n");
		app_unregister(str);	/* Application unregisters with inotify */
			exit(0);
    }

}

int main(int argc, char *argv[])
{
	struct stat s;
	int pid;
	char arr[512];
	
	if (argc < 2) {
		printf("Kindly enter the directory\n");
		exit(0);
	}
	int err = stat(argv[1], &s);
	if(-1 == err) {
    		if(ENOENT == errno) {
			printf("%s is not a valid argument Check if it exists\n",argv[1]);
			exit(0);
        		/* does not exist */
    		} else {
        		perror("stat");
        		exit(1);
    		}
} 	else {
    		if(S_ISDIR(s.st_mode)) {
			printf("%s is a directory \n",argv[1]);
       			 /* it's a dir */
    		} else {
			printf("%s is not a directory \n", argv[1]);
        		/* exists but is no dir */
    		}
	}
	strcpy(arr, "npname::inotify##dir::");
	strcat(arr, argv[1]);	// The directory to be watched.
	strcat(arr, "##flags::IN_CREATE*IN_DELETE*IN_MODIFY##count::2");
	printf("Arr is %s\n", arr);
	pid = getpid();
	signal(SIGUSR1, sigusrhandler);
	sigset_t mask;
	sigemptyset(&mask);
	sigaddset(&mask, SIGUSR1);

	printf(" Dummy app  signal handler set\n");
	//sigprocmask (SIG_UNBLOCK, &mask, NULL);

	sprintf(str, "%d", pid);
	strcat(str, "::inotify");

	app_register(str);	/* Application registers with inotify */
	printf("Dummy app app register done\n");
	printf("DUMMYAPP : Sending pid = %d\n", pid);
	app_getnotify(pid, arr, 'N');
	/* Application request for notification from inotify */

	printf("Dummy app getnotify done \n");
	//sigsuspend(&mask);
	while (1) ;		/* Loop to make application wait for notification */
	printf("dummy app sigsuspend recieved\n");

	
	return 0;
}
