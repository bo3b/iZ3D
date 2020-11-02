for /f "tokens=2*" %%i in ('reg query "HKLM\SOFTWARE\Microsoft\VisualStudio\9.0" /v InstallDir') do SET vsPath=%%~dpj
set PATH=%PATH%;%vsPath%
echo Start time %TIME%
devenv.exe S3DDriver.sln /rebuild "Final Release|Win32" > Win32.log
devenv.exe S3DDriver.sln /rebuild "Final Release|x64" > x64.log
echo End time %TIME%