./app_register 
echo "Success 1"

./app_register app1
echo "Success 2"

./app_register app2::inotify
echo "Success 3"

./app_register app3::unregistered_np
echo "Success 4"

./app_register app4:inotify
echo "Success 5"

./app_register app5##inotify
echo "Success 6"
