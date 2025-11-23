workspace "Tiny16"
    configurations {
        "Debug",
        "Release",
        "Distribution",
        "MinSizeDistribution"
    }
    startproject "Tiny16-Emulator"

outputdir = "/BIN/%{cfg.toolset}/%{cfg.shortname}/%{prj.name}/"
cwd = os.getcwd() -- get current working directory
targetdir(cwd .. outputdir .. "bin")
objdir(cwd .. outputdir .. "bin-int")

RaylibDir = cwd .. "/Dependencies/raylib"

filter "system:windows"
    platforms { "x64", "x86", "arm", "arm64" }
filter "system:linux"
    platforms { "x64", "x86", "arm", "arm64" }
filter "system:macosx"
    platforms "universal"
filter "system:emscripten" -- wasm
    platforms { "wasm32", "wasm64" }
    targetextension ".html" -- we want to get the .js and .html files aswell as the .wasm file

filter "platforms:x86"
    architecture "x86"
filter "platforms:x64"
    architecture "x86_64"
filter "platforms:arm"
    architecture "ARM"
filter "platforms:arm64"
    architecture "ARM64"
filter "platforms:universal"
    architecture "universal"
filter "platforms:wasm32"
    architecture "wasm32"
filter "platforms:wasm64"
    architecture "wasm64"

filter { "configurations:Debug" }
    runtime "Debug"
    symbols "on"
    optimize "off"
    floatingpoint "Default"

filter { "configurations:Release" }
    runtime "Release"
    symbols "on"
    optimize "Speed"
    defines { "NDEBUG", "MODE_RELEASE" }
    floatingpoint "fast"
    linktimeoptimization "on"

filter { "configurations:Distribution" }
    runtime "Release"
    symbols "off"
    optimize "Full"
    defines { "NDEBUG", "MODE_RELEASE" }
    floatingpoint "fast"
    linktimeoptimization "on"

filter { "configurations:MinSizeDistribution" }
    runtime "Release"
    symbols "off"
    optimize "Size"
    defines { "NDEBUG", "MODE_RELEASE" }
    floatingpoint "fast"
    linktimeoptimization "on"


filter { "configurations:Release or configurations:Distribution or configurations:MinSizeDistribution", "toolset:gcc*" }
    buildoptions { "-ffunction-sections", "-fdata-sections" } -- places each function and data item in its own section
    linkoptions { "-Wl,--gc-sections" } -- remove unused sections (code)

filter { "system:linux or system:macosx", "configurations:Release or configurations:Distribution or configurations:MinSizeDistribution", "toolset:clang*" }
    buildoptions { "-ffunction-sections", "-fdata-sections" } -- places each function and data item in its own section
    linkoptions { "-Wl,--gc-sections" } -- remove unused sections (code)

filter { "system:windows", "configurations:Release or configurations:Distribution or configurations:MinSizeDistribution", "toolset:clang*" }
    buildoptions { "-ffunction-sections", "-fdata-sections" } -- places each function and data item in its own section
    linkoptions { "-fuse-ld=lld", "-Wl,/OPT:REF,/OPT:ICF" } -- remove unused sections (code)

filter { "configurations:Release or configurations:Distribution or configurations:MinSizeDistribution", "toolset:msc*" }
    linkoptions { "/OPT:REF", "/OPT:ICF" } -- remove unused sections (code)
filter {}

-- only for visual studio
flags {
    "MultiProcessorCompile"
}
linkgroups "off"
staticruntime "on"
removeunreferencedcodedata "on"

include "Emulator"
include "Assembler"
include "Assembler/lib"
include "Assembler/test"
include "Dependencies/ImGui"
include "Dependencies/raylib"
include "Dependencies/raygui"
include "Dependencies/rlImGui"
include "Dependencies/ImGuiColorTextEdit"
