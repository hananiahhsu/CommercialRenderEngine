@echo off
setlocal EnableExtensions
set "MERIDIAN_INTERNAL_CHAIN=1"
call "%~dp0configure_vs2022.bat"
if errorlevel 1 goto :fail
call "%~dp0build_vs2022.bat"
if errorlevel 1 goto :fail
call "%~dp0test_vs2022.bat"
if errorlevel 1 goto :fail
call "%~dp0package_vs2022.bat"
if errorlevel 1 goto :fail

echo [OK] Configure + Build + Test + Package completed successfully.
endlocal
call "%~dp0finish_windows_script.bat" 0
exit /b 0

:fail
echo [ERROR] One or more steps failed. Review logs under out\logs.
endlocal
call "%~dp0finish_windows_script.bat" 1
exit /b 1
