@echo off
setlocal EnableExtensions

for %%I in ("%~dp0\..\..") do set "ROOT_DIR=%%~fI"
set "OUT_DIR=%ROOT_DIR%\out"
set "BUILD_DIR=%OUT_DIR%\build-vs2022"
set "INSTALL_DIR=%OUT_DIR%\install-vs2022"
set "PACKAGE_DIR=%OUT_DIR%\package-vs2022"
set "LOG_DIR=%OUT_DIR%\logs"
set "CONFIG_PRESET=windows-vs2022-debug"
set "BUILD_CONFIG=Debug"

if not exist "%OUT_DIR%" mkdir "%OUT_DIR%" >nul 2>nul
if not exist "%LOG_DIR%" mkdir "%LOG_DIR%" >nul 2>nul

where cmake >nul 2>nul
if errorlevel 1 (
    echo [ERROR] cmake.exe was not found in PATH.
    echo [HINT] Install CMake and make sure PATH is configured.
    exit /b 1
)

where ctest >nul 2>nul
if errorlevel 1 (
    echo [ERROR] ctest.exe was not found in PATH.
    echo [HINT] Install CMake and make sure PATH is configured.
    exit /b 1
)

where cpack >nul 2>nul
if errorlevel 1 (
    echo [ERROR] cpack.exe was not found in PATH.
    echo [HINT] Install CMake and make sure PATH is configured.
    exit /b 1
)

set "VSWHERE_EXE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
if not exist "%VSWHERE_EXE%" (
    echo [ERROR] vswhere.exe was not found.
    echo [HINT] Install Visual Studio 2022 with Desktop development with C++.
    exit /b 1
)

set "VS_INSTALL_DIR="
for /f "usebackq delims=" %%I in (`"%VSWHERE_EXE%" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do set "VS_INSTALL_DIR=%%I"
if not defined VS_INSTALL_DIR (
    for /f "usebackq delims=" %%I in (`"%VSWHERE_EXE%" -latest -products * -requires Microsoft.Component.MSBuild -property installationPath`) do set "VS_INSTALL_DIR=%%I"
)
if not defined VS_INSTALL_DIR (
    echo [ERROR] Visual Studio 2022 with C++ build tools was not found.
    echo [HINT] Install Visual Studio 2022 and include MSVC + MSBuild components.
    exit /b 1
)

if exist "%VS_INSTALL_DIR%\Common7\Tools\VsDevCmd.bat" (
    call "%VS_INSTALL_DIR%\Common7\Tools\VsDevCmd.bat" -host_arch=x64 -arch=x64 >nul
) else if exist "%VS_INSTALL_DIR%\VC\Auxiliary\Build\vcvars64.bat" (
    call "%VS_INSTALL_DIR%\VC\Auxiliary\Build\vcvars64.bat" >nul
) else (
    echo [ERROR] Could not find VsDevCmd.bat or vcvars64.bat under "%VS_INSTALL_DIR%".
    exit /b 1
)
if errorlevel 1 (
    echo [ERROR] Failed to initialize the Visual Studio build environment.
    exit /b 1
)

endlocal & (
    set "ROOT_DIR=%ROOT_DIR%"
    set "OUT_DIR=%OUT_DIR%"
    set "BUILD_DIR=%BUILD_DIR%"
    set "INSTALL_DIR=%INSTALL_DIR%"
    set "PACKAGE_DIR=%PACKAGE_DIR%"
    set "LOG_DIR=%LOG_DIR%"
    set "CONFIG_PRESET=%CONFIG_PRESET%"
    set "BUILD_CONFIG=%BUILD_CONFIG%"
    set "VS_INSTALL_DIR=%VS_INSTALL_DIR%"
)
exit /b 0
