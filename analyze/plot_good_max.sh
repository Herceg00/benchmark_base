#! /bin/sh 

/usr/bin/gnuplot <<EOF 
set terminal png font "/usr/share/fonts/liberation/LiberationSans-Regular.ttf" 25 size 2048,1534 enhanced
set xlabel "$4"
set ylabel "$5"
set yrange [0:$6]
set title "$3" font "/usr/share/fonts/liberation/LiberationSans-Bold.ttf,25"
plot "$1" with $2 notitle
EOF
