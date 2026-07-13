#!/bin/bash

gcc -O0 tlb.c -o tlb

for((i = 1; i < 2049; i = i * 2))
do
	if [[ $i -lt 33 ]]; then
		./tlb $i 500000000
	else
    	./tlb $i 3000000
	fi
done