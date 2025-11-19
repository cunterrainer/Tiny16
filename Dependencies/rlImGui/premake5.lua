project "rlImGui"
	kind "StaticLib"
	language "C++"
	cdialect "C99"
	cppdialect "C++17"

	includedirs { 
		"../ImGui",
        RaylibDir .. "/src",
	}

	files {
		"../ImGui/*.h",
		"../ImGui/*.cpp",
		"../ImGui/*.h",
		"../ImGui/*.cpp",
		"*.cpp",
		"*.h",
		"extras/**.h"
	}

	defines {
		"IMGUI_DISABLE_OBSOLETE_FUNCTIONS",
		"IMGUI_DISABLE_OBSOLETE_KEYIO"
	}