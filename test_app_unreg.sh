./app_unregister
echo "Success 1"

./app_unregister app1
echo "Success 2"

./app_unregister app2::inotify
echo "Success 3"

./app_unregister app3::unregistered_np
echo "Success 4"

./app_unregister app4:inotify
echo "Success 5"

./app_unregister app5##inotify
echo "Success 6"


