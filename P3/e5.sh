#!/bin/bash
grep -m 1 '^model name' /proc/cpuinfo | sed -r -e 's/.*: (.*)/Modelo de procesador: \1/'
grep -m 1 '^cpu MHz' /proc/cpuinfo | sed -r -e 's/.*: (.*)/Megahercios: \1/'
echo -n "Número de hilos máximo en ejecución: "
grep -c '^model name' /proc/cpuinfo
echo "Puntos de montaje: "
cat /proc/mounts | sort -k 3 | sed -r -e 's/^(.*) (.*) (.*) .* .* .*/-> Punto de montaje: \2, Dispositivo: \1, Tipo de dispositivo: \3/'
