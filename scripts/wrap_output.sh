#!/bin/sh

for i in `ls ./output/*.out`; do
    fmt -s -w 65 $i > ./temp.wrap.1
    cat ../scripts/wrap_start.txt ./temp.wrap.1 ../scripts/wrap_end.txt > ./temp.wrap.2
    rm ./temp.wrap.1
    mv temp.wrap.2 $i
done
