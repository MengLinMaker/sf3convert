cmake_minimum_required(VERSION 3.20)
project(sf3convert)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Building static binary requires linking static libraries '.a' or'.lib' extension on windows
if(WIN32)
    set(CMAKE_FIND_LIBRARY_SUFFIXES ".lib")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /STACK:50000000")
else()
    set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
endif()

# Log how cmake finds libraries
set(CMAKE_FIND_DEBUG_MODE TRUE)

# Add source files
file(GLOB_RECURSE MY_SOURCES src/*.cpp)
add_executable(${PROJECT_NAME} ${MY_SOURCES})

# Autosearch lib dependencies
find_package(CLI11 REQUIRED)
find_package(Vorbis REQUIRED)
target_link_libraries(${PROJECT_NAME}
    CLI11::CLI11
    vorbis::vorbis
)
