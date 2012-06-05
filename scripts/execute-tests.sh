#!/bin/bash

cd .. > /dev/null

for i in $(ls tests); do
    cd tests/$i
    if [ -f motd ]; then
        cat motd
    fi
    qmake CONFIG+=debug
    make > /dev/null
    ./bin/test || exit 1
    make distclean > /dev/null
    cd - > /dev/null
done
