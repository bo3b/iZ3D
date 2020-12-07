@echo off
FOR %%i IN (debug, release) DO (
	echo %%i
	FOR %%j IN (Win32, Win64) DO (
		FOR %%k IN (logs, logs_dx7, logs_dx8, logs_dx10) DO (
			echo %%i\%%j\%%k
			rd /S /Q %%i\%%j\%%k
		)
	)
)