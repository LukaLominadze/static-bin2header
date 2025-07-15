-- premake5.lua
workspace "Bin2Header"
   architecture "x64"
   configurations { "Debug", "Release" }
   startproject "Bin2Header"

   -- Workspace-wide build options for MSVC
   filter "system:windows"
      buildoptions { "/EHsc", "/Zc:preprocessor", "/Zc:__cplusplus" }

-- Directories
outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

include "src/Build-Bin2Header.lua"

