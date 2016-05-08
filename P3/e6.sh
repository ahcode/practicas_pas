#!/bin/bash
if [ ! -d "$1" ];
then
	echo "El directorio $1 no existe"
	exit 1
elif [ ! -d "$1/init.d" ];
then
  echo "El dicrectorio $1 no es una carpeta de scripts de arranque (no existe $1/init.d/)"
  exit 1
fi
echo "***************************"
let dir=0
let sym=0
let exe=0
for x in $(find $1 -mindepth 1)
do
  if [ -d $x ];
  then
    let dir=dir+1
  elif [ -L $x ];
  then
    let sym=sym+1
  elif [ -x $x ];
  then
    let exe=exe+1
  fi
done

echo "Número de enlaces simbólicos: $sym"
echo "Número de directorios: $dir"
echo "Número de ficheros convencionales ejecutables: $exe"
echo "***************************"
echo "El listado de los intérpretes en $1/init.d/ es:"
for x in $(find $1/init.d -type f)
do
  sed -n -e '1 p' $x
done | sed -n -r -e 's/^#! ?(\S+).*/\1/p' | sort | uniq -c | sed -r -e 's/ *([0-9]+) (\S+)/- Hay \1 scripts con intérprete \2/'
echo "***************************"
for x in $(find $1/init.d -type f | grep -v 'README')
do
  fich=$(basename $x)
  echo "-------------"
  echo "Script $fich"
  find $1 -type l | grep -E "rc.\.d/[SK][0-9]{2,2}$fich$" | sort | sed -r -e 's/.*rc(.)\.d\/S([0-9]{2,2}).*/Nivel \1: Arranque con prioridad \2/' | sed -r -e 's/.*rc(.)\.d\/K([0-9]{2,2}).*/Nivel \1: Parada con prioridad \2/'
done
