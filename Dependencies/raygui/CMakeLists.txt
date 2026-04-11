# This file ports Dependencies/raygui/premake5.lua. raygui is a tiny C static
# library that wraps the raylib headers, so the CMake translation is mostly a
# direct source glob plus the same include directory and warning behavior.

add_library(raygui STATIC)

file(GLOB tiny16_raygui_sources CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
)

target_sources(raygui PRIVATE ${tiny16_raygui_sources})

target_include_directories(raygui PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/../raylib/src"
)

if(WIN32)
    target_compile_definitions(raygui PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()

tiny16_apply_common_compiler_policy(raygui)
tiny16_apply_c_premake_warnings(raygui)
tiny16_set_output_layout(raygui)
