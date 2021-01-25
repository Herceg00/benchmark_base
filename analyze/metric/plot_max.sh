#! /bin/sh 

/usr/bin/gnuplot <<EOF 
set terminal png font "/usr/share/fonts/liberation/LiberationSans-Regular.ttf" 10 size 1024,768 enhanced
set xlabel "Memory access"
set ylabel "Virtual address"
set yrange [0:$4]
set title "$3" font "/usr/share/fonts/liberation/LiberationSans-Bold.ttf,15"
plot "$1" with $2 notitle
EOF
