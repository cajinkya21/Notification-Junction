# Test cases for np_unregister

# no arguments
./np_unregister
echo "Success 1"

# existing NP
./np_unregister inotify
echo "Success 2"

# non-existent NP
./np_unregister unregistered_np
echo "Success 3"
