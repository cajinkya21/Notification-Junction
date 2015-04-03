# Test cases for np_register

# No arguments
./np_register
echo "Success 1"

# Val missing for key
./np_register npname::inotify2##dir::~/Desktop##flags::##TYPE::N##count::
echo "Success 2"

# Meaningless ## string in between (no key vals)
./np_register npname::inotify3################dir::~/Desktop##flags::IN_CREATE*IN_DELETE*IN_MODIFY##TYPE::N##count::2
echo "Success 3"

# Valid case
./np_register npname::inotify4##dir::~/Desktop##flags::IN_CREATE*IN_DELETE*IN_MODIFY##TYPE::N##count::2
echo "Success 4"

# Put no usage
./np_register inotify5
echo "Success 5"

# No npname in beginning
./np_register dir::~/Desktop##flags::IN_CREATE*IN_DELETE*IN_MODIFY##TYPE::N##count::2##npname::inotify6
echo "Success 6"

