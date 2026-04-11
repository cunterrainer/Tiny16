include_guard(GLOBAL)

function(tiny16_detect_toolset out_var)
    # Premake uses `cfg.toolset` to partition binaries by compiler family. CMake
    # does not expose that exact concept, so the helper maps the active compiler
    # and toolchain to the closest readable bucket.
    if(CMAKE_SYSTEM_NAME STREQUAL "Emscripten")
        set(toolset_name "emscripten")
    elseif(MSVC)
        set(toolset_name "msc")
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        set(toolset_name "gcc")
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        set(toolset_name "clang")
    else()
        set(toolset_name "unknown")
    endif()

    set(${out_var} "${toolset_name}" PARENT_SCOPE)
endfunction()

function(tiny16_set_output_layout target_name)
    # Premake wrote binaries to:
    #   /BIN/<toolset>/<config>/<project>/bin
    # and intermediates to:
    #   /BIN/<toolset>/<config>/<project>/bin-int
    # CMake does not let us fully recreate the internal object-file layout, but
    # target output directories can match the visible binary locations exactly.
    set(output_base "${TINY16_OUTPUT_ROOT}/${TINY16_TOOLSET_NAME}/$<CONFIG>/${target_name}")

    set_target_properties(${target_name} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${output_base}/bin"
        LIBRARY_OUTPUT_DIRECTORY "${output_base}/bin"
        ARCHIVE_OUTPUT_DIRECTORY "${output_base}/bin"
        PDB_OUTPUT_DIRECTORY "${output_base}/bin-int"
        COMPILE_PDB_OUTPUT_DIRECTORY "${output_base}/bin-int"
    )

    if(TINY16_EXECUTABLE_SUFFIX AND NOT target_name MATCHES "raylib|ImGui|raygui|rlImGui|ImGuiColorTextEdit|Tiny16-Assembler-Lib")
        set_target_properties(${target_name} PROPERTIES SUFFIX "${TINY16_EXECUTABLE_SUFFIX}")
    endif()
endfunction()

function(tiny16_apply_common_compiler_policy target_name)
    # Premake turned on the multi-processor compiler flag for Visual Studio.
    if(MSVC)
        target_compile_options(${target_name} PRIVATE /MP)
    endif()

    # Release-like configurations in Premake all share the same strip/section
    # behavior. The compiler and linker options below keep that behavior intact.
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(${target_name} PRIVATE
            $<$<OR:$<CONFIG:Release>,$<CONFIG:Distribution>,$<CONFIG:MinSizeDistribution>>:-ffunction-sections>
            $<$<OR:$<CONFIG:Release>,$<CONFIG:Distribution>,$<CONFIG:MinSizeDistribution>>:-fdata-sections>
            $<$<OR:$<CONFIG:Release>,$<CONFIG:Distribution>,$<CONFIG:MinSizeDistribution>>:-ffast-math>
        )

        target_link_options(${target_name} PRIVATE
            $<$<OR:$<CONFIG:Release>,$<CONFIG:Distribution>,$<CONFIG:MinSizeDistribution>>:-Wl,--gc-sections>
        )
    elseif(MSVC)
        target_compile_options(${target_name} PRIVATE
            $<$<CONFIG:Release>:/O2>
            $<$<CONFIG:Distribution>:/Ox>
            $<$<CONFIG:MinSizeDistribution>:/O1>
            $<$<OR:$<CONFIG:Release>,$<CONFIG:Distribution>,$<CONFIG:MinSizeDistribution>>:/fp:fast>
        )

        target_link_options(${target_name} PRIVATE
            $<$<OR:$<CONFIG:Release>,$<CONFIG:Distribution>,$<CONFIG:MinSizeDistribution>>:/OPT:REF>
            $<$<OR:$<CONFIG:Release>,$<CONFIG:Distribution>,$<CONFIG:MinSizeDistribution>>:/OPT:ICF>
        )
    endif()

    set_target_properties(${target_name} PROPERTIES
        INTERPROCEDURAL_OPTIMIZATION_RELEASE OFF
        INTERPROCEDURAL_OPTIMIZATION_DISTRIBUTION ON
        INTERPROCEDURAL_OPTIMIZATION_MINSIZEDISTRIBUTION ON
    )
endfunction()

