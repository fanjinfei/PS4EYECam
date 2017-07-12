@echo off

IF NOT EXIST build mkdir build
cd build

echo "Rebuilding PS4EYECam x64 Project files..."
cmake .. -G "Visual Studio 14 2015 Win64"
pause