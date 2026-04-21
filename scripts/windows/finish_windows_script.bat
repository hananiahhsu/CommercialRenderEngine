@echo off
setlocal EnableExtensions
set "EXIT_CODE=%~1"
if "%EXIT_CODE%"=="" set "EXIT_CODE=0"

if not defined MERIDIAN_INTERNAL_CHAIN (
    if not "%MERIDIAN_NO_PAUSE%"=="1" (
        echo.
        if "%EXIT_CODE%"=="0" (
            echo [DONE] Press any key to close this window...
        ) else (
            echo [FAILED] Press any key to close this window...
        )
        pause >nul
    )
)

endlocal & exit /b %EXIT_CODE%