function(tiny16_apply_cpp_premake_warnings target_name)
    # This matches the warning strategy used by the C++ Premake projects: quiet
    # Debug builds, then progressively stricter checks in Release and beyond.
    target_compile_options(${target_name} PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/Zc:__cplusplus>
        $<$<AND:$<CXX_COMPILER_ID:MSVC>,$<OR:$<CONFIG:Release>,$<CONFIG:Distribution>,$<CONFIG:MinSizeDistribution>>>:/W4>
        $<$<AND:$<CXX_COMPILER_ID:MSVC>,$<OR:$<CONFIG:Release>,$<CONFIG:Distribution>,$<CONFIG:MinSizeDistribution>>>:/WX>
        $<$<AND:$<NOT:$<CXX_COMPILER_ID:MSVC>>,$<OR:$<CONFIG:Release>,$<CONFIG:Distribution>,$<CONFIG:MinSizeDistribution>>>:-Wall>
        $<$<AND:$<NOT:$<CXX_COMPILER_ID:MSVC>>,$<OR:$<CONFIG:Release>,$<CONFIG:Distribution>,$<CONFIG:MinSizeDistribution>>>:-Wextra>
        $<$<AND:$<CXX_COMPILER_ID:GNU>,$<OR:$<CONFIG:Release>,$<CONFIG:Distribution>,$<CONFIG:MinSizeDistribution>>>:-Wcast-align>
        $<$<AND:$<CXX_COMPILER_ID:GNU>,$<OR:$<CONFIG:Release>,$<CONFIG:Distribution>,$<CONFIG:MinSizeDistribution>>>:-Wcast-qual>
        $<$<AND:$<CXX_COMPILER_ID:GNU>,$<OR:$<CONFIG:Release>,$<CONFIG:Distribution>,$<CONFIG:MinSizeDistribution>>>:-Wdisabled-optimization>
        $<$<AND:$<CXX_COMPILER_ID:GNU>,$<OR:$<CONFIG:Release>,$<CONFIG:Distribution>,$<CONFIG:MinSizeDistribution>>>:-Wformat=2>
        $<$<AND:$<CXX_COMPILER_ID:GNU>,$<OR:$<CONFIG:Release>,$<CONFIG:Distribution>,$<CONFIG:MinSizeDistribution>>>:-Winit-self>
        $<$<AND:$<CXX_COMPILER_ID:GNU>,$<OR:$<CONFIG:Release>,$<CONFIG:Distribution>,$<CONFIG:MinSizeDistribution>>>:-Wmissing-declarations>
        $<$<AND:$<CXX_COMPILER_ID:GNU>,$<OR:$<CONFIG:Release>,$<CONFIG:Distribution>,$<CONFIG:MinSizeDistribution>>>:-Wmissing-include-dirs>
        $<$<AND:$<CXX_COMPILER_ID:GNU>,$<OR:$<CONFIG:Release>,$<CONFIG:Distribution>,$<CONFIG:MinSizeDistribution>>>:-Wmissing-field-initializers>
        $<$<AND:$<CXX_COMPILER_ID:GNU>,$<OR:$<CONFIG:Release>,$<CONFIG:Distribution>,$<CONFIG:MinSizeDistribution>>>:-Wunused-parameter>
        $<$<AND:$<CXX_COMPILER_ID:GNU>,$<OR:$<CONFIG:Release>,$<CONFIG:Distribution>,$<CONFIG:MinSizeDistribution>>>:-Wredundant-decls>
        $<$<AND:$<CXX_COMPILER_ID:GNU>,$<OR:$<CONFIG:Release>,$<CONFIG:Distribution>,$<CONFIG:MinSizeDistribution>>>:-Wshadow>
        $<$<AND:$<CXX_COMPILER_ID:GNU>,$<OR:$<CONFIG:Release>,$<CONFIG:Distribution>,$<CONFIG:MinSizeDistribution>>>:-Wsign-conversion>
        $<$<AND:$<CXX_COMPILER_ID:GNU>,$<OR:$<CONFIG:Release>,$<CONFIG:Distribution>,$<CONFIG:MinSizeDistribution>>>:-Wstrict-overflow=5>
        $<$<AND:$<CXX_COMPILER_ID:GNU>,$<OR:$<CONFIG:Release>,$<CONFIG:Distribution>,$<CONFIG:MinSizeDistribution>>>:-Wswitch-default>
        $<$<AND:$<CXX_COMPILER_ID:GNU>,$<OR:$<CONFIG:Release>,$<CONFIG:Distribution>,$<CONFIG:MinSizeDistribution>>>:-Wundef>
        $<$<AND:$<CXX_COMPILER_ID:GNU>,$<OR:$<CONFIG:Release>,$<CONFIG:Distribution>,$<CONFIG:MinSizeDistribution>>>:-Wuninitialized>
        $<$<AND:$<CXX_COMPILER_ID:GNU>,$<OR:$<CONFIG:Release>,$<CONFIG:Distribution>,$<CONFIG:MinSizeDistribution>>>:-Wunreachable-code>
        $<$<AND:$<CXX_COMPILER_ID:GNU>,$<OR:$<CONFIG:Release>,$<CONFIG:Distribution>,$<CONFIG:MinSizeDistribution>>>:-Wunused>
        $<$<AND:$<CXX_COMPILER_ID:GNU>,$<OR:$<CONFIG:Release>,$<CONFIG:Distribution>,$<CONFIG:MinSizeDistribution>>>:-Walloca>
        $<$<AND:$<CXX_COMPILER_ID:GNU>,$<OR:$<CONFIG:Release>,$<CONFIG:Distribution>,$<CONFIG:MinSizeDistribution>>>:-Wconversion>
        $<$<AND:$<CXX_COMPILER_ID:GNU>,$<OR:$<CONFIG:Release>,$<CONFIG:Distribution>,$<CONFIG:MinSizeDistribution>>>:-Wdeprecated>
        $<$<AND:$<CXX_COMPILER_ID:GNU>,$<OR:$<CONFIG:Release>,$<CONFIG:Distribution>,$<CONFIG:MinSizeDistribution>>>:-Wformat-security>
        $<$<AND:$<CXX_COMPILER_ID:GNU>,$<OR:$<CONFIG:Release>,$<CONFIG:Distribution>,$<CONFIG:MinSizeDistribution>>>:-Wnull-dereference>
        $<$<AND:$<CXX_COMPILER_ID:GNU>,$<OR:$<CONFIG:Release>,$<CONFIG:Distribution>,$<CONFIG:MinSizeDistribution>>>:-Wdeprecated-copy>
        $<$<AND:$<CXX_COMPILER_ID:GNU>,$<OR:$<CONFIG:Release>,$<CONFIG:Distribution>,$<CONFIG:MinSizeDistribution>>>:-Wstack-protector>
        $<$<AND:$<CXX_COMPILER_ID:GNU>,$<OR:$<CONFIG:Release>,$<CONFIG:Distribution>,$<CONFIG:MinSizeDistribution>>>:-Wvla>
        $<$<AND:$<CXX_COMPILER_ID:GNU>,$<OR:$<CONFIG:Release>,$<CONFIG:Distribution>,$<CONFIG:MinSizeDistribution>>>:-Wshift-overflow>
        $<$<AND:$<CXX_COMPILER_ID:GNU>,$<OR:$<CONFIG:Release>,$<CONFIG:Distribution>,$<CONFIG:MinSizeDistribution>>>:-Warray-bounds=2>
        $<$<AND:$<CXX_COMPILER_ID:GNU>,$<OR:$<CONFIG:Release>,$<CONFIG:Distribution>,$<CONFIG:MinSizeDistribution>>>:-Wduplicated-branches>
        $<$<AND:$<CXX_COMPILER_ID:GNU>,$<OR:$<CONFIG:Release>,$<CONFIG:Distribution>,$<CONFIG:MinSizeDistribution>>>:-Wduplicated-cond>
        $<$<AND:$<CXX_COMPILER_ID:GNU>,$<OR:$<CONFIG:Release>,$<CONFIG:Distribution>,$<CONFIG:MinSizeDistribution>>>:-Wlogical-op>
        $<$<AND:$<CXX_COMPILER_ID:GNU>,$<OR:$<CONFIG:Release>,$<CONFIG:Distribution>,$<CONFIG:MinSizeDistribution>>>:-Warith-conversion>
        $<$<AND:$<CXX_COMPILER_ID:GNU>,$<OR:$<CONFIG:Release>,$<CONFIG:Distribution>,$<CONFIG:MinSizeDistribution>>>:-Wstringop-overflow=4>
        $<$<AND:$<CXX_COMPILER_ID:GNU>,$<OR:$<CONFIG:Release>,$<CONFIG:Distribution>,$<CONFIG:MinSizeDistribution>>>:-Wimplicit-fallthrough=3>
        $<$<AND:$<CXX_COMPILER_ID:GNU>,$<OR:$<CONFIG:Release>,$<CONFIG:Distribution>,$<CONFIG:MinSizeDistribution>>>:-Wtrampolines>
        $<$<AND:$<CXX_COMPILER_ID:Clang>,$<OR:$<CONFIG:Release>,$<CONFIG:Distribution>,$<CONFIG:MinSizeDistribution>>>:-Wcast-align>
        $<$<AND:$<CXX_COMPILER_ID:Clang>,$<OR:$<CONFIG:Release>,$<CONFIG:Distribution>,$<CONFIG:MinSizeDistribution>>>:-Wcast-qual>
        $<$<AND:$<CXX_COMPILER_ID:Clang>,$<OR:$<CONFIG:Release>,$<CONFIG:Distribution>,$<CONFIG:MinSizeDistribution>>>:-Wdisabled-optimization>
        $<$<AND:$<CXX_COMPILER_ID:Clang>,$<OR:$<CONFIG:Release>,$<CONFIG:Distribution>,$<CONFIG:MinSizeDistribution>>>:-Wformat=2>
        $<$<AND:$<CXX_COMPILER_ID:Clang>,$<OR:$<CONFIG:Release>,$<CONFIG:Distribution>,$<CONFIG:MinSizeDistribution>>>:-Winit-self>
        $<$<AND:$<CXX_COMPILER_ID:Clang>,$<OR:$<CONFIG:Release>,$<CONFIG:Distribution>,$<CONFIG:MinSizeDistribution>>>:-Wmissing-declarations>
        $<$<AND:$<CXX_COMPILER_ID:Clang>,$<OR:$<CONFIG:Release>,$<CONFIG:Distribution>,$<CONFIG:MinSizeDistribution>>>:-Wmissing-include-dirs>
        $<$<AND:$<CXX_COMPILER_ID:Clang>,$<OR:$<CONFIG:Release>,$<CONFIG:Distribution>,$<CONFIG:MinSizeDistribution>>>:-Wmissing-field-initializers>
        $<$<AND:$<CXX_COMPILER_ID:Clang>,$<OR:$<CONFIG:Release>,$<CONFIG:Distribution>,$<CONFIG:MinSizeDistribution>>>:-Wunused-parameter>
        $<$<AND:$<CXX_COMPILER_ID:Clang>,$<OR:$<CONFIG:Release>,$<CONFIG:Distribution>,$<CONFIG:MinSizeDistribution>>>:-Wredundant-decls>
        $<$<AND:$<CXX_COMPILER_ID:Clang>,$<OR:$<CONFIG:Release>,$<CONFIG:Distribution>,$<CONFIG:MinSizeDistribution>>>:-Wshadow>
        $<$<AND:$<CXX_COMPILER_ID:Clang>,$<OR:$<CONFIG:Release>,$<CONFIG:Distribution>,$<CONFIG:MinSizeDistribution>>>:-Wsign-conversion>
        $<$<AND:$<CXX_COMPILER_ID:Clang>,$<OR:$<CONFIG:Release>,$<CONFIG:Distribution>,$<CONFIG:MinSizeDistribution>>>:-Wstrict-overflow=5>
        $<$<AND:$<CXX_COMPILER_ID:Clang>,$<OR:$<CONFIG:Release>,$<CONFIG:Distribution>,$<CONFIG:MinSizeDistribution>>>:-Wswitch-default>
        $<$<AND:$<CXX_COMPILER_ID:Clang>,$<OR:$<CONFIG:Release>,$<CONFIG:Distribution>,$<CONFIG:MinSizeDistribution>>>:-Wundef>
        $<$<AND:$<CXX_COMPILER_ID:Clang>,$<OR:$<CONFIG:Release>,$<CONFIG:Distribution>,$<CONFIG:MinSizeDistribution>>>:-Wuninitialized>
        $<$<AND:$<CXX_COMPILER_ID:Clang>,$<OR:$<CONFIG:Release>,$<CONFIG:Distribution>,$<CONFIG:MinSizeDistribution>>>:-Wunreachable-code>
        $<$<AND:$<CXX_COMPILER_ID:Clang>,$<OR:$<CONFIG:Release>,$<CONFIG:Distribution>,$<CONFIG:MinSizeDistribution>>>:-Wunused>
        $<$<AND:$<CXX_COMPILER_ID:Clang>,$<OR:$<CONFIG:Release>,$<CONFIG:Distribution>,$<CONFIG:MinSizeDistribution>>>:-Walloca>
        $<$<AND:$<CXX_COMPILER_ID:Clang>,$<OR:$<CONFIG:Release>,$<CONFIG:Distribution>,$<CONFIG:MinSizeDistribution>>>:-Wconversion>
        $<$<AND:$<CXX_COMPILER_ID:Clang>,$<OR:$<CONFIG:Release>,$<CONFIG:Distribution>,$<CONFIG:MinSizeDistribution>>>:-Wdeprecated>
        $<$<AND:$<CXX_COMPILER_ID:Clang>,$<OR:$<CONFIG:Release>,$<CONFIG:Distribution>,$<CONFIG:MinSizeDistribution>>>:-Wformat-security>
        $<$<AND:$<CXX_COMPILER_ID:Clang>,$<OR:$<CONFIG:Release>,$<CONFIG:Distribution>,$<CONFIG:MinSizeDistribution>>>:-Wnull-dereference>
        $<$<AND:$<CXX_COMPILER_ID:Clang>,$<OR:$<CONFIG:Release>,$<CONFIG:Distribution>,$<CONFIG:MinSizeDistribution>>>:-Wdeprecated-copy>
        $<$<AND:$<CXX_COMPILER_ID:Clang>,$<OR:$<CONFIG:Release>,$<CONFIG:Distribution>,$<CONFIG:MinSizeDistribution>>>:-Wstack-protector>
        $<$<AND:$<CXX_COMPILER_ID:Clang>,$<OR:$<CONFIG:Release>,$<CONFIG:Distribution>,$<CONFIG:MinSizeDistribution>>>:-Wvla>
        $<$<AND:$<CXX_COMPILER_ID:Clang>,$<OR:$<CONFIG:Release>,$<CONFIG:Distribution>,$<CONFIG:MinSizeDistribution>>>:-Wshift-overflow>
        $<$<AND:$<CXX_COMPILER_ID:Clang>,$<OR:$<CONFIG:Release>,$<CONFIG:Distribution>,$<CONFIG:MinSizeDistribution>>>:-Warray-bounds>
        $<$<AND:$<CXX_COMPILER_ID:Clang>,$<OR:$<CONFIG:Release>,$<CONFIG:Distribution>,$<CONFIG:MinSizeDistribution>>>:-Wlong-long>
        $<$<AND:$<CXX_COMPILER_ID:Clang>,$<OR:$<CONFIG:Release>,$<CONFIG:Distribution>,$<CONFIG:MinSizeDistribution>>>:-Wimplicit-fallthrough>
        $<$<AND:$<CXX_COMPILER_ID:Clang>,$<NOT:$<CONFIG:Debug>>>:-Wno-unknown-warning-option>
    )

    # Debug builds are intentionally quiet in Premake.
    target_compile_options(${target_name} PRIVATE
        $<$<AND:$<NOT:$<CXX_COMPILER_ID:MSVC>>,$<CONFIG:Debug>>:-w>
    )

    if(MSVC)
        target_compile_options(${target_name} PRIVATE
            $<$<CONFIG:Debug>:/W0>
        )
    endif()
endfunction()

function(tiny16_apply_c_premake_warnings target_name)
    # The C libraries in Tiny16 are vendored dependencies, so their Premake
    # files intentionally silence warnings instead of layering on extra policy.
    target_compile_options(${target_name} PRIVATE
        $<$<C_COMPILER_ID:MSVC>:/W0>
        $<$<NOT:$<C_COMPILER_ID:MSVC>>:-w>
    )
endfunction()

function(tiny16_silence_warnings target_name)
    target_compile_options(${target_name} PRIVATE
        $<$<C_COMPILER_ID:MSVC>:/W0>
        $<$<CXX_COMPILER_ID:MSVC>:/W0>
        $<$<NOT:$<OR:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>>:-w>
    )
endfunction()

function(tiny16_apply_release_linker_policy target_name)
    # The Windows-specific and POSIX-specific linker choices from Premake are
    # applied here as a small set of target-local branches.
    if(MSVC)
        target_link_options(${target_name} PRIVATE
            $<$<OR:$<CONFIG:Distribution>,$<CONFIG:MinSizeDistribution>>:/ENTRY:mainCRTStartup>
        )
    endif()
endfunction()
