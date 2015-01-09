/* The file contains code for dummy application that registers with NJ, request for notification and then unregisters from NJ using non-block library. */

#include "lib.h"

/* SIGNAL HANDLER FOR SIGUSR1 SINGAL THAT IS SENT FORM NJ WHEN NOTIFICATION ARRIVES  */
void sigusrhandler(int signum) {
	if (signum == SIGUSR1)
		printf("received SIGUSR1\n\n\n");
		exit(1);
}

int main() {
	int i;
	int pid;
	pid = getpid();
	signal(SIGUSR1, sigusrhandler);
	sigset_t mask, oldmask;
	sigemptyset (&mask);
	sigaddset (&mask, SIGUSR1);
	
	printf(" Dummy app  signal handler set\n");
	//sigprocmask (SIG_UNBLOCK, &mask, NULL);

	i = app_register("app1::inotify");				/* Application registers with inotify */
	printf("Dummy app app register done\n");
	i = app_getnotify(pid,"npname::inotify##dir::/home/student/Desktop/##flags::IN_CREATE*IN_DELETE*IN_MODIFY", 'N' );
									/* Application request for notification from inotify */

	printf("Dummy app getnotify done \n");
	//sigsuspend(&mask);
	while(1) ;							/* Loop to make application wait for notification */
	printf("dummy app sigsuspend recieved\n");

	app_unregister("app1::inotify");				/* Application unregisters with inotify */
	return 0;
}
