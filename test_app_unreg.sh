# Test cases for app_unregister

# no arguments
./app_unregister
echo "Success 1"

# Unregister app from all NPs
./app_unregister app1
echo "Success 2"

# unregister from inotify
./app_unregister app2::inotify
echo "Success 3"

# give unregistered NP as argument
./app_unregister app3::unregistered_np
echo "Success 4"

# Use : instead of ::
./app_unregister app4:inotify
echo "Success 5"

# Use ## instead of ::
./app_unregister app5##inotify
echo "Success 6"


