@echo off

robocopy "P:\Colorful-UI\Scripts\3_Game" "P:\Colorful-UI\Scripts\3_Game\Colorful-UI" /E /MOV >nul
if exist "P:\Colorful-UI\Scripts\3_Game\Colorful-UI\Colorful-UI" (
    move "P:\Colorful-UI\Scripts\3_Game\Colorful-UI\Colorful-UI\*" "P:\Colorful-UI\Scripts\3_Game\Colorful-UI" >nul
    rd /s /q "P:\Colorful-UI\Scripts\3_Game\Colorful-UI\Colorful-UI" >nul
)
for /d %%i in ("P:\Colorful-UI\Scripts\3_Game\*") do (
    if not "%%~nxi"=="Colorful-UI" (
        rd /s /q "%%i" >nul
    )
)

robocopy "P:\Colorful-UI\Scripts\4_World" "P:\Colorful-UI\Scripts\4_World\Colorful-UI" /E /MOV >nul
if exist "P:\Colorful-UI\Scripts\4_World\Colorful-UI\Colorful-UI" (
    move "P:\Colorful-UI\Scripts\4_World\Colorful-UI\Colorful-UI\*" "P:\Colorful-UI\Scripts\4_World\Colorful-UI" >nul
    rd /s /q "P:\Colorful-UI\Scripts\4_World\Colorful-UI\Colorful-UI" >nul
)
for /d %%i in ("P:\Colorful-UI\Scripts\4_World\*") do (
    if not "%%~nxi"=="Colorful-UI" (
        rd /s /q "%%i" >nul
    )
)

robocopy "P:\Colorful-UI\Scripts\5_Mission" "P:\Colorful-UI\Scripts\5_Mission\Colorful-UI" /E /MOV >nul
if exist "P:\Colorful-UI\Scripts\5_Mission\Colorful-UI\Colorful-UI" (
    move "P:\Colorful-UI\Scripts\5_Mission\Colorful-UI\Colorful-UI\*" "P:\Colorful-UI\Scripts\5_Mission\Colorful-UI" >nul
    rd /s /q "P:\Colorful-UI\Scripts\5_Mission\Colorful-UI\Colorful-UI" >nul
)
for /d %%i in ("P:\Colorful-UI\Scripts\5_Mission\*") do (
    if not "%%~nxi"=="Colorful-UI" (
        rd /s /q "%%i" >nul
    )
)

echo Folders created and contents moved successfully.
pause