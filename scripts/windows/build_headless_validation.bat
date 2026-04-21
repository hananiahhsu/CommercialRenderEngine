@echo off
setlocal EnableExtensions
set "MERIDIAN_INTERNAL_CHAIN=1"
call "%~dp0common_windows.bat"
if errorlevel 1 goto :fail_init

set "RUN_LOG=%LOG_DIR%\headless_validation_vs2022.log"
call "%~dp0configure_vs2022.bat"
if errorlevel 1 goto :fail

call "%~dp0build_vs2022.bat"
if errorlevel 1 goto :fail

call "%~dp0test_vs2022.bat"
if errorlevel 1 goto :fail

if not exist "%BUILD_DIR%\Debug\MeridianHeadlessValidation.exe" (
    echo [ERROR] Headless validation executable was not produced.
    goto :fail
)

echo [INFO] Running headless validation.
echo [INFO] Log    : "%RUN_LOG%"
"%BUILD_DIR%\Debug\MeridianHeadlessValidation.exe" > "%RUN_LOG%" 2>&1
if errorlevel 1 goto :fail

echo [OK] Headless validation succeeded.
endlocal
call "%~dp0finish_windows_script.bat" 0
exit /b 0

:fail_init
set "RUN_LOG=%LOG_DIR%\headless_validation_vs2022.log"
:fail
echo [ERROR] Headless validation pipeline failed. Review logs under "%LOG_DIR%".
endlocal
call "%~dp0finish_windows_script.bat" 1
exit /b 1
