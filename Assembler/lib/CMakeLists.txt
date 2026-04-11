# This file corresponds to Assembler/lib/premake5.lua. It is the core static
# library used by both the assembler executable and the assembler tests.

add_library(Tiny16-Assembler-Lib STATIC)

file(GLOB tiny16_assembler_lib_sources CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp"
)

target_sources(Tiny16-Assembler-Lib PRIVATE ${tiny16_assembler_lib_sources})

target_include_directories(Tiny16-Assembler-Lib PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/../../Utility/include"
)

target_compile_definitions(Tiny16-Assembler-Lib PRIVATE _CRT_SECURE_NO_WARNINGS)
target_compile_features(Tiny16-Assembler-Lib PRIVATE cxx_std_20)

tiny16_apply_common_compiler_policy(Tiny16-Assembler-Lib)
tiny16_apply_cpp_premake_warnings(Tiny16-Assembler-Lib)
tiny16_set_output_layout(Tiny16-Assembler-Lib)
