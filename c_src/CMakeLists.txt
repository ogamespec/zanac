# CMakeLists.txt - build the Zanac C port with MSVC (Windows) or any CMake
# toolchain.  Mirrors c_src/Makefile.
#
#   cmake -S c_src -B c_src/build-msvc -G Ninja -DCMAKE_BUILD_TYPE=Release
#   cmake --build c_src/build-msvc
#
# or simply run c_src\build.bat (it finds vcvars64 and SDL3 by itself).
#
# Two executables are produced:
#   zanac      the C port (opens an SDL3 window when SDL3 is found, otherwise it
#              runs headless and only supports --frames/--shot/--audio/--hashes)
#   zanac_rom  the reference runner: executes the original 6502 ROM in the same
#              engine, so the port can be compared against it frame by frame
#
# Options:
#   -DZANAC_NO_SDL=ON   force the headless build
#   -DSDL3_DIR=<path>   SDL3 development files (headers + lib\x64\SDL3.lib)
cmake_minimum_required(VERSION 3.20)
project(zanac_port C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(ZANAC_ROOT "${CMAKE_CURRENT_SOURCE_DIR}")

# ---------------------------------------------------------------- generated --
# The port's headers, the extracted bank images, the thunks, the stubs and the
# address dispatcher are generated from the original assembly.  They are
# produced once at configure time (so that the source globs below see them) and
# again at build time when one of their inputs changes.
set(GEN_INPUTS
    "${ZANAC_ROOT}/../src/vars.asm"
    "${ZANAC_ROOT}/../src/bank4.asm" "${ZANAC_ROOT}/../src/bank5.asm"
    "${ZANAC_ROOT}/../src/bank6.asm" "${ZANAC_ROOT}/../src/bank7.asm"
    "${ZANAC_ROOT}/../Build/bank4.bin" "${ZANAC_ROOT}/../Build/bank5.bin"
    "${ZANAC_ROOT}/../Build/bank6.bin" "${ZANAC_ROOT}/../Build/bank7.bin"
    "${ZANAC_ROOT}/../Build/bank6.lst" "${ZANAC_ROOT}/../Build/bank7.lst")
find_package(Python3 COMPONENTS Interpreter REQUIRED)

execute_process(COMMAND ${Python3_EXECUTABLE} "${ZANAC_ROOT}/tools/gen_zanac.py"
                WORKING_DIRECTORY "${ZANAC_ROOT}" RESULT_VARIABLE GEN_RC)
if(NOT GEN_RC EQUAL 0)
    message(FATAL_ERROR "tools/gen_zanac.py failed")
endif()
execute_process(COMMAND ${Python3_EXECUTABLE} "${ZANAC_ROOT}/tools/gen_support.py"
                WORKING_DIRECTORY "${ZANAC_ROOT}" RESULT_VARIABLE GEN_RC)
if(NOT GEN_RC EQUAL 0)
    message(FATAL_ERROR "tools/gen_support.py failed")
endif()

add_custom_target(zanac_generated
    COMMAND ${Python3_EXECUTABLE} "${ZANAC_ROOT}/tools/gen_zanac.py"
    COMMAND ${Python3_EXECUTABLE} "${ZANAC_ROOT}/tools/gen_support.py"
    WORKING_DIRECTORY "${ZANAC_ROOT}"
    DEPENDS ${GEN_INPUTS}
    BYPRODUCTS
        "${ZANAC_ROOT}/zanac/vars.h" "${ZANAC_ROOT}/zanac/labels.h"
        "${ZANAC_ROOT}/zanac/labels_addr.h" "${ZANAC_ROOT}/zanac/rom_labels.h"
        "${ZANAC_ROOT}/zanac/rom/zanac_banks.c" "${ZANAC_ROOT}/zanac/rom/zanac_banks.h"
        "${ZANAC_ROOT}/zanac/thunks.c" "${ZANAC_ROOT}/zanac/stubs.c"
        "${ZANAC_ROOT}/zanac/zanac_dispatch.c"
    COMMENT "Generating the port's headers, bank images, thunks, stubs and dispatcher")

file(GLOB NESLIB_SRC CONFIGURE_DEPENDS
     "${ZANAC_ROOT}/neslib/*.c" "${ZANAC_ROOT}/neslib/core/*.c")
file(GLOB ZANAC_GAME_SRC CONFIGURE_DEPENDS
     "${ZANAC_ROOT}/zanac/*.c" "${ZANAC_ROOT}/zanac/bank6/*.c"
     "${ZANAC_ROOT}/zanac/bank7/*.c" "${ZANAC_ROOT}/zanac/rom/*.c")
file(GLOB FRONTEND_COMMON CONFIGURE_DEPENDS "${ZANAC_ROOT}/frontend/image.c"
     "${ZANAC_ROOT}/frontend/input_script.c")

# ------------------------------------------------------------------- SDL3 ----
set(SDL3_USED FALSE)
if(NOT ZANAC_NO_SDL)
    # The Nespeculator checkout lives next to this repository
    # (C:\Work\Nespeculator), so its vendored SDL3 is two levels up from
    # c_src; also accept one level up for a checkout inside the repository.
    if(NOT DEFINED SDL3_DIR)
        foreach(candidate
                "${ZANAC_ROOT}/../../Nespeculator/third_party/SDL3"
                "${ZANAC_ROOT}/../Nespeculator/third_party/SDL3"
                "${ZANAC_ROOT}/third_party/SDL3")
            if(EXISTS "${candidate}/include/SDL3/SDL.h")
                set(SDL3_DIR "${candidate}")
                break()
            endif()
        endforeach()
    endif()
    if(SDL3_DIR AND EXISTS "${SDL3_DIR}/include/SDL3/SDL.h" AND
       EXISTS "${SDL3_DIR}/lib/x64/SDL3.lib")
        set(SDL3_USED TRUE)
        set(SDL3_INCLUDE_DIR "${SDL3_DIR}/include")
        set(SDL3_LIBRARY "${SDL3_DIR}/lib/x64/SDL3.lib")
    else()
        find_package(SDL3 QUIET)
        if(SDL3_FOUND)
            set(SDL3_USED TRUE)
            set(SDL3_INCLUDE_DIR "${SDL3_INCLUDE_DIRS}")
            set(SDL3_LIBRARY SDL3::SDL3)
        endif()
    endif()
endif()

if(SDL3_USED)
    message(STATUS "SDL3 frontend enabled (${SDL3_DIR})")
    set(FRONTEND_SRC "${ZANAC_ROOT}/frontend/sdl.c")
else()
    message(STATUS "Building the headless frontend (no SDL3)")
    set(FRONTEND_SRC "${ZANAC_ROOT}/frontend/headless.c")
endif()

# ------------------------------------------------------------------ targets --
# zanac: the engine + the ported game + the frontend + the host entry point.
add_executable(zanac ${NESLIB_SRC} ${ZANAC_GAME_SRC} ${FRONTEND_COMMON} ${FRONTEND_SRC}
               "${ZANAC_ROOT}/zanac/main.c")
add_dependencies(zanac zanac_generated)
target_include_directories(zanac PRIVATE "${ZANAC_ROOT}" "${ZANAC_ROOT}/zanac"
                          "${ZANAC_ROOT}/neslib" "${ZANAC_ROOT}/frontend")

# Windows: embed zanac.ico into the executable.  rc.exe resolves the INCLUDE
# against its working directory, so the path is substituted into a generated
# .rc instead of being written relative to the source.  The SDL window/taskbar
# icon is set at run time from frontend/icon.h (the same picture).
if(WIN32)
    set(ZANAC_ICON_PATH "${ZANAC_ROOT}/zanac.ico")
    configure_file("${ZANAC_ROOT}/zanac.rc.in"
                   "${CMAKE_CURRENT_BINARY_DIR}/zanac.rc" @ONLY)
    target_sources(zanac PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/zanac.rc")
    # Re-run rc.exe when the icon itself is regenerated (the .rc only names it,
    # so its text does not change).
    set_property(SOURCE "${CMAKE_CURRENT_BINARY_DIR}/zanac.rc"
                 APPEND PROPERTY OBJECT_DEPENDS "${ZANAC_ICON_PATH}")
endif()

if(SDL3_USED)
    target_include_directories(zanac PRIVATE "${SDL3_INCLUDE_DIR}")
    target_link_libraries(zanac PRIVATE "${SDL3_LIBRARY}")
    if(WIN32 AND EXISTS "${SDL3_DIR}/lib/x64/SDL3.dll")
        add_custom_command(TARGET zanac POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    "${SDL3_DIR}/lib/x64/SDL3.dll" "$<TARGET_FILE_DIR:zanac>")
    endif()
endif()

# zanac_rom: the engine + the extracted bank image + the original 6502 ROM
# runner (the reference the port is verified against).
set(REF_SRC "${NESLIB_SRC}" "${ZANAC_ROOT}/zanac/rom/zanac_banks.c"
            ${FRONTEND_COMMON} "${ZANAC_ROOT}/reference/run_rom.c")
add_executable(zanac_rom ${REF_SRC})
add_dependencies(zanac_rom zanac_generated)
target_include_directories(zanac_rom PRIVATE "${ZANAC_ROOT}" "${ZANAC_ROOT}/zanac"
                           "${ZANAC_ROOT}/neslib" "${ZANAC_ROOT}/frontend")

if(MSVC)
    # One option per argument: with a single string CMake may join them into
    # "/W3/wd4996", which cl.exe rejects.
    foreach(t zanac zanac_rom)
        target_compile_options(${t} PRIVATE "/W3" "/wd4996" "/utf-8" "/MP")
        target_compile_definitions(${t} PRIVATE _CRT_SECURE_NO_WARNINGS)
    endforeach()
else()
    foreach(t zanac zanac_rom)
        # -Wno-infinite-recursion: the $BC5E backward branch of sub_1BC04
        # (bank6/BB56.c) is ported as a call to the procedure's own entry, the
        # idiom PORTING.md section 5 prescribes for a branch into a procedure;
        # the loop is bounded by DEC byte_6F (byte_6F is loaded with 3).
        target_compile_options(${t} PRIVATE -Wall -Wextra -Wno-unused-parameter
                               -Wno-unused-const-variable -Wno-unused-function
                               -Wno-infinite-recursion)
    endforeach()
endif()
