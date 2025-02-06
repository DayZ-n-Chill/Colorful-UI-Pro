@echo off

SET "ASCIIARTPATH=.\..\Shared\Branding.txt"
SET "COLORS=Blue,Green,Cyan,DarkBlue,DarkGreen,DarkCyan,DarkRed,DarkMagenta,DarkYellow"
powershell -Command "$colors = '%COLORS%'.Split(','); $randomColor = Get-Random -InputObject $colors; Get-Content -Path '%ASCIIARTPATH%' | ForEach-Object {Write-Host $_ -ForegroundColor $randomColor}"

powershell -Command "Write-Host 'Starting the process...' -ForegroundColor Magenta"

powershell -Command "Write-Host 'Processing 3_Game...' -ForegroundColor Yellow"
:process_3_Game
if exist "P:\Colorful-UI\Scripts\3_Game" (
    robocopy "P:\Colorful-UI\Scripts\3_Game" "P:\Colorful-UI\Scripts\3_Game\Colorful-UI" /E /MOV >nul 2>&1
    if exist "P:\Colorful-UI\Scripts\3_Game\Colorful-UI\Colorful-UI" (
        move "P:\Colorful-UI\Scripts\3_Game\Colorful-UI\Colorful-UI\*" "P:\Colorful-UI\Scripts\3_Game\Colorful-UI" >nul 2>&1
        rd /s /q "P:\Colorful-UI\Scripts\3_Game\Colorful-UI\Colorful-UI" >nul 2>&1
    )
    for /d %%i in ("P:\Colorful-UI\Scripts\3_Game\*") do (
        if not "%%~nxi"=="Colorful-UI" (
            attrib -h -s "%%i\*" /S /D >nul 2>&1
            rd /s /q "%%i" >nul 2>&1
        )
    )
)

powershell -Command "Write-Host 'Processing 4_World...' -ForegroundColor Yellow"
:process_4_World
if exist "P:\Colorful-UI\Scripts\4_World" (
    robocopy "P:\Colorful-UI\Scripts\4_World" "P:\Colorful-UI\Scripts\4_World\Colorful-UI" /E /MOV >nul 2>&1
    if exist "P:\Colorful-UI\Scripts\4_World\Colorful-UI\Colorful-UI" (
        move "P:\Colorful-UI\Scripts\4_World\Colorful-UI\Colorful-UI\*" "P:\Colorful-UI\Scripts\4_World\Colorful-UI" >nul 2>&1
        rd /s /q "P:\Colorful-UI\Scripts\4_World\Colorful-UI\Colorful-UI" >nul 2>&1
    )
    for /d %%i in ("P:\Colorful-UI\Scripts\4_World\*") do (
        if not "%%~nxi"=="Colorful-UI" (
            attrib -h -s "%%i\*" /S /D >nul 2>&1
            rd /s /q "%%i" >nul 2>&1
        )
    )
)

powershell -Command "Write-Host 'Processing 5_Mission...' -ForegroundColor Yellow"
:process_5_Mission
if exist "P:\Colorful-UI\Scripts\5_Mission" (
    robocopy "P:\Colorful-UI\Scripts\5_Mission" "P:\Colorful-UI\Scripts\5_Mission\Colorful-UI" /E /MOV >nul 2>&1
    if exist "P:\Colorful-UI\Scripts\5_Mission\Colorful-UI\Colorful-UI" (
        move "P:\Colorful-UI\Scripts\5_Mission\Colorful-UI\Colorful-UI\*" "P:\Colorful-UI\Scripts\5_Mission\Colorful-UI" >nul 2>&1
        rd /s /q "P:\Colorful-UI\Scripts\5_Mission\Colorful-UI\Colorful-UI" >nul 2>&1
    )
    for /d %%i in ("P:\Colorful-UI\Scripts\5_Mission\*") do (
        if not "%%~nxi"=="Colorful-UI" (
            attrib -h -s "%%i\*" /S /D >nul 2>&1
            rd /s /q "%%i" >nul 2>&1
        )
    )
)

powershell -Command "Write-Host 'Folders created and contents moved successfully!' -ForegroundColor Green"
pause
