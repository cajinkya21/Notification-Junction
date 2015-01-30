/* The file contains for inotify library functions to work with inotify */

#include"inotify.h"

/* flags for inotify in string format to match with requested paramaters */
char *flags[] =
    { "IN_ACCESS", "IN_ATTRIB", "IN_CLOSE_WRITE", "IN_CLOSE_NOWRITE",
"IN_CREATE", "IN_DELETE", "IN_MODIFY", "IN_DELETE_SELF", "IN_MOVED_FROM",
"IN_MOVED_TO", "IN_OPEN", "IN_DONT_FOLLOW", "IN_EXCL_UNLINK", "IN_MASK_ADD",
"IN_ONESHOT", "IN_ONLYDIR" };

/* flags for intotify in integer format */
long long unsigned int values[] =
    { IN_ACCESS, IN_ATTRIB, IN_CLOSE_WRITE, IN_CLOSE_NOWRITE, IN_CREATE,
IN_DELETE, IN_MODIFY, IN_DELETE_SELF, IN_MOVED_FROM, IN_MOVED_TO, IN_OPEN, IN_DONT_FOLLOW,
IN_EXCL_UNLINK, IN_MASK_ADD, IN_ONESHOT, IN_ONLYDIR };

/* FUNCTION TO GET NOTIFICATION */
void getnotify(struct getnotify_threadArgs *args)
{

	printf("Args received in getnotify - %s!!!!\n", args->argssend);

	int length, i = 0;
	int fd;
	int wd;
	char buffer[EVENT_BUF_LEN];
	char pathname[256];

	char mask[512];
//      strcpy(mask, "IN_CREATE|IN_DELETE|IN_MODIFY");

	long long unsigned int maskval = 0;

	char np_name[64], dir_name[256], flag_set[512], one[512], two[512],
	    three[512];
	char delimattr[3] = "##";
	char delimval[3] = "::";
	char retStr[1024];

	strcpy(one, strtok(args->argssend, delimattr));
	strcpy(two, strtok(NULL, delimattr));
	strcpy(three, strtok(NULL, delimattr));
	printf("ONE : %s\nTWO : %s\nTHREE : %s\n", one, two, three);
	strtok(one, delimval);	/* HERE */
	strcpy(np_name, strtok(NULL, delimval));
	strtok(two, delimval);
	strcpy(dir_name, strtok(NULL, delimval));
	strtok(three, delimval);
	strcpy(flag_set, strtok(NULL, delimval));
	printf("NP NAME : %s\nDIR NAME : %s\nFLAG SET : %s\n", np_name,
	       dir_name, flag_set);

	strcpy(pathname, dir_name);
	strcpy(mask, flag_set);

	getmask(&maskval, mask);
	printf("mask val in main is %llu\n", maskval);

	fd = inotify_init();
	printf("fd:%d\n", fd);

	if (fd < 0) {
		perror("inotify_init");
	}

	if (open(pathname, O_RDONLY) == -1) {
		perror("Error Opening\n");
		exit(0);
	}

	wd = inotify_add_watch(fd, pathname, maskval);

	//while(1) {

	i = 0;
	length = read(fd, buffer, sizeof(buffer));

	if (length < 0) {
		perror("read");
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
	//}

	strcpy(args->argsrecv, retStr);

	inotify_rm_watch(fd, wd);

	close(fd);

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

/* FUNCTION TO CONVERT STRING MASK TO INTEGER */
void getmask(long long unsigned int *maskval, char *mask)
{

	char copy[512];
	strcpy(copy, mask);
	printf("Mask is %s\n", mask);
	int i = 0;
	int index;
	//const char del = '*';
	char p[512];
	char *ptr;
	printf("Mask copy is %s\n", copy);

	printf("PRINTING ALL TOKENS\n");

	ptr = strtok(copy, "*");
	printf("TOKEN - %s\n", ptr);
	while (ptr != NULL) {

		strcpy(p, ptr);

		for (i = 0; i < 16; i++) {
			if (!strcmp(p, flags[i])) {
				index = i;
				printf("INDEX %d\n", index);
				*maskval = *maskval | values[index];
				printf("Value %llu\n", values[index]);
				printf("Maskval = %llu\n", *maskval);
				break;
			}
		}

		printf("TOKEN - %s\n", ptr);
		ptr = strtok(NULL, "*");
	}

/*

	strcpy(p, strtok(copy, &del));
	ptr = p;
	//strcpy(p, strtok(m, &space));
	printf("p is %s.\n", p);

	while(ptr != NULL) {
		
		strcpy(p, ptr);
	
		printf("p is %s.\n", p);

		for(i = 0; i < 16; i++) {
			if(!strcmp(p, flags[i])) {
				index = i;
				printf("INDEX %d\n", index);
				*maskval = *maskval | values[index];
				printf("Value %d\n", values[index]);
				printf("Maskval = %d\n", *maskval);
				break;
			}
		}
		
		ptr = strtok(NULL, &del);
		printf("ptr after strtok %s\n", ptr);
		strcat(ptr, "\0");
		
	}

	*/

}
