/* The file contains for inotify library functions to work with inotify */

#include"inotify.h"

/* flags for inotify in string format to match with requested paramaters */

char *flags[] =    {    "IN_ACCESS", 
	"IN_ATTRIB", 
	"IN_CLOSE_WRITE", 
	"IN_CLOSE_NOWRITE",
	"IN_CREATE", 
	"IN_DELETE", 
	"IN_MODIFY", 
	"IN_DELETE_SELF", 
	"IN_MOVED_FROM",
	"IN_MOVED_TO", 
	"IN_OPEN", 
	"IN_DONT_FOLLOW", 
	"IN_EXCL_UNLINK", 
	"IN_MASK_ADD",
	"IN_ONESHOT", 
	"IN_ONLYDIR" };

/* flags for intotify in integer format */
long long unsigned int values[] =    {
	IN_ACCESS, 
	IN_ATTRIB, 
	IN_CLOSE_WRITE, 
	IN_CLOSE_NOWRITE, 
	IN_CREATE,
	IN_DELETE, 
	IN_MODIFY, 
	IN_DELETE_SELF, 
	IN_MOVED_FROM, 
	IN_MOVED_TO, 
	IN_OPEN, 
	IN_DONT_FOLLOW,
	IN_EXCL_UNLINK, 
	IN_MASK_ADD, 
	IN_ONESHOT, 
	IN_ONLYDIR };

/* FUNCTION TO GET NOTIFICATION */
void getnotify(struct getnotify_thread_args *args)
{



	int length, i = 0, fd, wd;
	long long unsigned int maskval = 0;

	char buffer[EVENT_BUF_LEN], pathname[256], mask[512];
	char retStr[1024];
	char *expt2, *expt3;

	expt2 = getValFromArgs(args->argssend, "dir");
	expt3 = getValFromArgs(args->argssend, "flags");

	printf("> %s %d getnotify() : Args received in getnotify - %s.\n", __FILE__, __LINE__, args->argssend);

	strcpy(pathname, extractVal(expt2));
	strcpy(mask, extractVal(expt3));

	printf("> %s %d getnotify() : pathname - %s. and mask is %s.\n", __FILE__, __LINE__, pathname, mask);

	getmask(&maskval, mask);
	printf("> %s %d getnotify() : mask value is %llu.\n", __FILE__, __LINE__, maskval);

	fd = inotify_init();
	printf("> %s %d getnotify() : fd is %d.\n", __FILE__, __LINE__, fd);


	if (fd < 0) {
		perror("inotify_init");
	}

	if (open(pathname, O_RDONLY) == -1) {
		perror("Error Opening\n");
		exit(0);
	}

	wd = inotify_add_watch(fd, pathname, maskval);

	i = 0;
	length = read(fd, buffer, sizeof(buffer));

	if (length < 0) {
		perror("read");
		return;
	}



	while (i < length) {

		struct inotify_event *event =
			(struct inotify_event *)&buffer[i];

		if (event->len) {

			if (event->mask & IN_CREATE) {
				if (event->mask & IN_ISDIR) {
					printf
						("NOTIFICATION : NEW DIRECTORY %s CREATED.\n",
						 event->name);
					strcpy(retStr,
							"NOTIFICATION : NEW DIRECTORY ");
					strcat(retStr, event->name);
					strcat(retStr, " CREATED.");
				} else {
					printf("New file %s created.\n",
							event->name);
					strcpy(retStr,
							"NOTIFICATION : NEW FILE ");
					strcat(retStr, event->name);
					strcat(retStr, " CREATED.");
				}
			} else if (event->mask & IN_DELETE) {
				if (event->mask & IN_ISDIR) {
					printf("Directory %s deleted.\n",
							event->name);
					strcpy(retStr,
							"NOTIFICATION : DIRECTORY ");
					strcat(retStr, event->name);
					strcat(retStr, " DELETED.");
				} else {
					printf("File %s deleted.\n",
							event->name);
					strcpy(retStr, "NOTIFICATION : FILE ");
					strcat(retStr, event->name);
					strcat(retStr, " DELETED.");
				}
			}

			else if (event->mask & IN_MODIFY) {
				if (event->mask & IN_ISDIR) {
					printf("Directory %s modified.\n",
							event->name);
					strcpy(retStr,
							"NOTIFICATION : DIRECTORY ");
					strcat(retStr, event->name);
					strcat(retStr, " MODIFIED.");
				} else {
					printf("File %s modified.\n",
							event->name);
					strcpy(retStr, "NOTIFICATION : FILE ");
					strcat(retStr, event->name);
					strcat(retStr, " MODIFIED.");
				}
			}

			else if (event->mask & IN_ACCESS) {
				if (event->mask & IN_ISDIR) {
					printf("Directory %s accessed.\n",
							event->name);
					strcpy(retStr,
							"NOTIFICATION : DIRECTORY ");
					strcat(retStr, event->name);
					strcat(retStr, " ACCESSED.");
				} else {
					printf("File %s accessed.\n",
							event->name);
					strcpy(retStr, "NOTIFICATION : FILE ");
					strcat(retStr, event->name);
					strcat(retStr, " ACCESSED.");
				}
			}

			else if (event->mask & IN_CLOSE) {
				printf("File %s was closed.\n", event->name);
				strcpy(retStr, "NOTIFICATION : FILE ");
				strcat(retStr, event->name);
				strcat(retStr, " CLOSED.");
			}

			else if (event->mask & IN_MOVE) {
				if (event->mask & IN_ISDIR) {
					printf("Directory %s moved.\n",
							event->name);
					strcpy(retStr,
							"NOTIFICATION : DIRECTORY ");
					strcat(retStr, event->name);
					strcat(retStr, " MOVED.");
				} else {
					printf("File %s moved.\n", event->name);
					strcpy(retStr, "NOTIFICATION : FILE ");
					strcat(retStr, event->name);
					strcat(retStr, " MOVED.");
				}
			}

			else if (event->mask & IN_OPEN) {
				if (event->mask & IN_ISDIR) {
					printf("Directory %s opened.\n",
							event->name);
					strcpy(retStr,
							"NOTIFICATION : DIRECTORY ");
					strcat(retStr, event->name);
					strcat(retStr, " OPENED.");
				} else {
					printf("File %s opened.\n",
							event->name);
					strcpy(retStr, "NOTIFICATION : FILE ");
					strcat(retStr, event->name);
					strcat(retStr, " OPENED.");
				}
			}

			i += EVENT_SIZE + event->len;

		}
	}


	strcpy(args->argsrecv, retStr);

	inotify_rm_watch(fd, wd);

	close(fd);

}

