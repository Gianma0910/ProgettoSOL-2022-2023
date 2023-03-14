#!/bin/bash

if [ ! -e generafile ];
then
    echo "Compilare generafile, eseguibile mancante!";
    exit 1
fi
if [ ! -e farm ];
then
    echo "Compilare farm, eseguibile mancante!"
    exit 1
fi

valgrind --leak-check=full --error-exitcode=1 ./farm file1.dat file2.dat file3.dat file4.dat file5.dat file6.dat file7.dat -t 200 &
pid=$!
sleep 1
pkill farm
wait $pid
if [[ $? != 0 ]]; then
    echo "test failed"
else
    echo "test passed"
fi
