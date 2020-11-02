@ECHO OFF

SET ORIGPATH="%CD%"
CD %~dp0/../..

set BRANCH_NAME=
lib\python27\python BuildSetup\SetBranchFolder\SetBranchFolder.py
call setbranchname.bat
del setbranchname.bat

cd S3DDriverSetup
..\lib\InnoSetup\ISCC.exe SetupScript.iss /dsetupType="Anaglyph" "/dOUTDIR=..\bin\Final Release - Anaglyph" "/dFILESDIR=..\bin\Final Release" /O"..\BuildSetup\AnaglyphEdition\%BRANCH_NAME%"

CD %ORIGPATH%
exit /b %ERRORLEVEL%