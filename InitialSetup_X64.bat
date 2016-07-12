@echo off
setlocal

:: Add MSVC build tools to the path
call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" x86

:: Compile the DEBUG|x64 and RELEASE|x64 builds of libusb
pushd thirdparty\libusb\msvc\
echo "Building libusb DEBUG|x64..."
MSBuild.exe libusb_2015.sln /tv:12.0 /p:configuration=DEBUG /p:Platform="x64" /t:Clean;Build 
echo "Building libusb RELEASE|x64..."
MSBuild.exe libusb_2015.sln /tv:12.0 /p:configuration=RELEASE /p:Platform="x64" /t:Clean;Build
popd

:: Compile the RELEASE|x64 build of SDL2
echo "Creating SDL2 project files..."
pushd thirdparty\SDL2
del /f /s /q build > nul
rmdir /s /q build
mkdir build
pushd build
cmake .. -G "Visual Studio 14 2015 Win64" -DDIRECTX=OFF -DDIRECTX=OFF -DSDL_STATIC=ON -DFORCE_STATIC_VCRT=ON -DEXTRA_CFLAGS="-MT -Z7 -DSDL_MAIN_HANDLED -DWIN32 -DNDEBUG -D_CRT_SECURE_NO_WARNINGS -DHAVE_LIBC -D_USE_MATH_DEFINES
echo "Building SDL2 Release|x64..."
MSBuild.exe SDL2.sln /p:configuration=RELEASE /p:Platform="x64" /t:Clean
MSBuild.exe SDL2-static.vcxproj /p:configuration=RELEASE /p:Platform="x64" /t:Build 
MSBuild.exe SDL2main.vcxproj /p:configuration=RELEASE /p:Platform="x64" /t:Build
popd
popd

:: Generate the project files for PS4EYECam
call GenerateProjectFiles_X64.bat
pause
goto exit

:failure
goto exit

:exit
endlocal
