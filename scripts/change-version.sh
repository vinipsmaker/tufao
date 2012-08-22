#!/bin/bash

sed -i -e "s,^PROJECT_NUMBER.*\$,PROJECT_NUMBER         = $1,g" ../Doxyfile
sed -i -e "s,^QHP_NAMESPACE^.*\$,QHP_NAMESPACE          = \"tufao.$1\",g" ../Doxyfile
sed -i -e "s,^QHP_VIRTUAL_FOLDER^.*\$,QHP_VIRTUAL_FOLDER     = \"tufao-$1\",g" ../Doxyfile

echo 'Remember to edit manually:'
echo '"../src/tufao_global.h"'
echo '"../README"'
echo '"../CMakeLists.txt"'
