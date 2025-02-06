@echo off
taskkill /F /IM DayZDiag_x64.exe
echo Task DayZDiag_x64 has been terminated.
SETLOCAL

:: These are the paths to the tools you will use to build the mods
:: If you want to use Mikero's tools, you need to install them first, then swap the paths variables
SET MakePboPath="C:\Program Files (x86)\Mikero\DePboTools\bin\MakePbo.exe"
SET AddonBuilderPath="C:\Program Files (x86)\Steam\steamapps\common\DayZ Tools\Bin\AddonBuilder\AddonBuilder.exe"

:: Set the source and output directories
SET ClientSource="P:\Colorful-UI"
SET ClientOutput="P:\Mods\@Colorful-UI\Addons"

:: Below is the command to pack the mod files, you can change the parameters as needed
%AddonBuilderPath% %ClientSource% %ClientOutput% -packonly -clear

:: Check the exit code of the last command
IF %ERRORLEVEL% NEQ 0 (
    powershell -Command "Write-Host 'Error: Mod build failed!' -ForegroundColor Red"
    pause
    exit /b %ERRORLEVEL%
) ELSE (
    powershell -Command "Write-Host 'All mods have been built from the C Drive.' -ForegroundColor Green"
    powershell -Command "Write-Host 'Please check your local server to verify.' -ForegroundColor DarkYellow"
    timeout /t 5 /nobreak
    exit
)
