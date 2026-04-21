@echo off
setlocal EnableExtensions
call "%~dp0common_windows.bat"
if errorlevel 1 goto :fail_init

set "INSTALL_LOG=%LOG_DIR%\install_vs2022.log"
set "PACKAGE_LOG=%LOG_DIR%\package_vs2022.log"
if not exist "%BUILD_DIR%\MeridianRenderEngine.sln" (
    echo [INFO] Build tree not found. Running configure first...
    set "MERIDIAN_INTERNAL_CHAIN=1"
    call "%~dp0configure_vs2022.bat"
    if errorlevel 1 goto :fail
)

if not exist "%BUILD_DIR%\Debug\MeridianHeadlessValidation.exe" (
    echo [INFO] Package inputs not found. Running build first...
    set "MERIDIAN_INTERNAL_CHAIN=1"
    call "%~dp0build_vs2022.bat"
    if errorlevel 1 goto :fail
)

if not exist "%INSTALL_DIR%" mkdir "%INSTALL_DIR%" >nul 2>nul
if not exist "%PACKAGE_DIR%" mkdir "%PACKAGE_DIR%" >nul 2>nul

echo [INFO] Installing build outputs.
echo [INFO] Install log : "%INSTALL_LOG%"
cmake --install "%BUILD_DIR%" --config "%BUILD_CONFIG%" --prefix "%INSTALL_DIR%" > "%INSTALL_LOG%" 2>&1
if errorlevel 1 goto :fail

echo [INFO] Packaging artifacts.
echo [INFO] Package log : "%PACKAGE_LOG%"
cpack --config "%BUILD_DIR%\CPackConfig.cmake" -C "%BUILD_CONFIG%" -B "%PACKAGE_DIR%" > "%PACKAGE_LOG%" 2>&1
if errorlevel 1 goto :fail

echo [OK] Package succeeded.
call "%~dp0finish_windows_script.bat" 0
exit /b 0

:fail_init
set "INSTALL_LOG=%LOG_DIR%\install_vs2022.log"
set "PACKAGE_LOG=%LOG_DIR%\package_vs2022.log"
:fail
echo [ERROR] Package step failed. Review "%INSTALL_LOG%" and "%PACKAGE_LOG%".
call "%~dp0finish_windows_script.bat" 1
exit /b 1
