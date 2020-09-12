#!/bin/bash
i=$1
make
while [ $i -ne 0 ]
do
	./cs311em -m 0x10000000:0x100FFFFC -n $i sample_input/$2 > mine.txt
	./cs311test -m 0x10000000:0x100FFFFC -n $i sample_input/$2 > ta.txt
	diff mine.txt ta.txt
	i=$(($i-1))
done
