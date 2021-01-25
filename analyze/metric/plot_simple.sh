#! /bin/sh 

/usr/bin/gnuplot <<EOF 
set terminal png large size 1024,768 enhanced
plot "$1" title "$1" with $2
EOF
