# This mirrors Dependencies/ImGuiColorTextEdit/premake5.lua. The project is a
# small static library, built with the same source glob pattern as Premake and
# treated as a warning-suppressed dependency.

add_library(ImGuiColorTextEdit STATIC)

file(GLOB tiny16_imgui_color_text_edit_sources CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/s*.h"
)

target_sources(ImGuiColorTextEdit PRIVATE ${tiny16_imgui_color_text_edit_sources})

target_include_directories(ImGuiColorTextEdit PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/../ImGui"
)

tiny16_apply_common_compiler_policy(ImGuiColorTextEdit)
target_compile_features(ImGuiColorTextEdit PRIVATE cxx_std_20)
tiny16_silence_warnings(ImGuiColorTextEdit)
tiny16_set_output_layout(ImGuiColorTextEdit)
