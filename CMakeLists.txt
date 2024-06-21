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

CMAKE_MINIMUM_REQUIRED(VERSION 3.5)
project(sf3convert)

# Allows deprecated API
cmake_policy(SET CMP0153 OLD)

# Add source files, targeting C++ 17
file(GLOB MY_SOURCES src/*.cpp src/*.h)
add_executable(${PROJECT_NAME} ${MY_SOURCES})
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)



# Include Qt library via autosearch
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Xml)
target_link_libraries(
      ${PROJECT_NAME}
      Qt6::Core
      Qt6::Widgets
      Qt6::Xml
)



# Search for lib dependencies
include (${PROJECT_SOURCE_DIR}/src/FindLib.cmake)

# libsndfile
PKGCONFIG1 (sndfile 1.0.25 SNDFILE_INCDIR SNDFILE_LIBDIR SNDFILE_LIB SNDFILE_CPP)
if (SNDFILE_INCDIR)
      message("\nlibsndfile detected ${SNDFILE_INCDIR} ${SNDFILE_LIBDIR} ${SNDFILE_LIB} ${SNDFILE_CPP}\n")
else (SNDFILE_INCDIR)
      message("libsndfile not found\n")
endif (SNDFILE_INCDIR)

## libvorbis
PKGCONFIG1 (vorbis 1.3.3 VORBIS_INCDIR VORBIS_LIBDIR VORBIS_LIB VORBIS_CPP)
if (VORBIS_INCDIR)
      message("\nlibvorbis detected ${VORBIS_INCDIR} ${VORBIS_LIBDIR} ${VORBIS_LIB} ${VORBIS_CPP}\n")
else (VORBIS_INCDIR)
      message("libvorbis not found\n")
endif (VORBIS_INCDIR)

## libogg
PKGCONFIG1 (ogg 1.3.0 OGG_INCDIR OGG_LIBDIR OGG_LIB OGG_CPP)
if (OGG_INCDIR)
      message("\nlibogg detected ${OGG_INCDIR} ${OGG_LIBDIR} ${OGG_LIB} ${OGG_CPP}\n")
else (OGG_INCDIR)
      message("libogg not found\n")
endif (OGG_INCDIR)

## Include lib files
include_directories(
      ${PROJECT_BINARY_DIR}
      ${SNDFILE_INCDIR}
      ${OGG_INCDIR}
      ${VORBIS_INCDIR}
)
target_link_libraries(${PROJECT_NAME}
      ${OGG_LIB}
      ${VORBIS_LIB}
      vorbisenc
      ${SNDFILE_LIB}
)



# Create binary with project name
include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
