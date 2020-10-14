# some basic configurations for the target platform
set(CPU	  cortex-m33	CACHE STRING "Device name for the compiler and tools (e.g.: cortex-m33)")

include(CMakeForceCompiler)

if(DEFINED ENV{GCC_ARM_TOOLCHAIN_BIN})
    set(GCC_ARM_TOOLCHAIN_BIN $ENV{GCC_ARM_TOOLCHAIN_BIN})
else()
   set(GCC_ARM_TOOLCHAIN_BIN "/Volumes/Untitled/opt/GnuArmEmb/gcc-arm-none-eabi-9-2019-q4-major/bin")
endif()

LIST(APPEND CMAKE_PROGRAM_PATH ${GCC_ARM_TOOLCHAIN_BIN})

# specify the cross compiler
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_C_COMPILER   arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER arm-none-eabi-g++)

add_link_options( --specs=nosys.specs )
add_compile_options( -Wall -pedantic -W -Wpointer-arith -Wcast-align -Wconversion -Wsign-compare -Wstrict-prototypes -Wmissing-prototypes -Wmissing-declarations -Wredundant-decls -Wnested-externs )
add_definitions( -DNRF9160_XXAA )

include_directories( ${CMSIS_DIR}/Core/Include
		     ${SDK_RAW_METAL}/modules/nrfx/mdk 
	)

# extend linker invocations so that map files are created
set( CMAKE_C_LINK_EXECUTABLE
     "<CMAKE_C_COMPILER> <FLAGS> <CMAKE_C_LINK_FLAGS> -Wl,-Map,<TARGET>.map <LINK_FLAGS> <OBJECTS> -o <TARGET> <LINK_LIBRARIES>")
set( CMAKE_CXX_LINK_EXECUTABLE
     "<CMAKE_CXX_COMPILER> <FLAGS> <CMAKE_CXX_LINK_FLAGS> -Wl,-Map,<TARGET>.map <LINK_FLAGS> <OBJECTS> -o <TARGET> <LINK_LIBRARIES>")

# set the ARCHITECTURE for the framework
set(ARCHITECTURE "ARCH_CORTEXM33"
  CACHE STRING "ARCH_X86, ARCH_X86_64, ARCH_CORTEXM33" FORCE)

# set the directory name for the assembler optimizations
set(ASM_ARCH_DIR "cortexm0/asm" CACHE STRING "Name of the folder with the ASM optimizations." FORCE)
# Optimizations for m-33
set(ASM_ARCH_m33_DIR "cortexm33" CACHE STRING "Name of the folder with the m33 ASM optimizations." FORCE)

# startup code for Cortex M33
set(BOARD_SOURCES  "${SDK_RAW_METAL}/modules/nrfx/mdk/gcc_startup_nrf9160.S"
	)

set(BoardLibSources	       "${SDK_RAW_METAL}/modules/nrfx/mdk/system_nrf9160.c"	   
			       "src/arch/nrf9160/board_init.c" )

set_source_files_properties( "src/arch/nrf9160/board_init.c" PROPERTIES
			     COMPILE_FLAGS
			    "-I${SDK_RAW_METAL}/modules/nrfx			        \
			     -I${SDK_RAW_METAL}/modules/nrfx/hal			\
			     -I${SDK_RAW_METAL}/modules/nrfx/templates/nRF9160		\
			     -I${SDK_RAW_METAL}/modules/nrfx/drivers/include		\
			     -I${SDK_RAW_METAL}/components/libraries/util		\
			     -I${SDK_RAW_METAL}/components/drivers_nrf/nrf_soc_nosd	\
			     -I${SDK_RAW_METAL}/integration/nrfx			\
			     -I${SDK_RAW_METAL}/integration/nrfx/legacy			 "
 )

set_source_files_properties( "${SDK_RAW_METAL}/modules/nrfx/mdk/gcc_startup_nrf9160.S" PROPERTIES
			     COMPILE_FLAGS "-D__STARTUP_CLEAR_BSS -D__START=board_init" )

# extend the compile flags with parameters to obtain small code size
set(TOOLCHAIN_ASM_FLAGS	   "-std=c99 -mcpu=${CPU} -mthumb -ffunction-sections -fdata-sections")
set(TOOLCHAIN_C_FLAGS	   "-std=c99 -mcpu=${CPU} -mthumb -ffunction-sections -fdata-sections")
set(TOOLCHAIN_CXX_FLAGS	   "-std=c99 -mcpu=${CPU} -mthumb -ffunction-sections -fdata-sections")
set(TOOLCHAIN_LINKER_FLAGS "-Wl,-static -u _sbrk -Wl,-gc-sections -Wl,-lnosys	    \
			    -Wl,-L${SDK_RAW_METAL}/modules/nrfx/mdk		    \
			    -Wl,-L${CMAKE_SOURCE_DIR}/linker		\
			    -Wl,-Tcortexm33.ld				    \
			    -nostartfiles --specs=nano.specs			    ")
