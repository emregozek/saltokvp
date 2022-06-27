#!/bin/bash

SCRIPT_DIR="$( cd -- "$( dirname -- "${BASH_SOURCE[0]:-$0}"; )" &> /dev/null && pwd 2> /dev/null; )";
PROJECT_TOP_DIR=${SCRIPT_DIR}/../
CUR_DIR=$(pwd)
BUILD_DIR=${PROJECT_TOP_DIR}/build/release

while :; do clear; cat ${BUILD_DIR}/src/saltokvp.txt; sleep 1; done
