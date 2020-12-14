@ECHO OFF

SET ORIGPATH="%CD%"
CD %~dp0/../..

rem this python mechanism was to fetch the branch name as text.
rem  but we no longer have a svn repo, we have git repo.  For now
rem  let's just hard code it.  ToDo.
rem set BRANCH_NAME=
rem lib\python27\python BuildSetup\SetBranchFolder\SetBranchFolder.py
rem call setbranchname.bat
rem del setbranchname.bat
set BRANCH_NAME=master

cd S3DDriverSetup
..\lib\InnoSetup\ISCC.exe SetupScript.iss /dsetupType="Anaglyph" "/dOUTDIR=..\bin\Final Release - Anaglyph" "/dFILESDIR=..\bin\Final Release" /O"..\BuildSetup\AnaglyphEdition\%BRANCH_NAME%"

CD %ORIGPATH%
exit /b %ERRORLEVEL%