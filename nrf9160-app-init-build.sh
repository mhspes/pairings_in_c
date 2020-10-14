#!/bin/bash

set -e

GCC_ARM_TOOLCHAIN_ID=gcc-arm-none-eabi-9-2019-q4-major
SDK_ID=nRF5SDK160098a08e2

if [[ `uname` == "Darwin" ]]; then
    GCC_ARM_TOOLCHAIN_BASE=/Volumes/Untitled/opt/GnuArmEmb
    SDK_BASE=/Volumes/Untitled/SDK
    READLINK=greadlink
else
    GCC_ARM_TOOLCHAIN_BASE=/windows/opt/GnuArmEmbLinux
    SDK_BASE=/windows/SDK
    READLINK=readlink
fi

export GCC_ARM_TOOLCHAIN_DIR=${GCC_ARM_TOOLCHAIN_BASE}/${GCC_ARM_TOOLCHAIN_ID}
export GCC_ARM_TOOLCHAIN_BIN=${GCC_ARM_TOOLCHAIN_DIR}/bin

BASE_DIR=`dirname $0`
BASE_DIR=`${READLINK} -f ${BASE_DIR}`

BUILD_DIR=${BASE_DIR}/build
CMSIS_DIR=${SDK_BASE}/ncs/modules/hal/cmsis/CMSIS

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
