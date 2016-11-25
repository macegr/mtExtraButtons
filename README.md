# mtExtraButtons
Map extra mouse buttons (3 and 4 out of zero-indexed button list) to forward and back shortcuts in OSX

This is a very hacky program to get some functionality out of the extra buttons on a mouse that has more buttons than Apple thinks you'd ever use. The ONLY thing is does is map the thumb buttons to forward and back shortcuts that work in web browsers on OSX (as well as in Finder). The shortcuts are Command-[ and Command-].

I am not an OSX developer so this is not guaranteed to be good code, or recommended for anyone ever. You should test it from the terminal before trying to make it start up with your computer.

You will need to install Xcode, and then use Xcode to install the command-line tools. After that, you can compile:

gcc -Wall -o mtExtraButtons mtExtraButtons.c -framework ApplicationServices

This program has been tested on only one computer running OSX El Capitan.
