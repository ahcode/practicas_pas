#!/bin/bash

if [ ! $# -eq 1 ];
then
	echo "El uso del programa es \"$0 [fichero o directorio]\""
	exit 1
fi

if [ -d $1 ];
then
  echo "Procesando un directorio"
  rm -f $1.tar.gz
  rm -f $1.tar.gz.gpg
  tar -czf $1.tar.gz $1
  gpg --symmetric --output $1.tar.gz.gpg $1.tar.gz
  #Descomenta para eliminar el fichero comprimido creado después de cifrar
  #rm $1.tar.gz
  echo "Fichero cifrado resultante: $1.tar.gz.gpg"
elif [ -e $1 ];
then
  echo "Procesando un fichero"
  rm -f $1.gpg
  gpg --symmetric --output $1.gpg $1
  echo "Fichero cifrado resultante: $1.gpg"
else
  echo "El fichero o carpeta \"$1\" no existe"
  exit 1
fi
