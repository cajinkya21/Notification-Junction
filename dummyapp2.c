/* The file contains code for dummy application that registers with NJ, request for notification and then unregisters from NJ using non-block library. */

#include "lib.h"
char str[512];
int times,i;
FILE* pidfd;	
char pidfilename[64];
char filebuffer[128];

/* SIGNAL HANDLER FOR SIGUSR1 SINGAL THAT IS SENT FORM NJ WHEN NOTIFICATION ARRIVES  */
void sigusrhandler(int signum) {

	if (signum == SIGUSR1) {
		times++;
		if(times == 1) {
			if (!(pidfd = fopen(pidfilename, "r+"))) {
				perror("dummyapp2.C   : not able to open  dummyapp file\n");
				return;
			}
		}
		printf("received SIGUSR1 %d \n\n\n",times);
		i = 0;
		fgets(filebuffer, 128,  pidfd );

		printf("Signalhandler Notification is %s \n",filebuffer);
		return;
		//exit(0);			/* Application unregisters with inotify */
	}	

}

int main(int argc, char* argv[]) {
	int i,j;
	int pid;
	times = 0;
	char arr[512],arr2[512];
	struct stat s;
	if(argc < 2) {
		printf("Kindly enter 2  directories\n");
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
			printf("%s is a directory\t Proceeding for NJ \n",argv[1]);
			/* it's a dir */
		} else {
			printf("%s is not a directory \n", argv[1]);
			/* exists but is no dir */
		}
	}
	err = stat(argv[2], &s);
	if(-1 == err) {
		if(ENOENT == errno) {
			printf("%s is not a valid argument Check if it exists\n",argv[2]);
			exit(0);
			/* does not exist */
		} else {
			perror("stat");
			exit(1);
		}
	} 	else {
		if(S_ISDIR(s.st_mode)) {
			printf("%s is a directory \tProceeding for NJ\n",argv[2]);
			/* it's a dir */
		} else {
			printf("%s is not a directory \n", argv[2]);
			/* exists but is no dir */
		}
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
	sprintf(str, "%d", pid );
	strcpy(pidfilename,str);
	strcat(pidfilename, ".txt");
	strcat(str, "::inotify");
	signal(SIGUSR1, sigusrhandler);
	sigset_t mask;
	sigemptyset (&mask);
	sigaddset (&mask, SIGUSR1);


	printf(" Dummy app  signal handler set\n");
	//sigprocmask (SIG_UNBLOCK, &mask, NULL);



	i = appRegister(str);				/* Application registers with inotify */
	printf("Dummy app app register done\n");
	printf("DUMMYAPP : Sending pid = %d\n", pid);
	i = appGetnotify(pid, arr, 'N' );
	printf("return value of getnotify1 : %d\n", i);								/* Application request for notification from inotify */
	j = appGetnotify(pid, arr2,'N');
	printf("return value of getnotify2 : %d\n", j);	


	printf("Dummy app getnotify done \n");
	//sigsuspend(&mask);
	while(1) ;							/* Loop to make application wait for notification */
	printf("dummy app sigsuspend recieved\n");


	return 0;
}
