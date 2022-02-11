cmake_minimum_required(VERSION 3.4.1)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}/libs/z8lua
        ${CMAKE_CURRENT_SOURCE_DIR}/libs/lodepng
        ${CMAKE_CURRENT_SOURCE_DIR}/libs/simpleini

        ${CMAKE_CURRENT_SOURCE_DIR}/../SDL2/include

        # SDL_internal.h is located in src required by SDL_android_main.c bridge
        ${CMAKE_CURRENT_SOURCE_DIR}/../SDL2/src

        ${CMAKE_CURRENT_SOURCE_DIR}/../SDL2_image/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../SDL2_gfx/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../SDL2_mixer/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../SDL2_ttf/include
)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../SDL2/ ${CMAKE_CURRENT_SOURCE_DIR}/../SDL2)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../SDL2_image/ ${CMAKE_CURRENT_SOURCE_DIR}/../SDL2_image)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../SDL2_gfx/ ${CMAKE_CURRENT_SOURCE_DIR}/../SDL2_gfx)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../SDL2_mixer/ ${CMAKE_CURRENT_SOURCE_DIR}/../SDL2_mixer)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../SDL2_ttf/ ${CMAKE_CURRENT_SOURCE_DIR}/../SDL2_ttf)


add_definitions(-Wno-deprecated)
remove_definitions(-Werror)

add_library(
             main-lib

             SHARED

             # Provides a relative path to your source file(s).
             source/SDL_android_main.c
             source/main.cpp
             source/Audio.cpp
             source/cart.cpp
             source/emojiconversion.cpp
             source/filehelpers.cpp
             source/fontdata.cpp
             source/graphics.cpp
             source/hostCommonFunctions.cpp
             source/Input.cpp
             source/logger.cpp
             source/mathhelpers.cpp
             source/nibblehelpers.cpp
             source/picoluaapi.cpp
             source/stringToDataHelpers.cpp
             source/synth.cpp
             source/vm.cpp
             platform/SDL2Common/source/sdl2basehost.cpp
             platform/android/source/AndroidHost.cpp
             libs/lodepng/lodepng.cpp
             libs/simpleini/ConvertUTF.c
             libs/z8lua/eris.c
             libs/z8lua/lapi.c
             libs/z8lua/lauxlib.c
             libs/z8lua/lbaselib.c
             libs/z8lua/lbitlib.c
             libs/z8lua/lcode.c
             libs/z8lua/lcorolib.c
             libs/z8lua/lctype.c
             libs/z8lua/ldblib.c
             libs/z8lua/ldebug.c
             libs/z8lua/ldo.c
             libs/z8lua/ldump.c
             libs/z8lua/lfunc.c
             libs/z8lua/lgc.c
             libs/z8lua/linit.c
             libs/z8lua/liolib.c
             libs/z8lua/llex.c
             libs/z8lua/lmem.c
             libs/z8lua/loadlib.c
             libs/z8lua/lobject.c
             libs/z8lua/lopcodes.c
             libs/z8lua/loslib.c
             libs/z8lua/lparser.c
             libs/z8lua/lpico8lib.c
             libs/z8lua/lstate.c
             libs/z8lua/lstring.c
             libs/z8lua/lstrlib.c
             libs/z8lua/ltable.c
             libs/z8lua/ltablib.c
             libs/z8lua/ltests.c
             libs/z8lua/ltm.c
             libs/z8lua/lundump.c
             libs/z8lua/lvm.c
             libs/z8lua/lzio.c)

set_source_files_properties(
        libs/z8lua/eris.c
        libs/z8lua/lapi.c
        libs/z8lua/lauxlib.c
        libs/z8lua/lbaselib.c
        libs/z8lua/lbitlib.c
        libs/z8lua/lcode.c
        libs/z8lua/lcorolib.c
        libs/z8lua/lctype.c
        libs/z8lua/ldblib.c
        libs/z8lua/ldebug.c
        libs/z8lua/ldo.c
        libs/z8lua/ldump.c
        libs/z8lua/lfunc.c
        libs/z8lua/lgc.c
        libs/z8lua/linit.c
        libs/z8lua/liolib.c
        libs/z8lua/llex.c
        libs/z8lua/lmem.c
        libs/z8lua/loadlib.c
        libs/z8lua/lobject.c
        libs/z8lua/lopcodes.c
        libs/z8lua/loslib.c
        libs/z8lua/lparser.c
        libs/z8lua/lpico8lib.c
        libs/z8lua/lstate.c
        libs/z8lua/lstring.c
        libs/z8lua/lstrlib.c
        libs/z8lua/ltable.c
        libs/z8lua/ltablib.c
        libs/z8lua/ltests.c
        libs/z8lua/ltm.c
        libs/z8lua/lundump.c
        libs/z8lua/lvm.c
        libs/z8lua/lzio.c
        PROPERTIES LANGUAGE CXX )


find_library( # Sets the name of the path variable.
              log-lib

              # Specifies the name of the NDK library that
              # you want CMake to locate.
              log )



target_link_libraries( # Specifies the target library.
                        main-lib
                        SDL2
                        SDL2_image
                        SDL2_gfx
                        SDL2_mixer
                        SDL2_ttf

                        ${log-lib} )
