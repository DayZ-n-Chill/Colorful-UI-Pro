@echo off

robocopy "P:\Colorful-UI\Scripts\3_Game\Colorful-UI" "P:\Colorful-UI\Scripts\3_Game" /E /MOV >nul
rd /s /q "P:\Colorful-UI\Scripts\3_Game\Colorful-UI" >nul

robocopy "P:\Colorful-UI\Scripts\4_World\Colorful-UI" "P:\Colorful-UI\Scripts\4_World" /E /MOV >nul
rd /s /q "P:\Colorful-UI\Scripts\4_World\Colorful-UI" >nul

robocopy "P:\Colorful-UI\Scripts\5_Mission\Colorful-UI" "P:\Colorful-UI\Scripts\5_Mission" /E /MOV >nul
rd /s /q "P:\Colorful-UI\Scripts\5_Mission\Colorful-UI" >nul

echo Folders unwrapped and contents moved back successfully.
pause
