#!/bin/bash
f="series.txt"
echo "****************************"
echo "1) Títulos de las series:"
grep -E '[0-9]+\. ' $f
echo "****************************"
echo "2) Cadenas que producen las series:"
grep '^\* .* \*$' $f
echo "****************************"
echo "3) Cadenas que producen las series sin asteriscos ni espacios:"
grep '^\* .* \*' $f | grep -o '[[:alpha:]]*'
echo "****************************"
echo "4) Eliminar las líneas de sinopsis:"
grep -v 'SINOPSIS:' $f
echo "****************************"
echo "5) Eliminar líneas vacías:"
grep -v '^$' $f
echo "****************************"
echo "6) Contar cuantas series produce cada cadena:"
for x in $(grep '^\* .* \*' $f | grep -o '[[:alpha:]]*' | sort -u)
do
  n=$(grep -c $x $f)
  echo "La cadena $x produce $n series"
done
echo "****************************"
echo "7) Líneas que contengan una palabra en mayúsculas entre paréntesis:"
grep -E '\([[:upper:]].*\)' $f
echo "****************************"
echo "8) Emparejamientos de palabras repetidas en la misma línea:"
grep -o -E '\b([[:alpha:]]+)\b.*\b\1\b' $f
