# This is the CMake counterpart to Dependencies/ImGui/premake5.lua. Premake
# builds ImGui as a quiet static library from the core .cpp files in this
# directory. The CMake port keeps that shape and keeps the source selection
# obvious by listing the same top-level files.

add_library(ImGui STATIC)

file(GLOB tiny16_imgui_sources CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
)

target_sources(ImGui PRIVATE ${tiny16_imgui_sources})

tiny16_apply_common_compiler_policy(ImGui)
target_compile_features(ImGui PRIVATE cxx_std_20)
tiny16_silence_warnings(ImGui)
tiny16_set_output_layout(ImGui)
