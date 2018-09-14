#!/bin/bash

let count0=0
let count1=0;
let count2=0;
for f in $(ls ./tests/*.txt); do 
	./a.out 0  < $f > ./tests/`basename $f .txt`.output0;
	./a.out 1  < $f > ./tests/`basename $f .txt`.output1;
	./a.out 2  < $f > ./tests/`basename $f .txt`.output2;
	diff -Bw  ./tests/`basename $f .txt`.output0  ${f}.expected0 > ./tests/`basename $f .txt`.diff0;
	diff -Bw  ./tests/`basename $f .txt`.output1  ${f}.expected1 > ./tests/`basename $f .txt`.diff1;
	diff -Bw  ./tests/`basename $f .txt`.output2  ${f}.expected2 > ./tests/`basename $f .txt`.diff2;
done;

for f in $(ls tests/*.txt); do
	echo "========================================================";
	echo "TEST CASE:" `basename $f .txt`;
	echo "========================================================";
	d0=./tests/`basename $f .txt`.diff0;
	d1=./tests/`basename $f .txt`.diff1;
	d2=./tests/`basename $f .txt`.diff2;
	if [ -s $d0 ]; then
		echo "For task 0, there is an output missmatch:"
		cat $d0
	else
		count0=$((count0 + 1));
		echo "Task 0: Passed";
	fi
	echo "-----------------------------------------------";
	if [ -s $d1 ]; then
		echo "For task 1, there is an output missmatch:"
		cat $d1
	else
		count1=$((count1 + 1));
		echo "Task 1: Passed";
	fi
	echo "-----------------------------------------------";
	if [ -s $d2 ]; then
		echo "For task 2, there is an output missmatch:"
		cat $d2
	else
		count2=$((count2 + 1));
		echo "Task 2: Passed";
	fi
done

echo
echo "Task 0 correct count:" $count0;
echo "Task 1 (FIRST) correct count:" $count1;
echo "Task 2 (FOLLOW) correct count:" $count2;

rm tests/*.output?
rm tests/*.diff?

