@echo off 
@set PATH=%PATH%;%ProgramFiles%\TortoiseSVN\bin
@SubWCRev.exe .. temp_CreateFullTestTask.bat.templ ..\BuildSetup\CreateFullTestTask.bat
@SubWCRev.exe .. temp_version.templ temp_version.h
@SubWCRev.exe .. temp_version.cs.templ temp_version.cs
exit /B 0
