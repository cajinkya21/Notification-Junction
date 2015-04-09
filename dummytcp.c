/* The file contains code for dummy application that registers with NJ, request for notification and then unregisters from NJ using non-block library. */

#include "lib.h"
char str[512];
int times,i;
FILE* pidfd;
int fd;	
char pidfilename[64];
char filebuffer[128];
	
/* SIGNAL HANDLER FOR SIGUSR1 SINGAL THAT IS SENT FORM NJ WHEN NOTIFICATION ARRIVES  */
void sigusrhandler(int signum)
{
	char ch;
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
		printf("Signalhandler Notification is - ");
		while(fgets(filebuffer, 1024, pidfd)) {
		printf(" %s \n",filebuffer);
		}
		//scanf("%c", &ch);
		return;
	    //exit(0);		
    }

}

int main(int argc, char *argv[])
{
	struct stat s;
	int pid;
	char arr[512];
		times = 0;
	if (argc < 2) {
		printf("Kindly enter the directory\n");
		exit(0);
	}
	strcpy(arr, "npname::tcpdump##devicename::");
	strcat(arr, argv[1]);	// The directory to be watched.
	strcat(arr, "##num_packets::");
	strcat(arr, argv[2]);
	strcat(arr, "##count::1");
	printf("Arr is %s\n", arr);
	pid = getpid();
	sprintf(str, "%d", pid );
	strcpy(pidfilename,str);
	strcat(pidfilename, ".txt");
	signal(SIGUSR1, sigusrhandler);
	sigset_t mask;
	sigemptyset(&mask);
	sigaddset(&mask, SIGUSR1);

	printf(" Dummy app  signal handler set\n");
	//sigprocmask (SIG_UNBLOCK, &mask, NULL);

	sprintf(str, "%d", pid);
	strcat(str, "::tcpdump");

	appRegister(str);	/* Application registers with inotify */
	printf("Dummy app app register done\n");
	printf("DUMMYAPP : Sending pid = %d\n", pid);
	appGetnotify(pid, arr, 'B');
	/* Application request for notification from inotify */

	printf("Dummy app getnotify done \n");
	//sigsuspend(&mask);
	while (1);
		//printf("*");	/* Loop to make application wait for notification */
	printf("dummy app sigsuspend recieved\n");

	
	return 0;
}
