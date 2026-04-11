# This is the largest first-party target and the closest match to
# Emulator/premake5.lua. It builds the graphical emulator executable, wires in
# the UI and assembler dependencies, and preserves the platform-specific link
# libraries and subsystem behavior from Premake.

add_executable(Tiny16-Emulator)

file(GLOB_RECURSE tiny16_emulator_sources CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp"
)

target_sources(Tiny16-Emulator PRIVATE ${tiny16_emulator_sources})

target_include_directories(Tiny16-Emulator PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/.."
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Utility/include"
)

# The Premake file distinguishes between normal and external include paths. In
# CMake we model that distinction with SYSTEM includes where it helps keep the
# dependency warnings suppressed in the same places.
target_include_directories(Tiny16-Emulator SYSTEM PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/../Dependencies/raylib/src"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Dependencies/raygui"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Dependencies/raygui/src"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Dependencies/rlImGui"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Dependencies/ImGui"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Dependencies/ImGuiColorTextEdit"
)

target_compile_definitions(Tiny16-Emulator PRIVATE _CRT_SECURE_NO_WARNINGS)
target_compile_features(Tiny16-Emulator PRIVATE cxx_std_20)

target_link_libraries(Tiny16-Emulator PRIVATE
    raylib
    raygui
    rlImGui
    ImGuiColorTextEdit
    Tiny16-Assembler-Lib
)

tiny16_apply_common_compiler_policy(Tiny16-Emulator)
tiny16_apply_cpp_premake_warnings(Tiny16-Emulator)
tiny16_set_output_layout(Tiny16-Emulator)
tiny16_apply_release_linker_policy(Tiny16-Emulator)

if(WIN32)
    target_link_libraries(Tiny16-Emulator PRIVATE
        Winmm
        opengl32
        gdi32
        shell32
        User32
    )
endif()

if(UNIX AND NOT APPLE)
    target_link_libraries(Tiny16-Emulator PRIVATE
        GL
        X11
        rt
        dl
        m
    )
endif()

if(APPLE)
    target_link_options(Tiny16-Emulator PRIVATE
        "-framework AppKit"
        "-framework iokit"
        "-framework OpenGl"
    )

    # Premake also disabled sign-conversion warnings on macOS for this target.
    target_compile_options(Tiny16-Emulator PRIVATE -Wno-sign-conversion)
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Emscripten")
    target_compile_options(Tiny16-Emulator PRIVATE -fexceptions -pthread)
    target_link_options(Tiny16-Emulator PRIVATE
        -sUSE_GLFW=3
        -sMIN_WEBGL_VERSION=2
        -sMAX_WEBGL_VERSION=2
        -sALLOW_MEMORY_GROWTH=1
        -sUSE_PTHREADS=1
        -fexceptions
        -sSTACK_SIZE=4194304
        -sPTHREAD_POOL_SIZE=2
    )
endif()

# Premake switches the Windows subsystem only for Distribution-like builds.
# CMake cannot express that with `WIN32` alone, so the link option is attached
# only for the matching configurations.
if(MSVC)
    target_link_options(Tiny16-Emulator PRIVATE
        $<$<CONFIG:Distribution,MinSizeDistribution>:/SUBSYSTEM:WINDOWS>
    )
endif()

if(WIN32 AND NOT MSVC)
    target_link_options(Tiny16-Emulator PRIVATE
        $<$<OR:$<CONFIG:Distribution>,$<CONFIG:MinSizeDistribution>>:-mwindows>
    )
endif()
