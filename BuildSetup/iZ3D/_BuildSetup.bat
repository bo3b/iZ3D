@ECHO OFF

SET ORIGPATH="%CD%"
CD %~dp0/../..

set BRANCH_NAME=
lib\python27\python BuildSetup\SetBranchFolder\SetBranchFolder.py
call setbranchname.bat
del setbranchname.bat

cd S3DDriverSetup
..\lib\InnoSetup\ISCC.exe SetupScript.iss /dsetupType="iZ3D" "/dOUTDIR=..\bin\Final Release" /O"..\BuildSetup\iZ3D\%BRANCH_NAME%"

CD %ORIGPATH%
exit /b %ERRORLEVEL%