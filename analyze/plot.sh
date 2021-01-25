#! /bin/sh 

/usr/bin/gnuplot <<EOF 
set terminal png
set output "$2"
set zrange [-1:100]
set xlabel "dist to prev"
set ylabel "index dist"
set zlabel "GB\sec"
set title "Mem test blocks: 2048"
set pm3d
set view 60,25
splot "$1" matrix w l
show output
EOF
