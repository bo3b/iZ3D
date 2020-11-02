@ECHO OFF

SET DriverEdition=%1
SET FTPDriverFolder=%2
SET BRANCH=
IF NOT "%BRANCH_NAME%"=="" SET BRANCH=%BRANCH_NAME:~0,-1%

FOR /F "delims=" %%I IN ('DIR %DriverEdition%\%BRANCH_NAME%*.exe /B /O:D') DO SET NewestFile=%%I
call UploadDriver %FTPDriverFolder% "%DriverEdition%\%BRANCH_NAME%%NewestFile%" %BRANCH%
IF "%NewestFile%"=="" GOTO EndBad

set DRIVERURL=ftp://alterego-ftp@ftp.neurok.ru/pub/IX/iZ3DDriverVersions/%FTPDriverFolder%/%NewestFile%
IF NOT "%BRANCH%"=="" set DRIVERURL=ftp://alterego-ftp@ftp.neurok.ru/pub/IX/iZ3DDriverVersions/%FTPDriverFolder%/%BRANCH%/%NewestFile%
echo url: "%DRIVERURL%"

del svn_log.xml /q

svn log .. -r BASE:1 -l 10 -v --xml > svn_log.xml
IF %ERRORLEVEL% NEQ 0 GOTO SendWithoutAttach

"%~dp0..\bin\others\SVNLog2XHTML" svn_log.xml svn_log.html
IF %ERRORLEVEL% NEQ 0 GOTO SendWithoutAttach

"%~dp0..\bin\others\blat" - -subject "%DriverEdition%: %NewestFile%" -body "%USERDOMAIN%: %DRIVERURL%" -to testers@iz3dru.com -cc developers@iz3dru.com,asadov@iz3dru.com,autotest@iz3dru.com -charset utf-8 -html -attachi "svn_log.html" -f developers@iz3dru.com -server neurok.ru

del svn_log.xml /q
del svn_log.html /q

GOTO EndGood

:SendWithoutAttach
"%~dp0..\bin\others\blat" - -subject "%DriverEdition%: %NewestFile%" -body "%USERDOMAIN%: %DRIVERURL%" -to testers@iz3dru.com -cc developers@iz3dru.com,asadov@iz3dru.com -bcc autotest@iz3dru.com -charset utf-8 -f developers@iz3dru.com -server neurok.ru

GOTO EndGood

:EndBad
ECHO: ERROR: Build failed and aborted

:EndGood
