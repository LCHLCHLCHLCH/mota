@echo off
chcp 65001 >nul
setlocal enabledelayedexpansion

echo ========================================
echo   魔塔 - 编译脚本
echo ========================================
echo.

:: ------ locate cmake ------
set "CMAKE_EXE="
where cmake >nul 2>&1
if %errorlevel% equ 0 (
    set "CMAKE_EXE=cmake"
) else (
    for %%d in (
        "C:\Program Files\CMake\bin"
        "C:\Program Files (x86)\CMake\bin"
    ) do (
        if exist "%%~d\cmake.exe" (
            set "PATH=%%~d;%PATH%"
            set "CMAKE_EXE=cmake"
        )
    )
)
if "%CMAKE_EXE%"=="" (
    echo [错误] 未找到 cmake。请安装 CMake 并添加到环境变量。
    echo        下载地址: https://cmake.org/download/
    pause
    exit /b 1
)
echo [信息] cmake 已就绪

:: ------ locate g++ and mingw32-make ------
set "GXX_EXE="
set "MAKE_EXE="
where g++ >nul 2>&1
if %errorlevel% equ 0 (
    set "GXX_EXE=g++"
)
where mingw32-make >nul 2>&1
if %errorlevel% equ 0 (
    set "MAKE_EXE=mingw32-make"
)
:: if not found, try common MinGW paths
if "%GXX_EXE%"=="" (
    for %%d in (
        "C:\mingw64\bin"
        "C:\MinGW\bin"
        "C:\msys64\mingw64\bin"
    ) do (
        if exist "%%~d\g++.exe" (
            set "PATH=%%~d;%PATH%"
            set "GXX_EXE=g++"
            set "MAKE_EXE=mingw32-make"
        )
    )
)
if "%GXX_EXE%"=="" (
    echo [错误] 未找到 g++。请安装 MinGW-w64 并添加到环境变量。
    echo        下载地址: https://www.mingw-w64.org/
    pause
    exit /b 1
)
echo [信息] g++ 已就绪
echo [信息] mingw32-make 已就绪
echo.

:: ------ create build directory ------
if not exist build (
    mkdir build
    echo [信息] 已创建 build 目录
)

:: ------ configure ------
echo [信息] 正在配置项目...
cmake -G "MinGW Makefiles" -S . -B build
if %errorlevel% neq 0 (
    echo [错误] cmake 配置失败！
    pause
    exit /b 1
)

:: ------ build ------
echo.
echo [信息] 正在编译...
cmake --build build
if %errorlevel% neq 0 (
    echo [错误] 编译失败！
    pause
    exit /b 1
)

echo.
echo ========================================
echo   编译成功！二进制文件位于 build\MagicalTower.exe
echo ========================================
pause
