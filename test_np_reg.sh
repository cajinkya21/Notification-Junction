#normal case, should pass
./np_register inotify5==npname::inotify5##dir::~/Desktop##flags::##TYPE::N##count::


./np_register inotify1==npname::inotify1################dir::~/Desktop##flags::IN_CREATE*IN_DELETE*IN_MODIFY##TYPE::N##count::2
echo "Success 1"

#Put @@ instead of ##
./np_register inotify2==npname::inotify2##dir::~/Desktop##flags::IN_CREATE*IN_DELETE*IN_MODIFY##TYPE::N##count::2
echo "Success 2"

#Put no arguments
./np_register inotify3
echo "Success 3"

./np_register inotify4==npname::inotify4##dir::~/Desktop##flags::IN_CREATE*IN_DELETE*IN_MODIFY##TYPE::N##count::2
