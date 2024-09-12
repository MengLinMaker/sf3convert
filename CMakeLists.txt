cmake_minimum_required(VERSION 3.20)
project(sf3convert)

# Compile to static binary
set(BUILD_SHARED_LIBS OFF)
set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")

# Add source files, targeting C++ 23
file(GLOB_RECURSE MY_SOURCES src/*.cpp)
add_executable(${PROJECT_NAME} ${MY_SOURCES})
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_23)

# Autosearch lib dependencies
set(CMAKE_FIND_DEBUG_MODE TRUE)
find_package(Vorbis REQUIRED)
target_link_libraries(${PROJECT_NAME}
    vorbis::vorbis
)
