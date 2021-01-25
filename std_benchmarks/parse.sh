odir=res_11_27
mkdir $odir
rm $odir/*

for n in 128 256 512 1024 2048 4096 8192 16384 32768
do
	echo "name,$n," > $odir/$n
	for f in */*_STD_$n.out-*
	do
#		cat $f | grep rects1 | awk '{print $9 "_" $13 "," $21 ","}' >> $odir/$n
		cat $f | grep rects1 | awk '{print $21}' >> $odir/$n
	done
done