/* FUNCTION TO CONVERT STRING MASK TO INTEGER */
void getmask(long long unsigned int *maskval, char *mask)
{

	char copy[512];
	char p[512];
	char *ptr;
	int i = 0;
	int index;

	strcpy(copy, mask);

	ptr = strtok(copy, "*");

	while (ptr != NULL) {
		strcpy(p, ptr);

		for (i = 0; i < 16; i++) {

			if (!strcmp(p, flags[i])) {
				index = i;
				*maskval = *maskval | values[index];
				break;
			}

		}

		ptr = strtok(NULL, "*");
	}

}

char* extractVal(char *key_val) {

	char *ptr, *val;
	char temp[128] ;

	strcpy(temp, key_val);
	ptr = strtok(temp, "::");
	ptr = (key_val + strlen(ptr) + 2);
	if(!ptr) printf("> %s %d extractVal() : Returned NULL\n", __FILE__, __LINE__);
	val = (char *)malloc(sizeof(char) * (strlen(ptr) + 1));
	strcpy(val, ptr);
	return val;
}

char* getValFromArgs(char *usage, char* key) {   
	char *occ, keycopy[128], *retstr, localkeyval[256];
	int i = 0;

	strcpy(keycopy, key);
	strcat(keycopy, "::");
	occ = strstr(usage, keycopy);
	while(*occ != '#') {
		localkeyval[i++] = *occ;
		occ++;    
	}
	localkeyval[i] = '\0';

	retstr = (char *)malloc(sizeof(char) * (i+1));

	strcpy(retstr, localkeyval);
	return retstr;
}

/*
   int main(int argc, char *argv[]) {

   int length, i = 0;
   int fd;
   int wd;
   char buffer[EVENT_BUF_LEN];
   char *pathname = "/home/shivsam/Desktop/";

   char *mask = "IN_CREATE|IN_DELETE|IN_MODIFY";

   long long unsigned int maskval = 0;

   getmask(&maskval, mask);
   printf("mask val in main is %d\n", maskval);

   fd = inotify_init();
   printf("fd:%d\n", fd);

   if (fd < 0) {
   perror("inotify_init");
   }

   if(open(pathname, O_RDONLY) == -1)
   {
   perror("Error Opening\n"); 
   exit(0);	
   }

   wd = inotify_add_watch(fd, pathname, maskval);



   while(1) {

   i = 0;
   length = read(fd, buffer, sizeof(buffer)); 

   if (length < 0) {
   perror("read");
   } 

   while (i < length) {

   struct inotify_event *event = (struct inotify_event *) &buffer[i];

   if (event->len) {

   if (event->mask & IN_CREATE) {
   if (event->mask & IN_ISDIR) {
   printf("New directory %s created.\n", event->name);
   }
   else {
   printf("New file %s created.\n", event->name);
   }
   }
   else if (event->mask & IN_DELETE) {
   if (event->mask & IN_ISDIR) {
   printf("Directory %s deleted.\n", event->name);
   }
   else {
   printf("File %s deleted.\n", event->name);
   }
   }

   else if (event->mask & IN_MODIFY) {
   if (event->mask & IN_ISDIR) {
   printf("Directory %s modified.\n", event->name);
   }
   else {
   printf("File %s modified.\n", event->name);
}
}

else if (event->mask & IN_ACCESS) {
	if (event->mask & IN_ISDIR) {
		printf("Directory %s accessed.\n", event->name);
	}
	else {
		printf("File %s accessed.\n", event->name);
	}
}

else if (event->mask & IN_CLOSE) {
	printf("File %s was closed.\n", event->name);
}

else if (event->mask & IN_MOVE) {
	if (event->mask & IN_ISDIR) {
		printf("Directory %s moved.\n", event->name);
	}
	else {
		printf("File %s moved.\n", event->name);
	}
}

else if (event->mask & IN_OPEN) {
	if (event->mask & IN_ISDIR) {
		printf("Directory %s opened.\n", event->name);
	}
	else {
		printf("File %s opened.\n", event->name);
	}
}

i += EVENT_SIZE + event->len;

}
}
}


inotify_rm_watch(fd, wd);

close(fd);
return 0;
}
*/

