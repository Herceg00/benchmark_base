name=`basename $1`

python diff.py $1 4 > diff/$name.diff4

python histo.py diff/$name.diff4 4 > histo/$name.histo4

bash plot_hist.sh histo/$name.histo4 "$2" > pics/$name.histo4.png
