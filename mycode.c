#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<errno.h>
#include<sys/types.h>
#include<sys/inotify.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<string.h>
#include"inotify.h"
#include <dlfcn.h>

struct getnotify_threadArgs args;
int main(int argc, char *argv[])
{
	strcpy(args.argssend, argv[1]);
	printf("CALLING :\n");

	void *handle;
	void (*getnotify) (struct getnotify_threadArgs *);
	char *error;

	handle = dlopen("./libinotify.so", RTLD_LAZY);
	if (!handle) {
		perror("chukla");
		exit(1);
	}

	printf("dlopen successful\n");

	getnotify = dlsym(handle, "getnotify");
	if ((error = dlerror()) != NULL) {
		perror("chukla");
		exit(1);
	}

	(*getnotify) (&args);

	// printf ("%f\n", (*cosine)(2.0));
	dlclose(handle);

	//getnotify(&args);
	printf("CALLED :\n");
	printf("ARGSRECV = %s\n", args.argsrecv);
	return 0;
}

/*
#include <dlfcn.h>

    int main(int argc, char **argv) {
        void *handle;
        double (*cosine)(double);
        char *error;

        handle = dlopen ("/lib/libm.so.6", RTLD_LAZY);
        if (!handle) {
            fputs (dlerror(), stderr);
            exit(1);
        }

        cosine = dlsym(handle, "cos");
        if ((error = dlerror()) != NULL)  {
            fputs(error, stderr);
            exit(1);
        }

        printf ("%f\n", (*cosine)(2.0));
        dlclose(handle);
    }

*/
