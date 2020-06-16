#!/bin/bash

for L in 640
do
    ../local/bin/perc05 L=$L | tee data-L$L.dat 
done
