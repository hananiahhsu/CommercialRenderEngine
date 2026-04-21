@echo off
setlocal EnableExtensions
set "LOG_FILE=%~dp0..\..\out\logs\configure_vs2022.log"
call "%~dp0common_windows.bat"
if errorlevel 1 goto :fail

set "LOG_FILE=%LOG_DIR%\configure_vs2022.log"
set "PRESETS_FILE=%ROOT_DIR%\CMakePresets.json"

echo [INFO] Configuring MeridianRenderEngine with preset %CONFIG_PRESET%.
echo [INFO] Root    : "%ROOT_DIR%"
echo [INFO] Presets : "%PRESETS_FILE%"
echo [INFO] Build   : "%BUILD_DIR%"
echo [INFO] Log     : "%LOG_FILE%"

if not exist "%PRESETS_FILE%" (
    echo [ERROR] CMakePresets.json was not found at "%PRESETS_FILE%".
    goto :fail
)

pushd "%ROOT_DIR%" >nul 2>nul
if errorlevel 1 (
    echo [ERROR] Failed to change directory to "%ROOT_DIR%".
    goto :fail
)

cmake --preset "%CONFIG_PRESET%" %* > "%LOG_FILE%" 2>&1
set "CMAKE_EXIT=%ERRORLEVEL%"
popd >nul

if not "%CMAKE_EXIT%"=="0" goto :fail

echo [OK] Configure succeeded.
call "%~dp0finish_windows_script.bat" 0
exit /b 0

:fail
echo [ERROR] Configure failed. Review "%LOG_FILE%".
call "%~dp0finish_windows_script.bat" 1
exit /b 1
