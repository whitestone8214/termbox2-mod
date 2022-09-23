#!/bin/bash


CC="gcc"


if (test "$1" = "shared"); then
	echo ":: Build libtermbox.so for dynamic linking"
	rm -f libtermbox.so || exit 1
	${CC} -O3 -fPIC -shared -o libtermbox.so -DTB_IMPL -DTB_LIB_OPTS -I. termbox.c || exit 1
elif (test "$1" = "static"); then
	echo ":: Build termbox.o for static linking"
	rm -f termbox.o || exit 1
	${CC} -O3 -fPIC -c -o termbox.o -DTB_IMPL -DTB_LIB_OPTS -I. termbox-static.h || exit 1
else
	echo "Usage: ./setup.sh [shared | static]"
	echo "Example: ./setup.sh shared"
	
	exit 1
fi
