#! /bin/sh 

/usr/bin/gnuplot <<EOF 
set terminal png font "/usr/share/fonts/liberation/LiberationSans-Regular.ttf" 10 size 1024,768 enhanced
set xlabel "Memory access"
set ylabel "Virtual address"
plot "$1" using 1 title "diff to prev" with $2, \
     "$1" using 2 title "diff to prev prev" with $2
EOF
