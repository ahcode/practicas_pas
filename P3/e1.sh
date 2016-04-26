#!/bin/bash
echo "****************************"
echo "1) Títulos de las series:"
grep -E '[0-9]+\. ' $1
echo "****************************"
echo "2) Cadenas que producen las series:"
grep '^\* .* \*$' $1
echo "****************************"
echo "3) Cadenas que producen las series sin asteriscos ni espacios:"
grep '^\* .* \*' $1 | grep -o '[[:alpha:]]*'
echo "****************************"
echo "4) Eliminar las líneas de sinopsis:"
grep -v 'SINOPSIS:' $1
echo "****************************"
echo "5) Eliminar líneas vacías:"
grep -v '^$' $1
echo "****************************"
echo "6) Contar cuantas series produce cada cadena:"
for x in $(grep '^\* .* \*' $1 | grep -o '[[:alpha:]]*' | sort -u)
do
  n=$(grep -c $x $1)
  echo "La cadena $x produce $n series"
done
echo "****************************"
echo "7) Líneas que contengan una palabra en mayúsculas entre paréntesis:"
grep -E '\([[:upper:]].*\)' $1
echo "****************************"
echo "8) Emparejamientos de palabras repetidas en la misma línea:"
grep -o -E '\b([[:alpha:]]+)\b.*\b\1\b' $1
echo "****************************"
echo "9) Líneas que contienen 28 aes o más:"
grep -E '([aA].*){28,}' $1
echo "10) Nombre de película y temporadas:"
grep -E '^=+$' -C 1 $1
