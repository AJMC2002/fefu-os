@echo off
setlocal

echo Updating source code...
git pull origin main

set BUILD_DIR=build
if not exist "%BUILD_DIR%" mkdir "%BUILD_DIR%"

cd "%BUILD_DIR%"
cmake -G "MinGW Makefiles" ..
mingw32-make

echo Build successful. Running the program...
HelloWorld.exe

endlocal
