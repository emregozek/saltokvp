#!/bin/bash

SCRIPT_DIR="$( cd -- "$( dirname -- "${BASH_SOURCE[0]:-$0}"; )" &> /dev/null && pwd 2> /dev/null; )";
PROJECT_TOP_DIR=${SCRIPT_DIR}/../
CUR_DIR=$(pwd)
BUILD_DIR=${PROJECT_TOP_DIR}/build

mkdir -p ${PROJECT_TOP_DIR}/build
cd ${PROJECT_TOP_DIR}/build
cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug -B${BUILD_DIR} -DENABLE_TESTING=OFF -DBUILD_DOC=ON ${PROJECT_TOP_DIR}
make doc_doxygen
printf "\n\nDoxygen documentation is generated in ${BUILD_DIR}/doc_doxygen/ folder\n\n"