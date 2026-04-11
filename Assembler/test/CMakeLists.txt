# This CMakeLists maps to Assembler/test/premake5.lua. Premake compiles a test
# executable, wires in doctest, and deliberately recompiles several library
# translation units inside the test target. The duplication is preserved here so
# the generated object set matches the original build as closely as possible.

add_executable(Tiny16-Assembler-Tests)

add_subdirectory(Dependencies/doctest)

file(GLOB tiny16_assembler_test_sources CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp"
)

target_sources(Tiny16-Assembler-Tests PRIVATE ${tiny16_assembler_test_sources})

target_include_directories(Tiny16-Assembler-Tests PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/../../Utility/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../lib/src"
)

target_include_directories(Tiny16-Assembler-Tests SYSTEM PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/doctest"
)

target_link_libraries(Tiny16-Assembler-Tests PRIVATE
    Tiny16-Assembler-Lib
    doctest::doctest
)

target_compile_definitions(Tiny16-Assembler-Tests PRIVATE _CRT_SECURE_NO_WARNINGS)
target_compile_features(Tiny16-Assembler-Tests PRIVATE cxx_std_20)

tiny16_apply_common_compiler_policy(Tiny16-Assembler-Tests)
tiny16_apply_cpp_premake_warnings(Tiny16-Assembler-Tests)
tiny16_set_output_layout(Tiny16-Assembler-Tests)
