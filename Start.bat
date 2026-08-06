@echo off
setlocal EnableExtensions
title Colorful UI - P: drive link setup

rem ============================================================================
rem  Colorful UI - project link setup
rem  Links P:\Colorful-UI to this repo's Colorful-UI folder, wherever this
rem  repo is cloned. Safe to re-run any time (it only ever replaces LINKS -
rem  it will never delete a real folder).
rem ============================================================================

set "SRC=%~dp0Colorful-UI"
set "DEST=P:\Colorful-UI"

if not exist "%SRC%" (
    echo [FAIL] Cannot find "%SRC%".
    echo        Start.bat must sit in the repo root, next to the Colorful-UI folder.
    goto :end
)

rem --- Make sure P:\ is mounted (the DayZ Tools work drive) -------------------
if exist P:\ goto :pdrive_ok

echo P:\ is not mounted. Looking for the DayZ Tools work drive...
set "STEAM="
for /f "tokens=2,*" %%a in ('reg query "HKCU\Software\Valve\Steam" /v SteamPath 2^>nul ^| find /i "SteamPath"') do set "STEAM=%%b"
if defined STEAM set "STEAM=%STEAM:/=\%"
if defined STEAM if exist "%STEAM%\steamapps\common\DayZ Tools\Bin\Work" (
    subst P: "%STEAM%\steamapps\common\DayZ Tools\Bin\Work"
)
if not exist P:\ (
    echo [FAIL] Could not mount P:\ automatically.
    echo        Open DayZ Tools once so it mounts the work drive, then run this again.
    goto :end
)
echo [OK]   Mounted P:\
:pdrive_ok

rem --- Deal with whatever is currently at P:\Colorful-UI ----------------------
if not exist "%DEST%" goto :make_link

fsutil reparsepoint query "%DEST%" >nul 2>&1
if errorlevel 1 (
    echo [FAIL] "%DEST%" already exists and is a REAL folder, not a link.
    echo        This script never deletes real files. Move or delete that folder
    echo        yourself, then run this again.
    goto :end
)
echo        Removing old link at %DEST% ...
rmdir "%DEST%"

:make_link
mklink /J "%DEST%" "%SRC%"
if errorlevel 1 (
    echo [FAIL] Could not create the link.
    goto :end
)
echo.
echo [OK]   %DEST%  now points at  "%SRC%"
echo        Ready to build.

:end
echo.
pause
endlocal
