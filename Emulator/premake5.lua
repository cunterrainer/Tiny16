project "Tiny16-Emulator"
    language "C++"
    cppdialect "C++20"
    flags "FatalWarnings"

    files {
        "src/**.cpp",
        "src/**.hpp"
    }

    -- gcc* clang* msc*
    filter "toolset:msc*"
        warnings "High" -- High
        externalwarnings "Default" -- Default
        buildoptions { "/sdl" }

    filter { "toolset:gcc* or toolset:clang*" }
        enablewarnings {
            "cast-align",
            "cast-qual",
            "ctor-dtor-privacy",
            "disabled-optimization",
            "format=2",
            "init-self",
            "missing-declarations",
            "missing-include-dirs",
            "old-style-cast",
            "overloaded-virtual",
            "redundant-decls",
            "shadow",
            "sign-conversion",
            "sign-promo",
            "strict-overflow=5",
            "switch-default",
            "undef",
            "uninitialized",
            "unreachable-code",
            "unused",
            "alloca",
            "conversion",
            "deprecated",
            "format-security",
            "null-dereference",
            "stack-protector",
            "vla",
            "shift-overflow"
        }
        disablewarnings "unknown-warning-option"

    filter { "configurations:Release", "toolset:gcc*" }
        buildoptions { "-ffunction-sections", "-fdata-sections" } -- places each function and data item in its own section
        linkoptions { "-Wl,--gc-sections" } -- remove unused sections (code)

    filter { "system:linux or system:macosx", "configurations:Release", "toolset:clang*" }
        buildoptions { "-ffunction-sections", "-fdata-sections" } -- places each function and data item in its own section
        linkoptions { "-Wl,--gc-sections" } -- remove unused sections (code)

    filter { "system:windows", "configurations:Release", "toolset:clang*" }
        buildoptions { "-ffunction-sections", "-fdata-sections" } -- places each function and data item in its own section
        linkoptions { "-fuse-ld=lld", "-Wl,/OPT:REF,/OPT:ICF" } -- remove unused sections (code)

    filter { "configurations:Release", "toolset:msc*" }
        linkoptions { "/OPT:REF", "/OPT:ICF" } -- remove unused sections (code)

    filter "toolset:gcc*"
        warnings "Extra"
        externalwarnings "Off"
        linkgroups "on" -- activate position independent linking
        enablewarnings {
            "noexcept",
            "strict-null-sentinel",
            "array-bounds=2",
            "duplicated-branches",
            "duplicated-cond",
            "logical-op",
            "arith-conversion",
            "stringop-overflow=4",
            "implicit-fallthrough=3",
            "trampolines"
        }

    filter "toolset:clang*"
        warnings "Extra"
        externalwarnings "Everything"
        enablewarnings {
            "array-bounds",
            "long-long",
            "implicit-fallthrough", 
        }

    filter { "configurations:Debug" }
        kind "ConsoleApp"
        floatingpoint "Default"

    filter { "configurations:Release" }
        kind "ConsoleApp"
        floatingpoint "Default"
filter {}
