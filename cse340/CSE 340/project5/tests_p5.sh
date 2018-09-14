#!/bin/bash

let count0=0
let totalcount=0;
for f in $(ls ./tests/*.txt); do 
	./a.out < $f > ./tests/`basename $f .txt`.output
	diff -Bw  ./tests/`basename $f .txt`.output  ${f}.expected > ./tests/`basename $f .txt`.diff0;
done;

for f in $(ls tests/*.txt); do
	totalcount=$((totalcount + 1));
	echo "========================================================";
	echo "TEST CASE:" `basename $f .txt`;
	echo "========================================================";
	d0=./tests/`basename $f .txt`.diff0;
	if [ -s $d0 ]; then
		echo "There is an output missmatch:"
		cat $d0
	else
		count0=$((count0 + 1));
		echo "Task: Passed";
	fi
done

echo
echo "Correct count:" $count0;
echo "OUT OF: " $totalcount;
