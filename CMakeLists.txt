cmake_minimum_required(VERSION 3.20)
project(Gereedschap VERSION 1.0.0 LANGUAGES CXX)

# ── Platform-detectie ────────────────────────────────────────────────────────

set(IS_EMSCRIPTEN FALSE)
if(EMSCRIPTEN)
    set(IS_EMSCRIPTEN TRUE)
endif()

# macOS-specifiek (OBJCXX nodig voor metaalLaag.mm)
if(APPLE AND NOT IS_EMSCRIPTEN)
    enable_language(OBJCXX)
    if(NOT CMAKE_OSX_DEPLOYMENT_TARGET)
        set(CMAKE_OSX_DEPLOYMENT_TARGET "11.0" CACHE STRING "Minimum macOS version" FORCE)
    endif()
endif()

# ── Dependencies ─────────────────────────────────────────────────────────────

find_package(PkgConfig QUIET)

# ── GLFW3 (niet nodig op web) ───────────────────────────────────────────────

set(GLFW3_FOUND FALSE)
if(NOT IS_EMSCRIPTEN)
    if(PkgConfig_FOUND)
        pkg_check_modules(GLFW3 glfw3)
    endif()
    if(NOT GLFW3_FOUND)
        find_path(GLFW3_INCLUDE_DIR NAMES glfw3.h PATHS /opt/homebrew/include /usr/local/include ${VCPKG_INSTALLATION_PREFIX}/include)
        find_library(GLFW3_LIBRARY NAMES glfw3 glfw PATHS /opt/homebrew/lib /usr/local/lib ${VCPKG_INSTALLATION_PREFIX}/lib)
        if(GLFW3_INCLUDE_DIR AND GLFW3_LIBRARY)
            set(GLFW3_FOUND TRUE)
            set(GLFW3_INCLUDE_DIRS ${GLFW3_INCLUDE_DIR})
            set(GLFW3_LIBRARY_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/../build/src)
            set(GLFW3_LIBRARIES ${GLFW3_LIBRARY})
        endif()
    endif()
endif()

# ── libpng ───────────────────────────────────────────────────────────────────

set(LIBPNG_FOUND FALSE)
if(IS_EMSCRIPTEN)
    # Emscripten heeft内置 libpng ondersteuning via -sUSE_LIBPNG=1
    set(LIBPNG_FOUND TRUE)
    set(LIBPNG_INCLUDE_DIRS "")
    set(LIBPNG_LIBRARIES "")
else()
    if(PkgConfig_FOUND)
        pkg_check_modules(LIBPNG libpng)
    endif()
    if(NOT LIBPNG_FOUND)
        find_path(LIBPNG_INCLUDE_DIR NAMES png.h PATHS /opt/homebrew/include /usr/local/include ${VCPKG_INSTALLATION_PREFIX}/include)
        find_library(LIBPNG_LIBRARY NAMES png PATHS /opt/homebrew/lib /usr/local/lib ${VCPKG_INSTALLATION_PREFIX}/lib)
        if(LIBPNG_INCLUDE_DIR AND LIBPNG_LIBRARY)
            set(LIBPNG_FOUND TRUE)
            set(LIBPNG_INCLUDE_DIRS ${LIBPNG_INCLUDE_DIR})
            set(LIBPNG_LIBRARY_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/../build/src)
            set(LIBPNG_LIBRARIES ${LIBPNG_LIBRARY})
        endif()
    endif()
endif()

# ── glm (header-only) ───────────────────────────────────────────────────────

set(GLM_FOUND FALSE)
if(IS_EMSCRIPTEN)
    # Voor web-build: gebruik vaste pads (homebrew headers)
    set(GLM_INCLUDE_DIRS "/opt/homebrew/include")
    set(LIBPNG_INCLUDE_DIRS "/opt/homebrew/include")
    set(GLM_FOUND TRUE)
    set(LIBPNG_FOUND TRUE)
    message(STATUS "Web-build: glm en libpng gevonden in /opt/homebrew/include")
else()
    if(PkgConfig_FOUND)
        pkg_check_modules(GLM glm)
    endif()
    if(NOT GLM_FOUND)
        find_path(GLM_INCLUDE_DIR NAMES glm/glm.hpp PATHS /opt/homebrew/include /usr/local/include ${VCPKG_INSTALLATION_PREFIX}/include)
        if(GLM_INCLUDE_DIR)
            set(GLM_FOUND TRUE)
            set(GLM_INCLUDE_DIRS ${GLM_INCLUDE_DIR})
        endif()
    endif()
endif()

