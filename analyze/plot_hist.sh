#! /bin/sh 

/usr/bin/gnuplot <<EOF 
set terminal png font "/usr/share/fonts/liberation/LiberationSans-Regular.ttf" 25 size 2048,1536 enhanced
set xlabel "address distance to prev. element"
set ylabel "frequency"
set style fill solid 0.5 noborder
set boxwidth 0.95 relative
set grid
set format y '%2.0f%%' 
set xtics (0, 1, 2, 3, 4, 5, 6, 7, 8, "9-64" 9, "64++" 10)

set title "$2" font "/usr/share/fonts/liberation/LiberationSans-Bold.ttf,25"
plot "$1" u 1 w histogram title "Ni - N_i_-_1",\
"$1" u 2 w histogram title "Ni - N_i_-_2",\
"$1" u 3 w histogram title "Ni - N_i_-_3",\
"$1" u 4 w histogram title "Ni - N_i_-_4",\
"$1" u 5 w histogram title "Ni - N_i_-_5",\
"$1" u 6 w histogram title "Ni - N_i_-_6",\
"$1" u 7 w histogram title "Ni - N_i_-_7",\
"$1" u 8 w histogram title "Ni - N_i_-_8",\
"$1" u 9 w histogram title "Ni - N_i_-_9",\
"$1" u 10 w histogram title "Ni - N_i_-_10",\
"$1" u 11 w histogram title "Ni - N_i_-_11",\
"$1" u 12 w histogram title "Ni - N_i_-_12",\
"$1" u 13 w histogram title "Ni - N_i_-_13",\
"$1" u 14 w histogram title "Ni - N_i_-_14",\
"$1" u 15 w histogram title "Ni - N_i_-_15",\
"$1" u 16 w histogram title "Ni - N_i_-_16"
EOF
