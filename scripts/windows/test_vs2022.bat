@echo off
setlocal EnableExtensions
call "%~dp0common_windows.bat"
if errorlevel 1 goto :fail_init

set "LOG_FILE=%LOG_DIR%\test_vs2022.log"
if not exist "%BUILD_DIR%\MeridianRenderEngine.sln" (
    echo [INFO] Build tree not found. Running configure first...
    set "MERIDIAN_INTERNAL_CHAIN=1"
    call "%~dp0configure_vs2022.bat"
    if errorlevel 1 goto :fail
)

if not exist "%BUILD_DIR%\Debug\MeridianUnitTests.exe" (
    echo [INFO] Test binaries not found. Running build first...
    set "MERIDIAN_INTERNAL_CHAIN=1"
    call "%~dp0build_vs2022.bat"
    if errorlevel 1 goto :fail
)

echo [INFO] Running tests (%BUILD_CONFIG%).
echo [INFO] Log    : "%LOG_FILE%"
ctest --test-dir "%BUILD_DIR%" -C "%BUILD_CONFIG%" --output-on-failure > "%LOG_FILE%" 2>&1
if errorlevel 1 goto :fail

echo [OK] Tests passed.
call "%~dp0finish_windows_script.bat" 0
exit /b 0

:fail_init
set "LOG_FILE=%LOG_DIR%\test_vs2022.log"
:fail
echo [ERROR] Tests failed. Review "%LOG_FILE%".
call "%~dp0finish_windows_script.bat" 1
exit /b 1
