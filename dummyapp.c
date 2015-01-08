#include "lib.h"
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



	i = app_register("app1::inotify");
	printf("Dummy app app register done\n");
	i = app_getnotify(pid,"npname::inotify##dir::/home/chitra/Desktop/##flags::IN_CREATE*IN_DELETE*IN_MODIFY", 'N' );
	printf("Dummy app getnotify done \n");
	//sigsuspend(&mask);
	while(1) ;
	printf("dummy app sigsuspend recieved\n");

	
	
	app_unregister("app1::inotify");
	return 0;
}
