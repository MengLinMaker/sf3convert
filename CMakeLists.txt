cmake_minimum_required(VERSION 3.20)
project(sf3convert)

# Compile to static binary
set(BUILD_SHARED_LIBS OFF)
set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
set(CMAKE_MFC_FLAG 1)

# Add source files, targeting C++ 23
file(GLOB_RECURSE MY_SOURCES src/*.cpp)
add_executable(${PROJECT_NAME} ${MY_SOURCES})
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_23)



# Autosearch lib dependencies with hint for Windows
find_package(Vorbis REQUIRED PATHS "C:/Users/runneradmin/.conan2")
find_package(Ogg REQUIRED PATHS "C:/Users/runneradmin/.conan2")
target_link_libraries(${PROJECT_NAME}
    vorbis::vorbis
    Ogg::ogg
)
