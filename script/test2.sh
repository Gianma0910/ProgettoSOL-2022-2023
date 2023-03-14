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

valgrind --error-exitcode=1 ./farm -d testdir -t 200 &
pid=$!
sleep 3
pkill farm
wait $pid
if [[ $? != 0 ]]; then
    echo "test failed"
else
    echo "test passed"
fi
