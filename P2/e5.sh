#!/bin/bash

if [ ! $# -eq 2 ];
then
	echo "El uso del programa es \"$0 [clave] [num. caracteres]\""
	exit 1
fi

case $2 in
  1) patrones=$(echo {a..z}) ;;
  2) patrones=$(echo {a..z}{a..z}) ;;
  3) patrones=$(echo {a..z}{a..z}{a..z}) ;;
  *) echo "Solo se admiten claves de 1 a 3 caracteres"
     exit 1 ;;
esac

for x in $patrones
do
  resumen=$(echo $x | sha1sum | tr -d '  -')
  if [ $resumen == $1 ]
  then
    echo "Clave encontrada: $x"
    exit 0
  fi
done

echo "La clave no ha sido encontrada"
exit 1
