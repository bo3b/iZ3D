set vsPath=''
for /f "tokens=2*" %%i in ('reg query "HKLM\SOFTWARE\Microsoft\VisualStudio\9.0" /v InstallDir') do SET vsPath=%%~dpj
set PATH=%PATH%;%vsPath%
set beginTime = %TIME%
devenv S3DDriver.sln /rebuild "Debug|x64"
set endTime = %TIME%
echo beginTime = %beginTime%
echo endTime = %endTime%