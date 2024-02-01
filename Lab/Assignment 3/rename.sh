#!/bin/bash

filename='/tmp/new21.txt'
newname='/tmp/new22.txt'

cp $filename '/tmp/renametmp.txt'
./rename $filename $newname

if [ $? -eq 0 ]
then
	diff $newname '/tmp/renametmp.txt' 
	if [ $? -eq 0 ]
	then
		echo "rename pass"
	else
		echo "rename fail"
	fi
	rm '/tmp/renametmp.txt'
else
	echo "rename fail"
fi