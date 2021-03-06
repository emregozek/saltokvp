#!/bin/bash

SCRIPT_DIR="$( cd -- "$( dirname -- "${BASH_SOURCE[0]:-$0}"; )" &> /dev/null && pwd 2> /dev/null; )";
PROJECT_TOP_DIR=${SCRIPT_DIR}/../
CUR_DIR=$(pwd)
BUILD_DIR=${PROJECT_TOP_DIR}/build/release

mkdir -p ${BUILD_DIR}
cd ${BUILD_DIR}
cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release -B${BUILD_DIR} -DENABLE_TESTING=OFF ${PROJECT_TOP_DIR}
make kvp_server_exec
cd ${BUILD_DIR}/src
./kvp_server_exec