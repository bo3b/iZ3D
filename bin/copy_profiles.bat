@echo off

if not exist "..\bin\debug\Win32\" md "..\bin\debug\Win32\"
if not exist "..\bin\release\Win32\" md "..\bin\release\Win32\"
if not exist "..\bin\final release\Win32\" md "..\bin\final release\Win32\"
if not exist "..\bin\debug\Win64\" md "..\bin\debug\Win64\"
if not exist "..\bin\release\Win64\" md "..\bin\release\Win64\"
if not exist "..\bin\final release\Win64\" md "..\bin\final release\Win64\"
if not exist "..\bin\debug\Config.xml" "..\S3DDriverSetup\Content\bin\XmlPreprocess.exe" /c /q /nologo /i "..\bin\DebugConfig.xml" /o "..\bin\debug\Config.xml" 
if not exist "..\bin\release\Config.xml" "..\S3DDriverSetup\Content\bin\XmlPreprocess.exe" /c /q /nologo /i "..\bin\DebugConfig.xml" /o "..\bin\release\Config.xml"

if not exist "..\bin\final release\win32\d3d9VistaNoSP1.dll" copy "..\lib\d3d9\dll\x32\d3d9VistaNoSP1.dll" "..\bin\final release\win32\"
if not exist "..\bin\final release\win64\d3d9VistaNoSP1.dll" copy "..\lib\d3d9\dll\x64\d3d9VistaNoSP1.dll" "..\bin\final release\win64\"

if not exist "..\bin\debug\win32\S3DDevIL.dll" copy "..\lib\DevIL\lib\x86\*.dll" "..\bin\debug\win32\"
if not exist "..\bin\debug\win64\S3DDevIL.dll" copy "..\lib\DevIL\lib\x64\*.dll" "..\bin\debug\win64\"
if not exist "..\bin\release\win32\S3DDevIL.dll" copy "..\lib\DevIL\lib\x86\*.dll" "..\bin\release\win32\"
if not exist "..\bin\release\win64\S3DDevIL.dll" copy "..\lib\DevIL\lib\x64\*.dll" "..\bin\release\win64\"
if not exist "..\bin\final release\win32\S3DDevIL.dll" copy "..\lib\DevIL\lib\x86\*.dll" "..\bin\final release\win32\"
if not exist "..\bin\final release\win64\S3DDevIL.dll" copy "..\lib\DevIL\lib\x64\*.dll" "..\bin\final release\win64\"

if exist "..\bin\debug\win32\vld.dll" del "..\bin\debug\win32\vld.dll"
if exist "..\bin\debug\win64\vld.dll" del "..\bin\debug\win64\vld.dll"
if exist "..\bin\release\win32\vld.dll" del "..\bin\release\win32\vld.dll"
if exist "..\bin\release\win64\vld.dll" del "..\bin\release\win64\vld.dll"
if not exist "..\bin\debug\win32\vld_x86.dll" copy "..\lib\vld\dll\x86\*.*" "..\bin\debug\win32\"
if not exist "..\bin\debug\win64\vld_x64.dll" copy "..\lib\vld\dll\x64\*.*" "..\bin\debug\win64\"

for %%t in (..\bin\vld.ini) do set VLD_INI=%%~ft
if not exist "%VLD_INI%" copy "..\lib\vld\*.ini" "..\bin\"
reg add "HKCU\Software\Visual Leak Detector" /f
reg add "HKCU\Software\Visual Leak Detector" /f /v IniFile /t REG_SZ /d "%VLD_INI%"

if not exist "..\bin\final release\data\" (
   	md "..\bin\final release\data\"
	xcopy  /E "..\Control Center\data\iz3d" "..\bin\final release\data\"
)

if not exist "%USERPROFILE%\Application Data\iZ3D Driver\" md "%USERPROFILE%\Application Data\iZ3D Driver\"
if not exist "%ALLUSERSPROFILE%\Application Data\iZ3D Driver\" md "%ALLUSERSPROFILE%\Application Data\iZ3D Driver\"
if not exist "%ALLUSERSPROFILE%\Application Data\iZ3D Driver\Language" md "%ALLUSERSPROFILE%\Application Data\iZ3D Driver\Language"
if not exist "%ALLUSERSPROFILE%\Application Data\iZ3D Driver\Config.xml" copy "..\S3DDriverSetup\Config.xml" "%ALLUSERSPROFILE%\Application Data\iZ3D Driver\" /Y
if not exist "%ALLUSERSPROFILE%\Application Data\iZ3D Driver\BaseProfile.xml" copy "..\S3DDriverSetup\BaseProfile.xml" "%ALLUSERSPROFILE%\Application Data\iZ3D Driver\" /Y
copy "..\S3DDriverSetup\Content\language\*.xml" "%ALLUSERSPROFILE%\Application Data\iZ3D Driver\Language\" /Y

