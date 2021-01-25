#! /bin/sh 

/usr/bin/gnuplot <<EOF 
set terminal png large enhanced size 1024,768
set output "$2"

set zrange [0:2]
set xlabel "L"
set ylabel "alpha"
set zlabel "10^9 DAPS"

set title "APEX for Intel Xeon X5650"
set pm3d
set view 40,300

set cntrparam level discrete 1.26,0.57,0.96,0.40,1.30,1.12

set xtics ("1" 0, "2" 1, "4" 2, "8" 3, "16" 4 , "32" 5 , "64" 6 , "128" 7  , "256" 8  , "512" 9  , "1024" 10  , "2048" 11, "4096" 12, "8192" 13, "16384" 14, "32768" 15 )
set ytics ("1.0" 0, "0.5" 1, "0.25" 2, "0.125" 3, "0.06" 4, "0.03" 5, "0.015" 6, "0.007" 7, "0.004" 8, "0.001953" 9, "0.000977" 10, "0.000488" 11, "0.0002" 12, "0.0001" 13)
splot "$1" matrix w l
show output
EOF

