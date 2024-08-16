workspace "Tiny16"
    configurations {
        "Debug",
        "Release"
    }
    startproject "Tiny16-Emulator"

outputdir = "/BIN/%{cfg.toolset}/%{cfg.shortname}/%{prj.name}/"
cwd = os.getcwd() -- get current working directory
targetdir(cwd .. outputdir .. "bin")
objdir(cwd .. outputdir .. "bin-int")

filter "system:windows"
    platforms { "x64", "x86" }
    defines { "_CRT_SECURE_NO_WARNINGS", "PLATFORM_WINDOWS" }
filter "system:linux"
    platforms { "x64", "x86" }
    defines "PLATFORM_UNIX"
filter "system:macosx"
    platforms "universal"
    defines { "MAC_OS", "GL_SILENCE_DEPRECATION" }

filter "platforms:x64"
    architecture "x86_64"
filter "platforms:x86"
    architecture "x86"
filter "platforms:universal"
    architecture "universal"

filter { "configurations:Debug" }
    runtime "Debug"
    symbols "on"
    optimize "off"
    defines "BUILD_DEBUG"
filter { "configurations:Release" }
    runtime "Release"
    symbols "off"
    optimize "Speed"
    defines "NDEBUG"
    flags "LinkTimeOptimization"
filter {}


flags {
    "MultiProcessorCompile" -- only for visual studio
}
staticruntime "off"
removeunreferencedcodedata "on"

include "Emulator"