if not exist "..\bin\debug\Win32\bmfont.exe"    copy "..\S3DDriverSetup\Content\bin\bmfont.exe" "..\bin\debug\Win32\bmfont.exe"
if not exist "..\bin\release\Win32\bmfont.exe"  copy "..\S3DDriverSetup\Content\bin\bmfont.exe" "..\bin\release\Win32\bmfont.exe"
if not exist "..\bin\final release\Win32\bmfont.exe"  copy "..\S3DDriverSetup\Content\bin\bmfont.exe" "..\bin\final release\Win32\bmfont.exe"

if not exist "%ALLUSERSPROFILE%\Application Data\iZ3D Driver\DTest\" md "%ALLUSERSPROFILE%\Application Data\iZ3D Driver\DTest\"
if not exist "%ALLUSERSPROFILE%\Application Data\iZ3D Driver\DTest\data" md "%ALLUSERSPROFILE%\Application Data\iZ3D Driver\DTest\data"
if not exist "%ALLUSERSPROFILE%\Application Data\iZ3D Driver\DTest\data\models" md "%ALLUSERSPROFILE%\Application Data\iZ3D Driver\DTest\data\models"
if not exist "%ALLUSERSPROFILE%\Application Data\iZ3D Driver\DTest\data\models\logo" md "%ALLUSERSPROFILE%\Application Data\iZ3D Driver\DTest\data\models\logo"
if not exist "%ALLUSERSPROFILE%\Application Data\iZ3D Driver\DTest\data\models\logo\logo.x" copy "..\DTest\data\models\logo\logo.x" "%ALLUSERSPROFILE%\Application Data\iZ3D Driver\DTest\data\models\logo\logo.x" /Y
if not exist "%ALLUSERSPROFILE%\Application Data\iZ3D Driver\DTest\data\models\logo\logo.sdkmesh" copy "..\DTest\data\models\logo\logo.sdkmesh" "%ALLUSERSPROFILE%\Application Data\iZ3D Driver\DTest\data\models\logo\logo.sdkmesh" /Y
if not exist "%ALLUSERSPROFILE%\Application Data\iZ3D Driver\DTest\data\shaders" md "%ALLUSERSPROFILE%\Application Data\iZ3D Driver\DTest\data\shaders"
if not exist "%ALLUSERSPROFILE%\Application Data\iZ3D Driver\DTest\data\shaders\DTestDX10.fx" copy "..\DTest\data\shaders\DTestDX10.fx" "%ALLUSERSPROFILE%\Application Data\iZ3D Driver\DTest\data\shaders\DTestDX10.fx" /Y
if not exist "%ALLUSERSPROFILE%\Application Data\iZ3D Driver\DTest\data\shaders\DTestDX9.fx" copy "..\DTest\data\shaders\DTestDX9.fx" "%ALLUSERSPROFILE%\Application Data\iZ3D Driver\DTest\data\shaders\DTestDX9.fx" /Y

rem #############################################################################################################
rem #archiving all resources begin#

del "..\bin\resources.zip"
set searchdir="..\bin\resources\#%~n1"

for  %%i in (%searchdir%) do (
      	set configuration=%%~si
)

..\bin\tools\7z a -tzip -mx0 ..\bin\resources.zip -r ..\bin\resources\*.*  -x!.svn -x!#*

if exist %configuration% (
	echo "Patch dir found: %configuration%"
	..\bin\tools\7z a -tzip -mx0 ..\bin\resources.zip -r %configuration%\*.* -x!.svn
)

if not exist %configuration% (
	echo "Patch dir not found:  %configuration%"
)

copy "..\bin\resources.zip" "%ALLUSERSPROFILE%\Application Data\iZ3D Driver\resources.zip" /y
rem del "..\bin\resources.zip"

rem #archiving all resources end#
rem #############################################################################################################

SET ORIGPATH="%CD%"
cd "final release\Win32\"
if exist S3DUtils.dll rundll32 S3DUtils.dll,GenerateFont
if exist S3DUtils.dll rundll32 S3DUtils.dll,GetAndWriteToRegistryDriverModuleName
cd "..\Win64\"
if exist S3DUtils.dll rundll32 S3DUtils.dll,GetAndWriteToRegistryDriverModuleName
CD %ORIGPATH%
