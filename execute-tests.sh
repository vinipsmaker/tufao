#!/bin/bash

for i in $(ls tests); do
    cd tests/$i
    if [ -f motd ]; then
        cat motd
    fi
    qmake
    make > /dev/null
    ./bin/test || exit 1
    cd - > /dev/null
done
