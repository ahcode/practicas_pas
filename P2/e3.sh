#!/bin/bash
function contenidoMayuscula()
{
	for x in $(find $1 -mindepth 1 -maxdepth 1)
	do
		mayus=$(echo "$(basename $x)" | tr 'a-z' 'A-Z')
		mv $x "$(dirname $x)/$mayus"
		if [ -d "$(dirname $x)/$mayus" ];
		then
			contenidoMayuscula "$(dirname $x)/$mayus"
		fi
	done
}

if [ ! $# -eq 2 ];
then
	echo "El uso del programa es \"$0 [origen] [destino]\""
	exit 1
fi

if [ ! -d $1 ];
then
	echo "El directorio $1 no existe"
	exit 1
fi

if [ -d $2 ];
then
	rm -r $2
fi

mkdir $2
cp -r $1 $2

contenidoMayuscula $2
