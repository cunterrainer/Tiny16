# This mirrors Dependencies/rlImGui/premake5.lua. The noteworthy detail here is
# that Premake intentionally sets C++17 for this target, even though the rest of
# Tiny16 uses C++20. That asymmetry is preserved so the resulting build stays
# faithful to the original workspace.

add_library(rlImGui STATIC)

file(GLOB tiny16_rlimgui_sources CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/extras/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/../ImGui/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/../ImGui/*.h"
)

target_sources(rlImGui PRIVATE ${tiny16_rlimgui_sources})

target_include_directories(rlImGui PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/../ImGui"
    "${CMAKE_CURRENT_SOURCE_DIR}/../raylib/src"
)

target_compile_features(rlImGui PRIVATE cxx_std_17)
tiny16_apply_common_compiler_policy(rlImGui)
tiny16_set_output_layout(rlImGui)
