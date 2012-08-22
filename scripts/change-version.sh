#!/usr/bin/env bash

IFS='.' read MAJOR MINOR PATCH <<< "${1}"

if [ -z "${MAJOR}" ]; then
    echo "SYNTAX ERROR"
    echo "Usage:"
    echo "$0 \${VERSION_MAJOR}"
    echo "$0 \${VERSION_MAJOR}.\${VERSION_MINOR}"
    echo "$0 \${VERSION_MAJOR}.\${VERSION_MINOR}.\${VERSION_PATCH}"
    exit 1
fi

if [ -z "${MINOR}" ]; then
    MINOR='0'
fi

if [ -z "${PATCH}" ]; then
    PATCH='0'
fi

PROJECT_ROOT="$(dirname $0)/.."
PROJECT_CONF="${PROJECT_ROOT}/CMakeLists.txt"

# Edit CMakeLists.txt
sed -i -r -e "s,(TUFAO_VERSION_MAJOR +)[[:digit:]]+,\1${MAJOR},g" "${PROJECT_CONF}"
sed -i -r -e "s,(TUFAO_VERSION_MINOR +)[[:digit:]]+,\1${MINOR},g" "${PROJECT_CONF}"
sed -i -r -e "s,(TUFAO_VERSION_PATCH +)[[:digit:]]+,\1${PATCH},g" "${PROJECT_CONF}"

# Edit CMakeLists.txt
sed -i -r -e "s,(TUFAO_VERSION_MAJOR +)[[:digit:]]+,\1${MAJOR},g" \
    "${PROJECT_ROOT}/src/tufao_global.h"
sed -i -r -e "s,(TUFAO_VERSION_MINOR +)[[:digit:]]+,\1${MINOR},g" \
    "${PROJECT_ROOT}/src/tufao_global.h"

echo 'Remember to update the README file'
