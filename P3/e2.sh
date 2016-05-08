#!/bin/bash
if [ ! -e "$1" ];
then
	echo "El fichero $1 no existe"
	exit 1
fi
sed -e '/^=*$/d' $1 | sed -r -e 's/([0-9]+\..*) \((.*)\)/\1\n\|-> Año de la serie: \2/' | sed -e 's/ Ver mas//' | sed -r -e 's/([0-9]+) TEMPORADAS/\|-> Número de temporadas: \1/' | sed -r -e 's/\* (.*) \*/\|-> Productora de la serie: \1/' | sed -r -e 's/SINOPSIS: (.*)/\|-> Sinopsis: \1/' | sed -r -e 's/Ha recibido ([0-9]+) puntos/\|-> Número de puntos: \1/'
