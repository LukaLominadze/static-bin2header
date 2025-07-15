project "Bin2Header"
   kind "StaticLib"
   language "C++"
   cppdialect "C++20"
   targetdir "bin/%{cfg.buildcfg}"
   staticruntime "off"

   files { "**.h", "**.hpp", "**.cpp" }

   includedirs
   {
      "include"
   }

   targetdir ("../bin/" .. outputdir .. "/%{prj.name}")
   objdir ("../bin-int/" .. outputdir .. "/%{prj.name}")

   filter "system:windows"
      systemversion "latest"
      defines { "WIN32", "_WINDOWS" }

filter "configurations:Debug"
    defines { "_DEBUG" }
    runtime "Debug"
    symbols "On"

filter "configurations:Release"
    defines { "NDEBUG" }
    runtime "Release"
    optimize "On"
    symbols "On"
