#!/bin/bash

set -e

READLINK=readlink

# Edit the paths and versions
# Note, the SDK and CMSIS are assumed to be in the same base directory as the pairing repository.

# GCC toolchain and SDK versions
GCC_ARM_TOOLCHAIN_ID=gcc-arm-none-eabi-9-2020-q2-update
SDK_ID=nRF5SDK17009d13099

# GCC toolchain and SDK BASE directories
GCC_ARM_TOOLCHAIN_BASE=/usr/local
SDK_BASE=~/files/project_base_directory

# CMSIS path, modify if needed
CMSIS_DIR=${SDK_BASE}/CMSIS_5/CMSIS

export GCC_ARM_TOOLCHAIN_DIR=${GCC_ARM_TOOLCHAIN_BASE}/${GCC_ARM_TOOLCHAIN_ID}
export GCC_ARM_TOOLCHAIN_BIN=${GCC_ARM_TOOLCHAIN_DIR}/bin

BASE_DIR=`dirname $0`
BASE_DIR=`${READLINK} -f ${BASE_DIR}`
BUILD_DIR=${BASE_DIR}/build

if [[ -n ${CLEAN} ]]; then

    if [[ -n ${VERBOSE} ]]; then
        CMAKE_VERBOSE="-DCMAKE_VERBOSE_MAKEFILE:BOOL=ON"
    fi

    rm -Rf ${BUILD_DIR}
    mkdir  ${BUILD_DIR}

    cmake -DARCHITECTURE=ARCH_CORTEXM33                                                                 \
          -DSDK_RAW_METAL=${SDK_BASE}/${SDK_ID}                                                         \
          -DCMSIS_DIR=${CMSIS_DIR}                                                                      \
          -DCMAKE_BUILD_TYPE=Debug                                                                      \
          -DCMAKE_TOOLCHAIN_FILE=${BASE_DIR}/framework/cmake/toolchain/arm-none-eabi-gcc-nrf9160.cmake  \
          ${CMAKE_VERBOSE}                                                                              \
          -B ${BUILD_DIR}                                                                               \
          -S ${BASE_DIR}/framework
fi

cmake --build ${BUILD_DIR} --target bench
