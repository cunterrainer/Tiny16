project "Tiny16-Assembler"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++20"

    files {
        "src/**.cpp",
        "src/**.hpp"
    }

    includedirs {
        "lib/src"
    }

    links {
        "Tiny16-Assembler-Lib"
    }

    filter "configurations:Debug"
        warnings "off"
        externalwarnings "off"

    -- gcc* clang* msc*
    filter { "toolset:msc*", "configurations:Release or configurations:Distribution or configurations:MinSizeDistribution" }
        warnings "High" -- High
        externalwarnings "off" -- Default
        -- buildoptions { "/sdl" } -- only usefull for debug and maybe not even that, we don't want uninitialized pointers to be nullptrs
        -- disablewarnings "4244" -- float to int without cast

    filter { "toolset:gcc* or toolset:clang*", "configurations:Release or configurations:Distribution or configurations:MinSizeDistribution" }
        enablewarnings {
            "cast-align",
            "cast-qual",
            "disabled-optimization",
            "format=2",
            "init-self",
            "missing-declarations",
            "missing-include-dirs",
            "missing-field-initializers",
            "unused-parameter",
            "redundant-decls",
            "shadow",
            "sign-conversion",
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
            "deprecated-copy",
            "stack-protector",
            "vla",
            "shift-overflow"
        }
        disablewarnings { "unknown-warning-option" }

    filter { "toolset:gcc*", "configurations:Release or configurations:Distribution or configurations:MinSizeDistribution" }
        warnings "Extra"
        externalwarnings "off"
        -- linkgroups "on" -- activate position independent linking
        enablewarnings {
            "array-bounds=2",
            "duplicated-branches",
            "duplicated-cond",
            "logical-op",
            "arith-conversion",
            "stringop-overflow=4",
            "implicit-fallthrough=3",
            "trampolines"
        }

    filter { "toolset:clang*", "configurations:Release or configurations:Distribution or configurations:MinSizeDistribution" }
        warnings "Extra"
        externalwarnings "Everything"
        enablewarnings {
            "array-bounds",
            "long-long",
            "implicit-fallthrough", 
        }

    filter { "system:emscripten", "configurations:Release or configurations:Distribution or configurations:MinSizeDistribution" }
        linkoptions "--memory-init-file 0"
        warnings "Extra"
        externalwarnings "Everything"

    filter { "configurations:Distribution or MinSizeDistribution or Release" }
        fatalwarnings { "All" }
    filter {}
