#!/usr/bin/env python
import os
import pygtk
import gtk
import ctypes
import signal
import sys
import time
from ctypes.util import find_library
pygtk.require('2.0')

window = gtk.Window(gtk.WINDOW_TOPLEVEL)

app_reg_arg = ""
get_notify_arg = "npname::inotify##"
flags = "flags::"
flag_check = 0
pid = os.getpid()
pidstr = str(pid)
app_reg_arg = app_reg_arg + pidstr + "::inotify"
i = 0
pidfilename = ""
arglist = []
times = 0
pidfd = 0


def sigusrhandler(signum, stack):
    print 'Received:', signum
    if signum == signal.SIGUSR1:
    	print "SIGUSR1"
    	global times 
    	global pidfilename
    	global pidfd
    	
	#md = gtk.MessageDialog(type=gtk.MESSAGE_ERROR, buttons=gtk.BUTTONS_OK)  
    	times = times + 1;
    	filebuff = ""
    	if (times == 1): #open file for first instance of Notification 
    		pidfd = open(pidfilename, "r+")
    		print "file opened for first time"
    		
    	filebuff = pidfd.readline()
    	print "SignalHandler Notification is " + filebuff
    	#md.set_markup("Notification - " + filebuff)
        #md.run()	
        shownotif(filebuff)
    	print times;
    else:
    	print "someone else"
    	
    	
    return;
    
def shownotif(filebuff):
    message = gtk.MessageDialog(type=gtk.MESSAGE_INFO, buttons=gtk.BUTTONS_OK)
    message.set_markup("Notification Received : " + filebuff)
    response = message.run()
    if response == gtk.BUTTONS_OK:
	message.destroy()
class CheckButton(object):
    # Our callback.
    # The data passed to this method is printed to stdout
    
    
    def callback(self, widget, data=None):
    	global window

        global flags
        global get_notify_arg
        print "%s was toggled %s" % (data, ("OFF", "ON")[widget.get_active()])
        flags = flags + data + "##"
        get_notify_arg = get_notify_arg + flags
        print "flags - " + flags
        print "get notify - " + get_notify_arg
       
	#gtk_widget_set_sensitive(self,FALSE);
	#gtk_widget_set_sensitive(self, FALSE);
	#self.gtk_widget_hide();
    # This callback quits the program
    	
    	
    def delete_event(self, widget, event, data=None):
        gtk.main_quit()
        return False

    def __init__(self):
        global flag_check
        global window

        # Create a vertical box
        vbox = gtk.VBox(True, 2)

        # Put the vbox in the main window
        window.add(vbox)
        fixed.put(vbox, 700, 10)
        # Create first button
        button = gtk.CheckButton("IN__CREATE             ")

        # When the button is toggled, we call the "callback" method
        # with a pointer to "button" as its argument
        button.connect("toggled", self.callback, "IN_CREATE")


        # Insert button 1
        vbox.pack_start(button, True, True, 2)

        button.show()

        # Create second button

        button = gtk.CheckButton("IN__DELETE   ")

        # When the button is toggled, we call the "callback" method
        # with a pointer to "button 2" as its argument
        button.connect("toggled", self.callback, "IN_DELETE")
        # Insert button 2
        vbox.pack_start(button, True, True, 2)

        button.show()
        
        button = gtk.CheckButton("BOTH   ")

        # When the button is toggled, we call the "callback" method
        # with a pointer to "button 2" as its argument
        
        if flag_check == 1:
        	vbox.hide()
        
        button.connect("toggled", self.callback, "IN_CREATE*IN_DELETE")
        # Insert button 2
        vbox.pack_start(button, True, True, 2)

        button.show()
        
        vbox.show()

class FileSelectionExample:
    # Get the selected filename and print it to the console
    def file_ok_sel(self, w):
        print "%s" % self.filew.get_filename()
        global get_notify_arg
        global window

        get_notify_arg = get_notify_arg + "dir::" + self.filew.get_filename() + "##"
        print "get notify - " + get_notify_arg
        self.filew.destroy()
        return 0
        
    def destroy(self, widget):
        gtk.main_quit()

    def __init__(self):
        # Create a new file selection widget
        self.filew = gtk.FileSelection("File selection")

        # Connect the ok_button to file_ok_sel method
        self.filew.ok_button.connect("clicked", self.file_ok_sel)
    
        # Connect the cancel_button to destroy the widget
        self.filew.cancel_button.connect("clicked",
                                         lambda w: self.filew.destroy())
    
        # Lets set the filename, as if this were a save dialog,
        # and we are giving a default filename
        self.filew.set_filename("")
    
        self.filew.show()
        

