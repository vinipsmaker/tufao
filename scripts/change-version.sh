#!/bin/bash

sed -i -e "s/^PROJECT_NUMBER.*\$/PROJECT_NUMBER         = $1/g" ../Doxyfile
sed -i -e "s/^VERSION.*\$/VERSION = $1/g" ../tufao.pro

echo 'Edit "..src/tufao_global.h" file manually'
