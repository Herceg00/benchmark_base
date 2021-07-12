for f in ../input/*/*.virt.log*
do 
	echo -n "$f   "
	./emul $f | tail -n 1 
done

