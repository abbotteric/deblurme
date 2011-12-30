#!/bin/bash

if test $# -eq 0
then
	echo "Usage: "$0 " <source> <destination>"
	exit 1
fi

gcc -g -o ./$2 ./$1 $(pkg-config --cflags --libs MagickWand)  
