@echo off 
@set PATH=%PATH%;%ProgramFiles%\TortoiseGit\bin
@GitWCRev.exe .. temp_CreateFullTestTask.bat.templ ..\BuildSetup\CreateFullTestTask.bat
@GitWCRev.exe .. temp_version.templ temp_version.h
@GitWCRev.exe .. temp_version.cs.templ temp_version.cs
exit /B 0
