@echo off

SET "ASCIIARTPATH=.\..\Shared\Branding.txt"
SET "COLORS=Blue,Green,Cyan,DarkBlue,DarkGreen,DarkCyan,DarkRed,DarkMagenta,DarkYellow"
powershell -Command "$colors = '%COLORS%'.Split(','); $randomColor = Get-Random -InputObject $colors; Get-Content -Path '%ASCIIARTPATH%' | ForEach-Object {Write-Host $_ -ForegroundColor $randomColor}"

robocopy "P:\Colorful-UI\Scripts\3_Game\Colorful-UI" "P:\Colorful-UI\Scripts\3_Game" /E /MOV >nul
rd /s /q "P:\Colorful-UI\Scripts\3_Game\Colorful-UI" >nul

robocopy "P:\Colorful-UI\Scripts\4_World\Colorful-UI" "P:\Colorful-UI\Scripts\4_World" /E /MOV >nul
rd /s /q "P:\Colorful-UI\Scripts\4_World\Colorful-UI" >nul

robocopy "P:\Colorful-UI\Scripts\5_Mission\Colorful-UI" "P:\Colorful-UI\Scripts\5_Mission" /E /MOV >nul
rd /s /q "P:\Colorful-UI\Scripts\5_Mission\Colorful-UI" >nul

powershell -Command "Write-Host 'Folders unwrapped and contents moved back successfully.' -ForegroundColor Green"

pause
