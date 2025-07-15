@echo off
pushd ..
scripts\premake\Windows\premake5 --file="Build-Solution.lua" vs2022
popd
pause