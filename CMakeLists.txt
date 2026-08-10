cmake_minimum_required(VERSION 3.20)
project(Gereedschap VERSION 1.0.0 LANGUAGES CXX)

# Set C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# macOS arm64 only
if(APPLE)
    set(CMAKE_OSX_ARCHITECTURES "arm64" CACHE STRING "Build for Apple Silicon" FORCE)
    set(CMAKE_OSX_DEPLOYMENT_TARGET "11.0" CACHE STRING "Minimum macOS version" FORCE)
endif()

# Dependencies
find_package(PkgConfig REQUIRED)
pkg_check_modules(GLFW3 REQUIRED glfw3)
pkg_check_modules(GLEW REQUIRED glew)
pkg_check_modules(LIBPNG REQUIRED libpng)
pkg_check_modules(GLM REQUIRED glm)
find_package(OpenGL REQUIRED)

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${GLFW3_INCLUDE_DIRS})
include_directories(${GLEW_INCLUDE_DIRS})
include_directories(${LIBPNG_INCLUDE_DIRS})
include_directories(${GLM_INCLUDE_DIRS})

# Source files
set(GEREEDSCHAP_SOURCES
    weergaveScherm.cpp
    weergaveSchermVierkant.cpp
    weergaveSchermPerspectief.cpp
    helpers.cpp
    perlinRuis.cpp
    nepScherm.cpp
)

# Geometry sources
set(GEOMETRY_SOURCES
    geometrie/icosahedron.cpp
    geometrie/geodesisch.cpp
    geometrie/vierkantRooster.cpp
)

# Subjecten sources
set(SUBJECTEN_SOURCES
    subjecten/wolfSchaap.cpp
)

# All sources
set(ALL_SOURCES
    ${GEREEDSCHAP_SOURCES}
    ${GEOMETRY_SOURCES}
    ${SUBJECTEN_SOURCES}
)

# Static library (as requested)
add_library(gereedschap STATIC ${ALL_SOURCES})

# Target include directories
target_include_directories(gereedschap PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${GLFW3_INCLUDE_DIRS}
    ${GLEW_INCLUDE_DIRS}
    ${LIBPNG_INCLUDE_DIRS}
    ${GLM_INCLUDE_DIRS}
)

# Link libraries (no OpenGL here - add in subdirectories where needed)
target_link_libraries(gereedschap PUBLIC
    ${GLFW3_LIBRARIES}
    ${GLEW_LIBRARIES}
    ${LIBPNG_LIBRARIES}
)

# Compile definitions
target_compile_definitions(gereedschap PRIVATE
    _GLFWHAVE_CONTEXT_CREATION_API
)

# Compiler flags
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options(gereedschap PRIVATE
        -Wall
        -Wextra
        -Wpedantic
        -Werror=return-type
    )
endif()

# Shader preprocessing (CMake-based includes)
# This will process #include "..." in shader files at build time
# For now, keep runtime regex processing for compatibility
