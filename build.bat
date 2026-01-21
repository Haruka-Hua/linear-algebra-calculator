@echo off
REM Linear Algebra Calculator - Windows Build Script
REM 用于自动化构建 Release 版本可执行文件

setlocal enabledelayedexpansion

echo ========================================
echo LAC (Linear Algebra Calculator)
echo Windows Build Script v1.0.0
echo ========================================
echo.

REM 检查 CMake 是否安装
where cmake >nul 2>nul
if %errorlevel% neq 0 (
    echo [ERROR] CMake not found. Please install CMake 3.14+
    echo Download from: https://cmake.org/download/
    exit /b 1
)

echo [INFO] CMake found: 
cmake --version | findstr "cmake version"
echo.

REM 检查 build 目录，如果存在则清理
if exist build (
    echo [INFO] Cleaning old build directory...
    rmdir /s /q build >nul 2>&1
)

echo [INFO] Creating build directory...
mkdir build

echo [INFO] Configuring CMake (Release mode)...
cd build
cmake -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release ..
if %errorlevel% neq 0 (
    echo [ERROR] CMake configuration failed!
    cd ..
    exit /b 1
)

echo [INFO] Building project...
cmake --build . --config Release
if %errorlevel% neq 0 (
    echo [ERROR] Build failed!
    cd ..
    exit /b 1
)

cd ..

echo.
echo ========================================
echo [SUCCESS] Build completed!
echo ========================================
echo.
echo Executable location:
echo   build\bin\lac_cli_app.exe
echo.
echo To run the program:
echo   build\bin\lac_cli_app.exe
echo.
echo Or double-click the .exe file directly.
echo ========================================
