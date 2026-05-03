@echo off
echo Building Dangerous Dave...

if not exist build mkdir build
cd build

cmake .. -G "MinGW Makefiles" 2>nul || cmake .. -G "NMake Makefiles" 2>nul || cmake ..

if %ERRORLEVEL% neq 0 (
    echo CMake configuration failed!
    exit /b 1
)

cmake --build .

if %ERRORLEVEL% neq 0 (
    echo Build failed!
    exit /b 1
)

echo.
echo Build complete! Run with: build\dave.exe
cd ..
