@echo off
@set PATH=%PATH%;C:/Work/UniversalIndentGUI/indenters/
cd ..
REM Run Astyle to format source code
REM Get Astyle from:
REM http://astyle.sourceforge.net/
REM $Id$

REM -in_cfg-"c:\Work\gc.cfg"
SET GREATECODEPARAMS=-no-bak-

For /F "Delims=" %%i In ('"dir /b /AD-H"') Do (
	echo %%i
	FOR /F "tokens=*" %%G IN ('DIR /B %%i\*.cpp') DO (
		echo Indenting file "%%i\%%G"
		greatcode %GREATECODEPARAMS% -file-"%%i\%%G"
	)
	FOR /F "tokens=*" %%G IN ('DIR /B %%i\*.h') DO (
		echo Indenting file "%%i\%%G"
		greatcode %GREATECODEPARAMS% -file-"%%i\%%G"
	)
)

For /F "Delims=" %%i In ('"dir "DTest\" /s /b /AD-H"') Do (
	echo %%i
	FOR /F "tokens=*" %%G IN ('DIR /B %%i\*.cpp') DO (
		echo Indenting file "%%i\%%G"
		greatcode %GREATECODEPARAMS% -file-"%%i\%%G"
	)
	FOR /F "tokens=*" %%G IN ('DIR /B %%i\*.h') DO (
		echo Indenting file "%%i\%%G"
		greatcode %GREATECODEPARAMS% -file-"%%i\%%G"
	)
)

For /F "Delims=" %%i In ('"dir "TestsPack\" /s /b /AD-H"') Do (
	echo %%i
	FOR /F "tokens=*" %%G IN ('DIR /B %%i\*.cpp') DO (
		echo Indenting file "%%i\%%G"
		greatcode %GREATECODEPARAMS% -file-"%%i\%%G"
	)
	FOR /F "tokens=*" %%G IN ('DIR /B %%i\*.h') DO (
		echo Indenting file "%%i\%%G"
		greatcode %GREATECODEPARAMS% -file-"%%i\%%G"
	)
)

For %%i In (Control Center\guilib\, Control Center\iz3d\) Do (
	echo %%i
	FOR /F "tokens=*" %%G IN ('DIR /B %%i\*.cpp') DO (
		echo Indenting file "%%i\%%G"
		greatcode %GREATECODEPARAMS% -file-"%%i\%%G"
	)
	FOR /F "tokens=*" %%G IN ('DIR /B %%i\*.h') DO (
		echo Indenting file "%%i\%%G"
		greatcode %GREATECODEPARAMS% -file-"%%i\%%G"
	)
)

For /F "Delims=" %%i In ('"dir OutputMethods\ /b /AD-H"') Do (
	echo %%i
	FOR /F "tokens=*" %%G IN ('DIR /B OutputMethods\%%i\*.cpp') DO (
		echo Indenting file "OutputMethods\%%i\%%G"
		greatcode %GREATECODEPARAMS% -file-"OutputMethods\%%i\%%G"
	)
	FOR /F "tokens=*" %%G IN ('DIR /B OutputMethods\%%i\*.h') DO (
		echo Indenting file "OutputMethods\%%i\%%G"
		greatcode %GREATECODEPARAMS% -file-"OutputMethods\%%i\%%G"
	)
)


For %%i In (S3DWrapper10\Commands, S3DWrapper10\Streamer) Do (
	echo %%i
	FOR /F "tokens=*" %%G IN ('DIR /B %%i\*.cpp') DO (
		echo Indenting file "%%i\%%G"
		greatcode %GREATECODEPARAMS% -file-"%%i\%%G"
	)
	FOR /F "tokens=*" %%G IN ('DIR /B %%i\*.h') DO (
		echo Indenting file "%%i\%%G"
		greatcode %GREATECODEPARAMS% -file-"%%i\%%G"
	)
)

cd bin
