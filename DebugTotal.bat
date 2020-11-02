for /f "tokens=2*" %%i in ('reg query "HKLM\SOFTWARE\Microsoft\VisualStudio\9.0" /v InstallDir') do SET vsPath=%%~dpj
set PATH=%PATH%;%vsPath%
echo Start time %TIME%
devenv S3DDriver.sln /rebuild "Debug|Win32"
devenv S3DDriver.sln /rebuild "Debug|x64"
echo End time %TIME%