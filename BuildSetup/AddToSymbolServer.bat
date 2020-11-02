@echo off
SETLOCAL
@set PATH=%PATH%;%ProgramFiles%\Debugging Tools for Windows (x86);%ProgramFiles%\Debugging Tools for Windows (x86)\srcsrv
rem You should install Debugging Tools for Windows (x86) before
rem http://msdl.microsoft.com/download/symbols/debuggers/dbg_x86_6.11.1.404.msi

@set CONF=%~1
IF "%CONF%"=="" set CONF=final release

:: Store start time
FOR /f "tokens=1-4 delims=:.," %%T IN ("%TIME%") DO (
	SET StartTIME=%TIME%
	:: Fix leading zero problem
	SET /a Start100S=%%T*360000+1%%U*6000+1%%V*100+1%%W - 610100
)

cd ..
svn > NUL
IF "%ERRORLEVEL%"=="9009" GOTO SkipSourceIndexing
rem You should install SlikSVN before http://www.sliksvn.com/en/download
echo Indexing source code...
call svnindex.cmd /symbols="bin\%CONF%" /ini=BuildSetup\srcsrv.ini -debug
:SkipSourceIndexing
cd bin

echo Adding symbols to server...
@set PARAMS=/s \\dev1\SymbolServer /t "iZ3DDriver"
echo %CONF%
symstore add /compress /f "%CONF%\*.*" %PARAMS%
IF "%ERRORLEVEL%"=="9009" GOTO DTWNotInstalled
FOR %%j IN (Win32, Win64) DO (
	echo %CONF%\%%j
	symstore add /compress /f "%CONF%\%%j\*.*" %PARAMS%
	echo %CONF%\%%j\OutputMethods
	symstore add /compress /f "%CONF%\%%j\OutputMethods\*.*" %PARAMS%
)

rem echo Add HTTP alias
rem walk.cmd *.pdb cv2http.cmd HTTP_ALIAS http://82.198.189.247:8080/source

:DTWNotInstalled
cd ..\BuildSetup


:: Retrieve Stop time
FOR /f "tokens=1-4 delims=:.," %%T IN ("%TIME%") DO (
	SET StopTIME=%TIME%
	:: Fix leading zero problem
	SET /a Stop100S=%%T*360000+1%%U*6000+1%%V*100+1%%W - 610100
)

:: Test midnight rollover. If so, add 1 day=8640000 1/100ths secs
IF %Stop100S% LSS %Start100S% SET /a Stop100S+=8640000
SET /a TookTimeSec=(%Stop100S%-%Start100S%)/100
SET /a TookTimeMin=TookTimeSec/60
SET /a TookTimeSec=%TookTimeSec%-%TookTimeMin%*60

ECHO Started: %StartTime%
ECHO Stopped: %StopTime%
ECHO Elapsed: %TookTimeMin% min. %TookTimeSec% sec.
