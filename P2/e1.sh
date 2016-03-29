#!/bin/bash
if [ ! $# -eq 1 ] && [ ! $# -eq 2 ];
then
	echo "El uso del programa es \"$0 directorio [numBytes]\""
	exit 1
fi

if [ $# -eq 1 ]
then
	tam=0
else
	tam=$2
fi

for x in $(find $1 -type f -size +$tam'c')
do
	echo -n "$(dirname $x);$(basename $x);$(stat $x -c \"%s\");$(stat $x -c \"%i\");$(stat $x -c \"%A\");"
	if [ -x $x ];
	then
		echo "1"
	else
		echo "0"
	fi
done
