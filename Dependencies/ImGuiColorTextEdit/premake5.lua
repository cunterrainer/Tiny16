project "ImGuiColorTextEdit"
    language "C++"
    cppdialect "C++20"
    kind "StaticLib"
    warnings "off"
    externalwarnings "off"

    files {
        "s*.h",
        "*.cpp"
    }

    includedirs {
        "../ImGui"
    }