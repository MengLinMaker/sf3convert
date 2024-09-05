#=============================================================================
#  MuseScore sftools
#
#  Copyright (C) 2011-2014 Werner Schweer && Meng Lin
#
#  This work is free software; you can redistribute it and/or
#  modify it under the terms of the GNU Library General Public
#  License as published by the Free Software Foundation; either
#  version 2 of the License, or (at your option) any later version.
#
#  See LICENCE for the licence text and disclaimer of warranty.
#=============================================================================

cmake_minimum_required(VERSION 3.5)
project(sf3convert)

# Add source files, targeting C++ 17
file(GLOB MY_SOURCES src/*.cpp)
add_executable(${PROJECT_NAME} ${MY_SOURCES})
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)



# Include Qt library via autosearch
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Xml)

# Include lib files
target_link_libraries(${PROJECT_NAME}
    Qt6::Core
    Qt6::Widgets
    Qt6::Xml
)



# Autosearch lib dependencies
find_package(PkgConfig REQUIRED)

# libsndfile
pkg_check_modules(SNDFILE REQUIRED IMPORTED_TARGET sndfile>=1.0.25)

# libvorbis
pkg_check_modules(VORBIS REQUIRED IMPORTED_TARGET vorbis>=1.3.3)

# libogg
pkg_check_modules(OGG REQUIRED IMPORTED_TARGET ogg>=1.3.0)

# Include lib files
target_link_libraries(${PROJECT_NAME}
    ${OGG_LDFLAGS}
    ${VORBIS_LDFLAGS}
    vorbisenc
    ${SNDFILE_LDFLAGS}
)
