#!/bin/bash

for i in $(ls tests); do
    cd tests/$i
    qmake
    make > /dev/null
    ./bin/test
    cd -
done
