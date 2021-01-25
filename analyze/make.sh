name=`basename $1`

python histo.py $1 > histo/$name.histo

python diff.py $1 1 > diff/$name.diff1
python diff.py $1 2 > diff/$name.diff2
python diff.py $1 4 > diff/$name.diff4

./plot_good.sh $1 dots "profile" "Memory access" "Address" > pics/$name.prof.png
./plot_good.sh histo/$name.histo lines "histogram" "Adddres" "Access count" > pics/$name.histo.png
./plot_good.sh diff/$name.diff1 points "diff between 1 last pair of accesses" "memory access" "diff1" > pics/$name.diff1.png
./plot_good.sh diff/$name.diff2 points "diff between 2 last pair of accesses" "memory access" "diff2" > pics/$name.diff2.png
./plot_good.sh diff/$name.diff4 points "diff between 4 last pair of accesses" "memory access" "diff4" > pics/$name.diff4.png

./plot_good_max.sh output/${name}_512.rects lines "CVG 64*512" "memory access" "CVG" 520 > pics/$name.cvg64.png
./plot_good_max.sh output/${name}_512.zero lines "CVG 1*512" "memory access" "CVG" 520 > pics/$name.cvg1.png


