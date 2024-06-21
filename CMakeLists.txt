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

CMAKE_MINIMUM_REQUIRED(VERSION 3.0.0)
project(sf3convert)

set(CMAKE_CXX_STANDARD 17)

# Allows deprecated API
cmake_policy(SET CMP0153 OLD)

# Add source files
file(GLOB MY_SOURCES src/*.cpp)
add_executable("${PROJECT_NAME}" ${MY_SOURCES})

# Include external Qt library
include_directories(/opt/homebrew/Cellar/qt/6.7.0_1/include)



include (${PROJECT_SOURCE_DIR}/build/UsePkgConfig1.cmake)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -fPIC -fPIE")

# libsndfile
PKGCONFIG1 (sndfile 1.0.25 SNDFILE_INCDIR SNDFILE_LIBDIR SNDFILE_LIB SNDFILE_CPP)
if (SNDFILE_INCDIR)
      message("libsndfile detected ${SNDFILE_INCDIR} ${SNDFILE_LIBDIR} ${SNDFILE_LIB}")
else (SNDFILE_INCDIR)
   message("libsndfile not found\n")
endif (SNDFILE_INCDIR)

## libvorbis
PKGCONFIG1 (vorbis 1.3.3 VORBIS_INCDIR VORBIS_LIBDIR VORBIS_LIB VORBIS_CPP)
if (VORBIS_INCDIR)
      message("libvorbis detected ${VORBIS_INCDIR} ${VORBIS_LIBDIR} ${VORBIS_LIB}")
else (VORBIS_INCDIR)
      message("libvorbis not found\n")
endif (VORBIS_INCDIR)

## libogg
PKGCONFIG1 (ogg 1.3.0 OGG_INCDIR OGG_LIBDIR OGG_LIB OGG_CPP)
if (OGG_INCDIR)
      message("libogg detected ${OGG_INCDIR} ${OGG_LIBDIR} ${OGG_LIB}")
else (OGG_INCDIR)
      message("libogg not found\n")
endif (OGG_INCDIR)

## Includes
include_directories(
   ${PROJECT_SOURCE_DIR}
   ${PROJECT_BINARY_DIR}
   ${SNDFILE_INCDIR}
   ${OGG_INCDIR}
   ${VORBIS_INCDIR}
)

target_link_libraries(sf3convert
   ${QT_LIBRARIES}
   ${OGG_LIB}
   ${VORBIS_LIB}
   vorbisenc
   ${SNDFILE_LIB}
)

install(TARGETS sf3convert RUNTIME DESTINATION bin)
install(FILES sf3convert.1 DESTINATION ${CMAKE_INSTALL_MANDIR}/man1 COMPONENT doc)
