#!/bin/bash
function copiar_convertir()
{
  if [ -e $2 ];
  then
    read -p "El fichero \"$2\" ya existe. ¿Desea sobrescribirlo? [S/N]: " sn
    if [ -z $sn ];
    then
      sn="n"
    fi
    if [ $sn == "S" ] || [ $sn == "s" ];
    then
      if [[ $2 == *".pdf" ]]
      then
        convert $1 $2
        return 0;
      else
        cp $1 $2
        return 0;
      fi
    fi
  else
    if [[ $2 == *".pdf" ]]
    then
      convert $1 $2
      return 0;
    else
      cp $1 $2
      return 0;
    fi
  fi
  return 1;
}

rm -f ejercicio6.log
let SECONDS=0

if [ $# -eq 0 ];
then
	carpetas=.
else
  carpetas=$*
fi

let ndir=0
let neje=0
let nlib=0
let nimg=0
let ninc=0

echo "Introduzca el directorio donde copiar los ejecutables [\$HOME/bin]: "
read -t5 ejecutables
if [ -z "$ejecutables" ];
then
  ejecutables=$HOME/bin
fi
if [ ! -d $ejecutables ];
then
  mkdir "$ejecutables";
fi

echo "Introduzca el directorio donde copiar las librerías [\$HOME/lib]: "
read -t5 librerias
if [ -z "$librerias" ];
then
  librerias=$HOME/lib
fi
if [ ! -d $librerias ];
then
  mkdir "$librerias";
fi

echo "Introduzca el directorio donde copiar las imágenes [\$HOME/img]: "
read -t5 imagenes
if [ -z "$imagenes" ];
then
  imagenes=$HOME/img
fi
if [ ! -d $imagenes ];
then
  mkdir "$imagenes";
fi

echo "Introduzca el directorio donde copiar las cabeceras [\$HOME/include]: "
read -t5 cabeceras
if [ -z "$cabeceras" ];
then
  cabeceras=$HOME/include
fi
if [ ! -d $cabeceras ];
then
  mkdir "$cabeceras";
fi


for x in $carpetas
do
  if [ -d $x ];
  then
    for y in $(find "$x" -type f)
    do
      if [ -x "$y" ];
      then
        if copiar_convertir $y $ejecutables/$(basename $y)
        then
          echo "[$(date +"%d-%m-%y / %H:%M:%S")] Fichero \"$y\" copiado en \"$ejecutables\"" >> ejercicio6.log
          let neje=neje+1
        fi
      fi
      if [[ $(basename "$y") == "lib"* ]];
      then
        if copiar_convertir $y $librerias/$(basename $y)
        then
          echo "[$(date +"%d-%m-%y / %H:%M:%S")] Fichero \"$y\" copiado en \"$librerias\"" >> ejercicio6.log
          let nlib=nlib+1
        fi
      fi
      if [[ $(basename "$y") == *".jpg" ]];
      then
        if copiar_convertir $y $imagenes/$(basename $y .jpg).pdf
        then
          echo "[$(date +"%d-%m-%y / %H:%M:%S")] Fichero \"$y\" convertido en \"$imagenes/$(basename $y .jpg).pdf\"" >> ejercicio6.log
          let nimg=nimg+1
        fi
      elif [[ $(basename "$y") == *".gif" ]];
      then
        if copiar_convertir $y $imagenes/$(basename $y .gif).pdf
        then
          echo "[$(date +"%d-%m-%y / %H:%M:%S")] Fichero \"$y\" convertido en \"$imagenes/$(basename $y .gif).pdf\"" >> ejercicio6.log
          let nimg=nimg+1
        fi
      elif [[ $(basename "$y") == *".png" ]];
      then
        if copiar_convertir $y $imagenes/$(basename $y .png).pdf
        then
          echo "[$(date +"%d-%m-%y / %H:%M:%S")] Fichero \"$y\" convertido en \"$imagenes/$(basename $y .png).pdf\"" >> ejercicio6.log
          let nimg=nimg+1
        fi
      elif [[ $(basename "$y") == *".h" ]];
      then
        if copiar_convertir $y $cabeceras/$(basename $y)
        then
          echo "[$(date +"%d-%m-%y / %H:%M:%S")] Fichero \"$y\" copiado en \"$cabeceras\"" >> ejercicio6.log
          let ninc=ninc+1
        fi
      fi
    done
    let ndir=ndir+1
  else
    echo "El directorio \"$x\" no existe";
  fi
done

echo "Número de directorios procesados: $ndir"
echo "Número de ficheros ejecutables: $neje"
echo "Número de librerías: $nlib"
echo "Número de imágenes: $nimg"
echo "Número de ficheros de cabecera: $ninc"
echo "Tiempo necesario: $SECONDS segundos"
