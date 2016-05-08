#!/bin/bash
#Se le debe pasar un directorio que contenga la carpeta etc
if [ ! -d "$1" ];
then
	echo "El directorio $1 no existe"
	exit 1
elif [ ! -d "$1/etc/init.d" ];
then
  echo "El directorio $1 no contiene una carpeta de scripts de arranque (no existe $1/etc/init.d/)"
  exit 1
fi
echo "***************************"
echo "Número de enlaces simbólicos: $(find $1 -type l | wc -l)"
echo "Número de directorios: $(find $1 -type d | wc -l)"
echo "Número de ficheros convencionales ejecutables: $(find $1 -executable -type f | wc -l)"
echo "***************************"
echo "El listado de los intérpretes en $1/etc/init.d/ es:"
for x in $(find $1/etc/init.d -type f)
do
  sed -n -e '1 p' $x
done | sed -n -r -e 's/^#! ?(\S+).*/\1/p' | sort | uniq -c | sed -r -e 's/ *([0-9]+) (\S+)/- Hay \1 scripts con intérprete \2/'
echo "***************************"
for x in $(find $1/etc/init.d -type f | grep -v 'README')
do
  fich=$(basename $x)
  echo "-------------"
  echo "Script $fich"
  find $1/etc -type l | grep -E "rc.\.d/[SK][0-9]{2,2}$fich$" | sort | sed -r -e 's/.*rc(.)\.d\/S([0-9]{2,2}).*/Nivel \1: Arranque con prioridad \2/' | sed -r -e 's/.*rc(.)\.d\/K([0-9]{2,2}).*/Nivel \1: Parada con prioridad \2/'
done
