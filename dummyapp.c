#include "lib.h"


int main() {
	int i;
	int pid;
	pid = getpid();
	i = app_register("app1::inotify");
	
	i = app_getnotify(pid,"npname::inotify##dir::/home/chitra/Desktop/##flags::IN_CREATE*IN_DELETE*IN_MODIFY", 'N' );
	
	
	
	app_unregister("app1::inotify");
	return 0;
}
