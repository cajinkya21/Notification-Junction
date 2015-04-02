Notification Junction

/*Copy abstract*/

To run a demo, do the following - 

cd to the directory including NJ code.

In nj.h, choose the datastructure that is needed (HASH, LIST, BOTH) in the preprocessor directive and save.

$make    clean

$make

Now, to register inotify, do

$./np_register    inotify==npname::inotify##dir::~/Desktop##flags::IN_CREATE*IN _DELETE*IN_MODIFY##TYPE::N##count::2

We have a code file which we call “demo_dummyapp.c”, which uses the nj_nonblocklib.so library, to write arguments to functions app_register, app_unregister and app_getnotify, on the socket for the NJ to read and process. Take a look at the arguments given through the library calls and change them if needed.

$./demo_dummyapp    <pathof directory to be monitored>

Below is the analysis of the non-blocking call, which happens by default in the demo_dummyapp.c :


The application carries on with its functionality (prints '*' all over the screen), which signifies that it doesn't have to poll for the notifications, which is the essence of Notification Junction.

We should now make an event occur.

Create a file in the directory to be monitored. 

$touch    <path to directory to be monitored>/<filename>

This is the kind of notification that demo_dummyapp has registered for, from inotify.

Now that the event has occured, NJ has sent a signal to the application, hence giving the control to the signal handler which waits for us to press a key. This has just been done to ensure legibility of the notification amidst the * that was being printed indefinitely.

After a key is entered, the control of the application passes to the main function of the dummyapp in which it prints * again. This signifies that the application has resumed its work, possibly on the notification that just arrived.

Meanwhile, the NJ is alive and ready for the next requests.
