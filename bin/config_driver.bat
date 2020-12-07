@if "%WINDDKDIR%"=="" set WINDDKDIR=C:\WinDDK\6001.18001

@set PATH=%PATH%;%WINDDKDIR%\bin\SelfSign;..\..\..\lib\madCHook\

cd "..\bin\final release\Win32\"
if exist "S3DInjectionDriver.sys" goto iZ3DWin64
copy "..\..\..\lib\madCHook\madCHookDriver32.sys" "S3DInjectionDriver.sys"
madConfigDrv.exe S3DInjectionDriver.sys iZ3DInjectionDriver S3DInjector.dll S3DOGLInjector.dll -stopAllowed
signtool sign /v /ac ..\..\..\lib\sign\MSCV-VSClass3.cer /s My /n iZ3D /t http://timestamp.verisign.com/scripts/timstamp.dll S3DInjectionDriver.sys

:iZ3DWin64
cd "..\Win64\"
if exist "S3DInjectionDriver.sys" goto end
copy "..\..\..\lib\madCHook\madCHookDriver64.sys" "S3DInjectionDriver.sys"
madConfigDrv.exe S3DInjectionDriver.sys iZ3DInjectionDriver S3DInjector.dll S3DOGLInjector.dll ..\Win32\S3DInjector.dll ..\Win32\S3DOGLInjector.dll -stopAllowed
signtool sign /v /ac ..\..\..\lib\sign\MSCV-VSClass3.cer /s My /n iZ3D /t http://timestamp.verisign.com/scripts/timstamp.dll S3DInjectionDriver.sys

:SapphireWin32
cd "..\..\final release - sapphire\Win32\"
if exist "S3DInjectionDriver.sys" goto SapphireWin64
copy "..\..\..\lib\madCHook\madCHookDriver32.sys" "S3DInjectionDriver.sys"
madConfigDrv.exe S3DInjectionDriver.sys iZ3DInjectionDriver S3DInjector.dll S3DOGLInjector.dll -stopAllowed

:SapphireWin64
cd "..\Win64\"
if exist "S3DInjectionDriver.sys" goto end
copy "..\..\..\lib\madCHook\madCHookDriver64.sys" "S3DInjectionDriver.sys"
madConfigDrv.exe S3DInjectionDriver.sys iZ3DInjectionDriver S3DInjector.dll S3DOGLInjector.dll ..\Win32\S3DInjector.dll ..\Win32\S3DOGLInjector.dll -stopAllowed
:end

cd ..\..
