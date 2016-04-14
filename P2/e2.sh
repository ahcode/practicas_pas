#!/bin/bash
if [ $# -lt 1 ] || [ $# -gt 3 ];
then
	echo "El uso del programa es \"$0 directorio [umbral1] [ubral2]\""
	exit 1
fi

if [ ! -d $1 ];
then
	echo "El directorio $1 no existe"
	exit 1
fi

rm -fr $1/pequenos $1/medianos $1/grandes

echo "Creando las carpetas pequenos, medianos y grandes"
mkdir $1/pequenos $1/medianos $1/grandes

if [ $# -eq 1 ]
then
	umbral1=10000
	umbral2=100000
else
	umbral1=$2
	if [ $# -eq 2 ]
	then
		umbral2=100000
	else
		umbral2=$3
	fi
fi

echo "Copiando los archivos..."
for x in $(find $1 -type f)
do
	if [ $(stat $x -c "%s") -lt $umbral1 ]
	then
		cp $x $1/pequenos
	else
		if [ $(stat $x -c "%s") -lt $umbral2 ]
		then
			cp $x $1/medianos
		else
			cp $x $1/grandes
		fi
	fi
done
