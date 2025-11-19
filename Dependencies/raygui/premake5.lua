project "raygui"
    language "C"
    kind "StaticLib"
    warnings "off"
    externalwarnings "off"
    
    files {
        "src/**.h",
        "src/**.c"
    }

    includedirs {
        RaylibDir .. "/src"
    }

    filter "system:windows"
        defines "_CRT_SECURE_NO_WARNINGS"