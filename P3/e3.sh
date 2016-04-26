#!/bin/bash
echo "Listado de archivos ocultos del directorio $HOME"
ls -a $HOME | grep '^\..*'
echo "===="
echo "El fichero a procesar es $1"
sed '/^$/d' $1 > $1.sinLineasVacias
echo "El fichero sin lineas vacias se ha guardado en $1.sinLineasVacias"
echo "===="
echo "Listado de procesos ejecutados por $USER"
ps -u $USER -U $USER -o pid,start,comm | sed -r -n -e 's/^ +\b([0-9]+)\b +\b([0-9]{2,2}:[0-9]{2,2}).*\b +(.+)$/PID: \"\1\" Hora: \"\2\" Ejecutable \"\3\"/p'