# ── Controleer dependencies (niet-web) ───────────────────────────────────────

if(NOT IS_EMSCRIPTEN)
    if(NOT GLFW3_FOUND OR NOT LIBPNG_FOUND OR NOT GLM_FOUND)
        message(FATAL_ERROR "Afhankelijkheden niet gevonden! Installeer met:\n"
                            "  macOS:   brew install glfw libpng glm\n"
                            "  Linux:   sudo apt install libglfw3-dev libpng-dev\n"
                            "  Windows: vcpkg install glfw3 libpng glm")
    endif()
else()
    if(NOT LIBPNG_FOUND OR NOT GLM_FOUND)
        message(FATAL_ERROR "Afhankelijkheden niet gevonden! Zorg dat libpng en glm beschikbaar zijn.")
    endif()
endif()

# ── wgpu-native (niet nodig op web) ─────────────────────────────────────────

set(WGPU_INCLUDE_DIR "")
set(WGPU_LIBRARY "")

if(NOT IS_EMSCRIPTEN)
    set(WGPU_INCLUDE_DIR "/opt/homebrew/include")
    set(WGPU_LIBRARY "/opt/homebrew/lib/libwgpu_native.dylib")

    if(NOT EXISTS "${WGPU_INCLUDE_DIR}/webgpu.h" OR NOT EXISTS "${WGPU_LIBRARY}")
        find_path(WGPU_INCLUDE_DIR webgpu.h PATHS /opt/homebrew/include /usr/local/include)
        find_library(WGPU_LIBRARY NAMES wgpu_native wgpu PATHS /opt/homebrew/lib /usr/local/lib)
    endif()

    if(NOT WGPU_INCLUDE_DIR OR NOT WGPU_LIBRARY)
        message(FATAL_ERROR "wgpu-native niet gevonden!\n"
                            "  macOS:   brew install wgpu-native\n"
                            "  Linux:   download van https://github.com/gfx-rs/wgpu-native/releases\n"
                            "  Windows: download van https://github.com/gfx-rs/wgpu-native/releases")
    endif()

    message(STATUS "wgpu-native gevonden: ${WGPU_LIBRARY} (${WGPU_INCLUDE_DIR})")
else()
    message(STATUS "Web-build: gebruik browser-native WebGPU (geen wgpu-native)")
endif()

# ── Bronbestanden ────────────────────────────────────────────────────────────

set(GEREEDSCHAP_SOURCES
    weergaveScherm.cpp
    weergaveSchermVierkant.cpp
    weergaveSchermPerspectief.cpp
    helpers.cpp
    perlinRuis.cpp
    nepScherm.cpp
)

# metaalLaag.mm alleen op native macOS (niet web, niet Windows/Linux)
if(APPLE AND NOT IS_EMSCRIPTEN)
    list(APPEND GEREEDSCHAP_SOURCES metaalLaag.mm)
endif()

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

# Alle bronnen
set(ALL_SOURCES
    ${GEREEDSCHAP_SOURCES}
    ${GEOMETRY_SOURCES}
    ${SUBJECTEN_SOURCES}
)

# Emscripten-webbackend (alleen bij Web-build)
if(IS_EMSCRIPTEN)
    list(APPEND ALL_SOURCES emscriptenWebBackend.cpp)
endif()

# Static library
add_library(gereedschap STATIC ${ALL_SOURCES})
target_compile_features(gereedschap PUBLIC cxx_std_20)

# Include directories
target_include_directories(gereedschap PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${GLFW3_INCLUDE_DIRS}
    ${LIBPNG_INCLUDE_DIRS}
    ${GLM_INCLUDE_DIRS}
    ${WGPU_INCLUDE_DIR}
)

# Link libraries (alleen native)
if(NOT IS_EMSCRIPTEN)
    target_link_directories(gereedschap PUBLIC ${GLFW3_LIBRARY_DIRS} ${LIBPNG_LIBRARY_DIRS})
    target_link_libraries(gereedschap PUBLIC
        ${GLFW3_LIBRARIES}
        ${LIBPNG_LIBRARIES}
        ${WGPU_LIBRARY}
    )
endif()

# macOS frameworks (voor CAMetalLayer in metaalLaag.mm)
if(APPLE AND NOT IS_EMSCRIPTEN)
    target_link_libraries(gereedschap PUBLIC
        "-framework Cocoa"
        "-framework QuartzCore"
        "-framework Metal"
    )
    # ARC voor .mm-bestanden
    set_source_files_properties(metaalLaag.mm PROPERTIES COMPILE_FLAGS "-fobjc-arc")
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
