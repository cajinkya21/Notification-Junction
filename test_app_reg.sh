# Test cases for app_register command

# No arguments
./app_register 
echo "Success 1"

# Register only app
./app_register app1
echo "Success 2"

# Register app and existing np

./np_register npname::inotify##dir::~/Desktop##flags::IN_CREATE*IN_DELETE*IN_MODIFY##TYPE::N##count::2
./app_register app2::inotify
echo "Success 3"

# Register app with non-existent NP
./app_register app3::unregistered_np
echo "Success 4"

# Use :  instead of ::
./app_register app4:inotify
echo "Success 5"

# Use ## instead of ::
./app_register app5##inotify
echo "Success 6"