class EntryExample(object):

    def enter_callback(self, widget, entry):
        global get_notify_arg
        global window

        entry_text = entry.get_text()
        print "Entry contents: %s\n" % entry_text
        get_notify_arg = get_notify_arg + "count::" + entry_text + "##"
        print "get notify - " + get_notify_arg
        return 0

    def entry_toggle_editable(self, checkbutton, entry):
        entry.set_editable(checkbutton.get_active())

    def entry_toggle_visibility(self, checkbutton, entry):
        entry.set_visibility(checkbutton.get_active())

    def __init__(self):
        # create a new window
        global window

        vbox = gtk.VBox(False, 0)
        window.add(vbox)
        fixed.put(vbox, 200, 10)
        vbox.show()

        entry = gtk.Entry()
        entry.set_max_length(50)
        entry.connect("activate", self.enter_callback, entry)
        #entry.set_text("hello")
        entry.insert_text("Enter count here", len(entry.get_text()))
        entry.select_region(0, len(entry.get_text()))
        vbox.pack_start(entry, True, True, 0)
        entry.show()

        hbox = gtk.HBox(False, 0)
        vbox.add(hbox)
        hbox.show()
                                  
        check = gtk.CheckButton("Editable")
        hbox.pack_start(check, True, True, 0)
        check.connect("toggled", self.entry_toggle_editable, entry)
        check.set_active(True)
        check.show()
    
        check = gtk.CheckButton("Visible")
        hbox.pack_start(check, True, True, 0)
        check.connect("toggled", self.entry_toggle_visibility, entry)
        check.set_active(True)
        check.show()
        
       
class RadioButtons(object):
    def callback(self, widget, data=None):
        print "%s was toggled %s" % (data, ("OFF", "ON")[widget.get_active()])
        return 0
    
    def __init__(self):
	global window

        box1 = gtk.VBox(False, 0)
        window.add(box1)
        fixed.put(box1, 10, 10)
        box1.show()

        box2 = gtk.VBox(False, 10)
        box2.set_border_width(10)
        box1.pack_start(box2, True, True, 0)
        box2.show()

        button = gtk.RadioButton(None, "inotify")
        button.connect("toggled", self.callback, "inotify")
        button.set_active(True)
        box2.pack_start(button, True, True, 0)
        button.show()

        button = gtk.RadioButton(button, "NP 2")
        button.connect("toggled", self.callback, "NP 2")
        box2.pack_start(button, True, True, 0)
        button.show()

        button = gtk.RadioButton(button, "NP 3")
        button.connect("toggled", self.callback, "NP 3")
        box2.pack_start(button, True, True, 0)
        button.show()

        separator = gtk.HSeparator()
        box1.pack_start(separator, False, True, 0)
        separator.show()

        box2 = gtk.VBox(False, 10)
        box2.set_border_width(10)
        box1.pack_start(box2, False, True, 0)
        box2.show()

        #self.window.show()

class Buttons(object):
    # Our usual callback method
    def callback(self, widget, data=None):
        FileSelectionExample()

    def __init__(self):
	global window

        # Create a new button
        button = gtk.Button(label="Select the directory to be monitored")

        # Connect the "clicked" signal of the button to our callback
        button.connect("clicked", self.callback, "Browse")

        # Pack and show all our widgets
    
        box1 = gtk.VBox(False, 0)
        window.add(box1)
        fixed.put(box1, 400, 10)
        box1.show()
        box1.pack_start(button, False, True, 0)

        box1.show()
        button.show()

class Submit(object):
    # Our usual callback method
    def callback(self, widget, data=None):
        print "app reg arg - " + app_reg_arg
        print "Getnotify arg in main - end - " + get_notify_arg
        global flag_check
        flag_check = 99
        main()

    def __init__(self):
	global window

        # Create a new button
        button = gtk.Button(label="Submit")

        # Connect the "clicked" signal of the button to our callback
        button.connect("clicked", self.callback, "Submit")

        # Pack and show all our widgets
    
        box1 = gtk.VBox(False, 0)
        window.add(box1)
        fixed.put(box1, 500, 100)
        box1.show()
        box1.pack_start(button, False, True, 0)

        box1.show()
        button.show()
    


def main():
  
    print "Inside main"
    #signal.signal(signal.SIGUSR1, sigusrhandler)
   
    global flag_check
   
    if flag_check == 99 :
        #write function calls
            print "app reg arg - " + app_reg_arg
            print "Getnotify arg in main - end - " + get_notify_arg
	    libra.appRegister(app_reg_arg)
	    print "Called app register"
	    get_notify_arg_new = get_notify_arg[0:-2]
	    print "Getnotify arg after -2  " + get_notify_arg_new  
	    pid = os.getpid()
	    libra.appGetnotify(int(pid), get_notify_arg_new,ord('N'))
	    print "Called getnotify"
	    while 1:
	        time.sleep(3)
	    return 0
    gtk.main()	    
    return 0	
    #entry = gtk.Entry(max=50)
    #entry.set_visibility(True)
   
    
    
    
   

if __name__ == "__main__":
    print "1"
    signal.signal(signal.SIGUSR1, sigusrhandler)
    libra = ctypes.CDLL("./nj_nonblocklib.so")
    global window
    print "*********************************************************************"
    pid = os.getpid()
    print 'My PID is:', pid
   
    window.connect("delete_event", gtk.main_quit)
    window.set_title("App with pid = " + str(pid))
    window.set_border_width(0)
    fixed = gtk.Fixed()
    window.add(fixed)
    RadioButtons()
    EntryExample()
    Buttons()
    Submit()
    CheckButton()
    window.show_all()
    
  
    
  

    pidfilename = (str(pid) + ".txt")
    
	

    
#    hbox.pack_start(RadioButtons(window))
#    hbox.pack_start(EntryExample(window))
    

    main()

