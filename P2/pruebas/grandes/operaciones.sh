#!/bin/bash
read -p "Introduzca el primer número: " N1
read -p "Introduzca el segundo número " N2
echo "Suma: $[$N1+$N2]"
echo "Resta: $[$N1-$N2]"
echo "Multiplicación: $[$N1*$N2]"
echo "División: $[$N1/$N2]"
echo "Módulo: $[$N1%$N2]"
