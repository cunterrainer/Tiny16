cmake_minimum_required(VERSION 3.24)

cmake_policy(SET CMP0069 NEW)

# This runtime choice needs to be visible before any project() call so the MSVC
# generator picks it up for every target, matching Premake's `staticruntime`.
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>" CACHE STRING "" FORCE)

# Tiny16 is a mixed C and C++ workspace, but the Premake files split the real
# build logic across several nested projects. This root CMake file mirrors that
# shape: it establishes the shared policy and output layout, then delegates each
# target to a focused CMakeLists close to the original Premake file it replaces.

project(Tiny16 VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Premake used `staticruntime "on"`, which maps to the static MSVC runtime.
# CMake exposes the same choice through this cache variable. Set it here so it
# applies consistently to every target configured from this tree.
set(CMAKE_CONFIGURATION_TYPES Debug;Release;Distribution;MinSizeDistribution CACHE STRING "" FORCE)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(Tiny16Common)

# Premake computed its output root from the current working directory. The CMake
# equivalent is the source directory, because the build tree can be anywhere.
set(TINY16_OUTPUT_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/BIN")
tiny16_detect_toolset(TINY16_TOOLSET_NAME)

# Premake's top-level `filter "system:emscripten"` changes the executable suffix
# to `.html`. CMake does not do that automatically, so the helper keeps the same
# behavior for every executable target when building with an Emscripten toolchain.
if(CMAKE_SYSTEM_NAME STREQUAL "Emscripten")
    set(TINY16_EXECUTABLE_SUFFIX ".html")
else()
    set(TINY16_EXECUTABLE_SUFFIX "")
endif()

# The dependency order matches the Premake workspace and keeps the comparison
# against the old build files straightforward.
add_subdirectory(Dependencies/raylib)
add_subdirectory(Dependencies/ImGui)
add_subdirectory(Dependencies/raygui)
add_subdirectory(Dependencies/rlImGui)
add_subdirectory(Dependencies/ImGuiColorTextEdit)

tiny16_apply_common_compiler_policy(raylib)
tiny16_silence_warnings(raylib)
tiny16_set_output_layout(raylib)

add_subdirectory(Assembler/lib)
add_subdirectory(Assembler)
add_subdirectory(Assembler/test)
add_subdirectory(Emulator)
