@echo off

call "%VS100COMNTOOLS%vsvars32.bat" || goto fail

del /f /q "debug\@@APP_NAME@@.exe" > nul
del /f /q "debug64\@@APP_NAME@@.exe" > nul

call devenv /nologo project.sln /build "Debug|Win32" > build.x86.log || goto fail
call devenv /nologo project.sln /build "Debug|x64" > build.x64.log || goto fail

rem mkdir "..\@@TESTCASE_NAME@@" > nul
rem cd "..\@@TESTCASE_NAME@@" || goto fail
rem copy "..\data.bin" . || goto fail
rem copy "..\src\debug\@@APP_NAME@@.exe" . || goto fail

echo OK
goto end

:fail
echo FAIL
:end

exit /b %ERRORLEVEL%
