@echo off
setlocal EnableExtensions
call "%~dp0common_windows.bat"
if errorlevel 1 goto :fail_init

set "LOG_FILE=%LOG_DIR%\build_vs2022.log"
if not exist "%BUILD_DIR%\MeridianRenderEngine.sln" (
    echo [INFO] Build tree not found. Running configure first...
    set "MERIDIAN_INTERNAL_CHAIN=1"
    call "%~dp0configure_vs2022.bat"
    if errorlevel 1 goto :fail
)

echo [INFO] Building MeridianRenderEngine (%BUILD_CONFIG%).
echo [INFO] Log    : "%LOG_FILE%"
cmake --build "%BUILD_DIR%" --config "%BUILD_CONFIG%" > "%LOG_FILE%" 2>&1
if errorlevel 1 goto :fail

echo [OK] Build succeeded.
call "%~dp0finish_windows_script.bat" 0
exit /b 0

:fail_init
set "LOG_FILE=%LOG_DIR%\build_vs2022.log"
:fail
echo [ERROR] Build failed. Review "%LOG_FILE%".
call "%~dp0finish_windows_script.bat" 1
exit /b 1
