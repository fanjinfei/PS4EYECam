@echo off
IF NOT EXIST build mkdir build
cd build

echo "Rebuilding PS4EyeCam Project files..."
cmake .. -G "Visual Studio 14 2015" 
pause