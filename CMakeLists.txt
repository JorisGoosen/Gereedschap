cmake_minimum_required(VERSION 3.20)
project(Gereedschap VERSION 1.0.0 LANGUAGES CXX)

# macOS arm64 only
if(APPLE)
    set(CMAKE_OSX_ARCHITECTURES "arm64" CACHE STRING "Build for Apple Silicon" FORCE)
    set(CMAKE_OSX_DEPLOYMENT_TARGET "26.0" CACHE STRING "Minimum macOS version" FORCE)
    enable_language(OBJCXX)
endif()

# Dependencies
find_package(PkgConfig REQUIRED)
pkg_check_modules(GLFW3 REQUIRED glfw3)
pkg_check_modules(LIBPNG REQUIRED libpng)
pkg_check_modules(GLM REQUIRED glm)

# wgpu-native (geen .pc bestand, dus zelf zoeken; homebrew zet het in /opt/homebrew/opt/wgpu-native)
execute_process(COMMAND brew --prefix wgpu-native
    OUTPUT_VARIABLE WGPU_PREFIX OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_QUIET)
if(NOT WGPU_PREFIX)
    set(WGPU_PREFIX "/opt/homebrew/opt/wgpu-native")
endif()
find_path(WGPU_INCLUDE_DIR webgpu.h
    HINTS ${WGPU_PREFIX}/include /opt/homebrew/include /usr/local/include)
find_library(WGPU_LIBRARY NAMES wgpu_native
    HINTS ${WGPU_PREFIX}/lib /opt/homebrew/lib /usr/local/lib)

if(NOT WGPU_INCLUDE_DIR OR NOT WGPU_LIBRARY)
    message(FATAL_ERROR "wgpu-native niet gevonden! Installeer met: brew install wgpu-native")
endif()
message(STATUS "wgpu-native gevonden: ${WGPU_LIBRARY}")

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${GLFW3_INCLUDE_DIRS})
include_directories(${LIBPNG_INCLUDE_DIRS})
include_directories(${GLM_INCLUDE_DIRS})
include_directories(${WGPU_INCLUDE_DIR})

# Source files
set(GEREEDSCHAP_SOURCES
    weergaveScherm.cpp
    weergaveSchermVierkant.cpp
    weergaveSchermPerspectief.cpp
    helpers.cpp
    perlinRuis.cpp
    nepScherm.cpp
    metaalLaag.mm
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
target_compile_features(gereedschap PUBLIC cxx_std_20)

# Target include directories
target_include_directories(gereedschap PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${GLFW3_INCLUDE_DIRS}
    ${LIBPNG_INCLUDE_DIRS}
    ${GLM_INCLUDE_DIRS}
    ${WGPU_INCLUDE_DIR}
)

# Link libraries
target_link_directories(gereedschap PUBLIC
    ${GLFW3_LIBRARY_DIRS}
    ${LIBPNG_LIBRARY_DIRS}
    ${WGPU_PREFIX}/lib
)
target_link_libraries(gereedschap PUBLIC
    ${GLFW3_LIBRARIES}
    ${LIBPNG_LIBRARIES}
    ${WGPU_LIBRARY}
)

# macOS frameworks (voor de CAMetalLayer hulp in metaalLaag.mm)
if(APPLE)
    target_link_libraries(gereedschap PUBLIC
        "-framework Cocoa"
        "-framework QuartzCore"
        "-framework Metal"
    )
endif()

# Compiler flags
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options(gereedschap PRIVATE
        -Wall
        -Wextra
        -Wpedantic
        -Werror=return-type
    )
endif()

# Demos
option(GEREEDSCHAP_BUILD_DEMOS "Bouw de demo-programma's" OFF)
if(GEREEDSCHAP_BUILD_DEMOS)
    set(GEREEDSCHAP_DEMO_SOURCES
        demos/quadDemo.cpp
        demos/perspectiefDemo.cpp
        demos/geoDemo.cpp
        demos/geometryDemo.cpp
        demos/landDemo.cpp
        demos/marcheerDemo.cpp
        demos/computeDemo.cpp
    )

    foreach(DEMO ${GEREEDSCHAP_DEMO_SOURCES})
        get_filename_component(DEMO_NAAM ${DEMO} NAME_WE)
        add_executable(${DEMO_NAAM} ${DEMO})
        target_link_libraries(${DEMO_NAAM} PRIVATE gereedschap)
    endforeach()
endif()