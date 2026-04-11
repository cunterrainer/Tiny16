# This CMakeLists mirrors Assembler/premake5.lua. The Premake project is tiny:
# it only builds the command-line assembler executable and links the shared
# assembler library. The CMake version keeps that same shape so the generated
# target graph stays easy to compare.

add_executable(Tiny16-Assembler)

file(GLOB tiny16_assembler_sources CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp"
)

target_sources(Tiny16-Assembler PRIVATE ${tiny16_assembler_sources})

target_include_directories(Tiny16-Assembler PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/src"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Utility/include"
)

target_link_libraries(Tiny16-Assembler PRIVATE Tiny16-Assembler-Lib)

target_compile_definitions(Tiny16-Assembler PRIVATE _CRT_SECURE_NO_WARNINGS)
target_compile_features(Tiny16-Assembler PRIVATE cxx_std_20)

tiny16_apply_common_compiler_policy(Tiny16-Assembler)
tiny16_apply_cpp_premake_warnings(Tiny16-Assembler)
tiny16_set_output_layout(Tiny16-Assembler)
