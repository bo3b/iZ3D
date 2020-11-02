@echo off
setlocal enabledelayedexpansion

rem -----------------------------------------

set APP_NAME=%1
set TIME2=%TIME::=.%
set TIME3=%TIME2:,=.%
set DIR_NAME=%1 TestCase %DATE% %TIME3%

set VCPROJ_OUTPUT=%APP_NAME%.vcproj
set SLN_OUTPUT=%APP_NAME%.sln
set MAKEFILE_OUTPUT=makefile
set BUILDDIR=Debug

rem -----------------------------------------
rem
rem проверка, установлениа ли студия и переменная окружения "VS90COMNTOOLS"
rem
if "%VS100COMNTOOLS%" == "" (
 	echo VC100 not installed on this computer, please install
 	exit /b %ERRORLEVEL%
)

rem -----------------------------------------
rem
rem проверка, установлениа ли студия и переменная окружения "DXSDK_DIR"
rem
if "%DXSDK_DIR%" == "" (
 	echo DXSDK not installed on this computer, please install
 	exit /b %ERRORLEVEL%
)

rem
rem получаем короткое имя текущей папки
rem
for  %%i in ("%cd%") do (
     	set SRCDIR=%%~si
)

rem
rem получаем короткое имя пути Visual Studio\VC
rem
for  %%i in ("%VS90COMNTOOLS%\..\..\VC") do (
     	set VCDIR=%%~si
)

rem
rem получаем короткое имя пути DirectX SDK
rem
for  %%i in ("%DXSDK_DIR%") do (
     	set DXSDKDIR=%%~si
)

rem -----------------------------------------


if "%STDOUT_REDIRECTED%" == "%MAKEFILE_OUTPUT%" (
	echo # Nmake macros for building Windows 32-Bit apps
	echo.
	echo ^^!include ^<win32.mak^>
        echo.
	echo OUTDIR = %BUILDDIR%
	echo PROJ   = %APP_NAME%
	echo CC_OPTIONS=/I"./global" /I"%DXSDKDIR%Include"
	echo LINK_OPTIONS = /MACHINE:X86 /SUBSYSTEM:WINDOWS /LIBPATH:%DXSDKDIR%Lib\x86 /OUT:$^(OUTDIR^)\$^(PROJ^).exe
        echo.
	rem
	rem  получаем все *.cpp - файлы в дирректории
	rem
	for /R %%i in ("*.cpp") do (
		if "!CPP_FILE!"=="" (
			echo CPP_FILE_SET = \
		)
		set CPP_FILE=%%~si
		set CPP_FILE=!CPP_FILE:%SRCDIR%=.!
		echo 		!CPP_FILE! \
	)
        echo.
	echo.
	
	rem
	rem  получаем все *.obj - файлы в дирректории
	rem
	for /R %%i in ("*.cpp") do (
		if "!TEMP_OBJ_FILE!"=="" (
			echo OBJ_FILE_LIST = \
		)		
		set TEMP_OBJ_FILE=%%~si
		set TEMP_OBJ_FILE=!TEMP_OBJ_FILE:%SRCDIR%=.!
		set TEMP_OBJ_FILE=!TEMP_OBJ_FILE:cpp=obj!
		echo 		!TEMP_OBJ_FILE! \
	)
        echo.
        
        
    rem
	rem  получаем все *.obj (просто список)
	rem
	for /R %%i in ("*.cpp") do (
		if "!OBJ_FILE!"=="" (
			echo OBJ_FILE_OUT = \
		)		
		set OBJ_FILE=%%~snxi		
		set OBJ_FILE=!OBJ_FILE:cpp=obj!
		set OBJ_FILE=.\%BUILDDIR%\!OBJ_FILE!
		echo 		!OBJ_FILE! \
	)
        echo.
        
       
        
	echo.   
	echo all: $^(OUTDIR^) $^(OUTDIR^)\$^(PROJ^).exe
	echo.
	echo #----- If OUTDIR does not exist, then create directory
	echo $^(OUTDIR^):
	echo		if not exist "$(OUTDIR)/$(NULL)" mkdir $^(OUTDIR^)
	echo.
	echo.
	echo # Inference rule for updating the object files
	echo .c.obj:
  	echo		$^(cc^) /D "_UNICODE" /D "UNICODE" /Fo"$(OUTDIR)\\" /Fd"$(OUTDIR)\\" $^(cdebug^) $^(cflags^) $^(cvars^) $^(CC_OPTIONS^) $*.c
	echo.
	echo .cpp.obj:
	echo		$^(cc^) /D "_UNICODE" /D "UNICODE" /Fo"$(OUTDIR)\\" /Fd"$(OUTDIR)\\" $^(cdebug^) $^(cflags^) $^(cvars^) $^(CC_OPTIONS^) $*.cpp
	echo.
	echo.
	echo # Update the executable file if necessary, and if so, add the resource back in.
	echo $^(OUTDIR^)\$^(PROJ^).exe: $^(OBJ_FILE_LIST^)
	echo		$^(link^) $^(LINK_OPTIONS^) $^(OBJ_FILE_OUT^) d3d10.lib d3dx10.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib
	echo.
	echo.
	echo # Clean up everything except .exe
	echo clean:
	echo		del *.obj *.res

        @echo off	
	exit /b %ERRORLEVEL%
)

rem -----------------------------------------

if "%STDOUT_REDIRECTED%" == "%VCPROJ_OUTPUT%" (
	echo ^<?xml version="1.0" encoding="windows-1251"?^>
	echo ^<VisualStudioProject
	echo 	^ProjectType="Visual C++"
	echo 	^Version="9,00"
	echo 	^Name="%APP_NAME%"
	echo 	^ProjectGUID="{1BAD8409-F433-4B12-9563-531AD500FE7C}"
	echo 	^RootNamespace="%APP_NAME%"
	echo 	^Keyword="Win32Proj"
	echo 	^>
	echo		^<Platforms^>
	echo			^<Platform
	echo				Name="Win32"
	echo			/^>
	echo			^<Platform
	echo				Name="x64"
	echo			/^>
	echo		^</Platforms^>
	echo		^<ToolFiles^>
	echo		^</ToolFiles^>
	echo		^<Configurations^>
	echo			^<Configuration
	echo				Name="Debug|Win32"
	echo				OutputDirectory="$(SolutionDir)$(ConfigurationName)"
	echo				IntermediateDirectory="$(ConfigurationName)"
	echo				ConfigurationType="1"
	echo				CharacterSet="1"
	echo				^>
	echo				^<Tool
	echo					Name="VCPreBuildEventTool"
	echo				/^>
	echo				^<Tool
	echo					Name="VCCustomBuildTool"
	echo				/^>
	echo				^<Tool
	echo					Name="VCXMLDataGeneratorTool"
	echo				/^>
	echo				^<Tool
	echo					Name="VCWebServiceProxyGeneratorTool"
	echo				/^>
	echo				^<Tool
	echo					Name="VCMIDLTool"
	echo				/^>
	echo				^<Tool
	echo					Name="VCCLCompilerTool"
	echo					Optimization="0"
	echo					PreprocessorDefinitions="WIN32;_DEBUG;_WINDOWS"
	echo					AdditionalOptions="/MP"
	echo					BasicRuntimeChecks="3"
	echo					RuntimeLibrary="1"
	echo					UsePrecompiledHeader="0"
	echo					WarningLevel="3"
	echo					DebugInformationFormat="4"
	echo				/^>
	echo				^<Tool
	echo					Name="VCManagedResourceCompilerTool"
	echo				/^>
	echo				^<Tool
	echo					Name="VCResourceCompilerTool"
	echo				/^>
	echo				^<Tool
	echo					Name="VCPreLinkEventTool"
	echo				/^>
	echo				^<Tool
	echo					Name="VCLinkerTool"
	echo					LinkIncremental="2"
	echo					GenerateDebugInformation="true"
	echo					SubSystem="2"
	echo					LargeAddressAware="2"
	echo					TargetMachine="1"
	echo				/^>
	echo				^<Tool
	echo					Name="VCALinkTool"
	echo				/^>
	echo				^<Tool
	echo					Name="VCManifestTool"
	echo				/^>
	echo				^<Tool
	echo					Name="VCXDCMakeTool"
	echo				/^>
	echo				^<Tool
	echo					Name="VCBscMakeTool"
	echo				/^>
	echo				^<Tool
	echo					Name="VCFxCopTool"
	echo				/^>
	echo				^<Tool
	echo					Name="VCAppVerifierTool"
	echo				/^>
	echo				^<Tool
	echo					Name="VCWebDeploymentTool"
	echo				/^>
	echo				^<Tool
	echo					Name="VCPostBuildEventTool"
	echo				/^>
	echo			^</Configuration^>	
    echo			^<Configuration
	echo				Name="Debug|x64"
	echo				OutputDirectory="$(SolutionDir)$(ConfigurationName)64"
	echo				IntermediateDirectory="$(ConfigurationName)64"
	echo				ConfigurationType="1"
	echo				CharacterSet="1"
	echo				^>
	echo				^<Tool
	echo					Name="VCPreBuildEventTool"
	echo				/^>
	echo				^<Tool
	echo					Name="VCCustomBuildTool"
	echo				/^>
	echo				^<Tool
	echo					Name="VCXMLDataGeneratorTool"
	echo				/^>
	echo				^<Tool
	echo					Name="VCWebServiceProxyGeneratorTool"
	echo				/^>
	echo				^<Tool
	echo					Name="VCMIDLTool"
	echo				/^>
	echo				^<Tool
	echo					Name="VCCLCompilerTool"
	echo					AdditionalOptions="/MP"
	echo					Optimization="0"
	echo					PreprocessorDefinitions="WIN32;_DEBUG;_WINDOWS;WIN64"
	echo					BasicRuntimeChecks="3"
	echo					RuntimeLibrary="1"
	echo					UsePrecompiledHeader="0"
	echo					WarningLevel="3"
	echo					DebugInformationFormat="4"
	echo				/^>
	echo				^<Tool
	echo					Name="VCManagedResourceCompilerTool"
	echo				/^>
	echo				^<Tool
	echo					Name="VCResourceCompilerTool"
	echo				/^>
	echo				^<Tool
	echo					Name="VCPreLinkEventTool"
	echo				/^>
	echo				^<Tool
	echo					Name="VCLinkerTool"
	echo					LinkIncremental="2"
	echo					GenerateDebugInformation="true"
	echo					SubSystem="2"
	echo					LargeAddressAware="2"
	echo					TargetMachine="17"
	echo				/^>
	echo				^<Tool
	echo					Name="VCALinkTool"
	echo				/^>
	echo				^<Tool
	echo					Name="VCManifestTool"
	echo				/^>
	echo				^<Tool
	echo					Name="VCXDCMakeTool"
	echo				/^>
	echo				^<Tool
	echo					Name="VCBscMakeTool"
	echo				/^>
	echo				^<Tool
	echo					Name="VCFxCopTool"
	echo				/^>
	echo				^<Tool
	echo					Name="VCAppVerifierTool"
	echo				/^>
	echo				^<Tool
	echo					Name="VCWebDeploymentTool"
	echo				/^>
	echo				^<Tool
	echo					Name="VCPostBuildEventTool"
	echo				/^>
	echo			^</Configuration^>
	echo			^<Configuration
	echo				Name="Release|Win32"
	echo				OutputDirectory="$(SolutionDir)$(ConfigurationName)"
	echo				IntermediateDirectory="$(ConfigurationName)"
	echo				ConfigurationType="1"
	echo				CharacterSet="1"
	echo				WholeProgramOptimization="0"
	echo				^>
	echo				^<Tool
	echo					Name="VCPreBuildEventTool"
	echo				/^>
	echo				^<Tool
	echo					Name="VCCustomBuildTool"
	echo				/^>
	echo				^<Tool
	echo					Name="VCXMLDataGeneratorTool"
	echo				/^>
	echo				^<Tool
	echo					Name="VCWebServiceProxyGeneratorTool"
	echo				/^>
	echo				^<Tool
	echo					Name="VCMIDLTool"
	echo				/^>
	echo				^<Tool
	echo					Name="VCCLCompilerTool"
	echo					AdditionalOptions="/MP"
	echo					PreprocessorDefinitions="WIN32;NDEBUG;_WINDOWS"
	echo					RuntimeLibrary="0"
	echo					BufferSecurityCheck="false"
	echo					UsePrecompiledHeader="0"
	echo					WarningLevel="3"
	echo					DebugInformationFormat="3"
	echo				/^>
	echo				^<Tool
	echo					Name="VCManagedResourceCompilerTool"
	echo				/^>
	echo				^<Tool
	echo					Name="VCResourceCompilerTool"
	echo				/^>
	echo				^<Tool
	echo					Name="VCPreLinkEventTool"
	echo				/^>
	echo				^<Tool
	echo					Name="VCLinkerTool"
	echo					LinkIncremental="1"
	echo					GenerateDebugInformation="true"
	echo					SubSystem="2"
	echo					LargeAddressAware="2"
	echo					OptimizeReferences="2"
	echo					EnableCOMDATFolding="2"
	echo					TargetMachine="1"
	echo				/^>
	echo				^<Tool
	echo					Name="VCALinkTool"
	echo				/^>
	echo				^<Tool
	echo					Name="VCManifestTool"
	echo				/^>
	echo				^<Tool
	echo					Name="VCXDCMakeTool"
	echo				/^>
	echo				^<Tool
	echo					Name="VCBscMakeTool"
	echo				/^>
	echo				^<Tool
	echo					Name="VCFxCopTool"
	echo				/^>
	echo				^<Tool
	echo					Name="VCAppVerifierTool"
	echo				/^>
	echo				^<Tool
	echo					Name="VCWebDeploymentTool"
	echo				/^>
	echo				^<Tool
	echo					Name="VCPostBuildEventTool"
	echo				/^>
	echo			^</Configuration^>
	echo			^<Configuration
	echo				Name="Release|x64"
	echo				OutputDirectory="$(SolutionDir)$(ConfigurationName)64"
	echo				IntermediateDirectory="$(ConfigurationName)64"
	echo				ConfigurationType="1"
	echo				CharacterSet="1"
	echo				WholeProgramOptimization="0"
	echo				^>
	echo				^<Tool
	echo					Name="VCPreBuildEventTool"
	echo				/^>
	echo				^<Tool
	echo					Name="VCCustomBuildTool"
	echo				/^>
	echo				^<Tool
	echo					Name="VCXMLDataGeneratorTool"
	echo				/^>
	echo				^<Tool
	echo					Name="VCWebServiceProxyGeneratorTool"
	echo				/^>
	echo				^<Tool
	echo					Name="VCMIDLTool"
	echo				/^>
	echo				^<Tool
	echo					Name="VCCLCompilerTool"
	echo					AdditionalOptions="/MP"
	echo					PreprocessorDefinitions="WIN32;NDEBUG;_WINDOWS;WIN64"
	echo					RuntimeLibrary="0"
	echo					BufferSecurityCheck="false"
	echo					UsePrecompiledHeader="0"
	echo					WarningLevel="3"
	echo					DebugInformationFormat="3"
	echo				/^>
	echo				^<Tool
	echo					Name="VCManagedResourceCompilerTool"
	echo				/^>
	echo				^<Tool
	echo					Name="VCResourceCompilerTool"
	echo				/^>
	echo				^<Tool
	echo					Name="VCPreLinkEventTool"
	echo				/^>
	echo				^<Tool
	echo					Name="VCLinkerTool"
	echo					LinkIncremental="1"
	echo					GenerateDebugInformation="true"
	echo					SubSystem="2"
	echo					LargeAddressAware="2"
	echo					OptimizeReferences="2"
	echo					EnableCOMDATFolding="2"
	echo					TargetMachine="17"
	echo				/^>
	echo				^<Tool
	echo					Name="VCALinkTool"
	echo				/^>
	echo				^<Tool
	echo					Name="VCManifestTool"
	echo				/^>
	echo				^<Tool
	echo					Name="VCXDCMakeTool"
	echo				/^>
	echo				^<Tool
	echo					Name="VCBscMakeTool"
	echo				/^>
	echo				^<Tool
	echo					Name="VCFxCopTool"
	echo				/^>
	echo				^<Tool
	echo					Name="VCAppVerifierTool"
	echo				/^>
	echo				^<Tool
	echo					Name="VCWebDeploymentTool"
	echo				/^>
	echo				^<Tool
	echo					Name="VCPostBuildEventTool"
	echo				/^>
	echo			^</Configuration^>
	echo		^</Configurations^>
	echo		^<References^>
	echo		^</References^>
	echo		^<Files^>
	echo			^<Filter
	echo				Name="Source Files"
	echo				Filter="cpp;c;cc;cxx;def;odl;idl;hpj;bat;asm;asmx"
	echo				UniqueIdentifier="{4FC737F1-C7A5-4376-A066-2A32D752A2FF}"
	echo				^>

	for %%i in ("*.c","*.cpp","*.h","*.hpp") do (

		echo				^<File
		echo 				RelativePath=".\%%i"
		echo					^>
		echo				^</File^>

	)

	echo			^</Filter^>
	echo		^</Files^>
	echo		^<Globals^>
	echo		^</Globals^>
	echo ^</VisualStudioProject^>

	@echo off	
	exit /b %ERRORLEVEL%
)

rem -----------------------------------------

if "%STDOUT_REDIRECTED%" == "%SLN_OUTPUT%" (
	echo п^>ї
	echo Microsoft Visual Studio Solution File, Format Version 9.00
	echo # Visual Studio 2005
	echo Project^("{8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942}"^) = "project", "%VCPROJ_OUTPUT%", "{1BAD8409-F433-4B12-9563-531AD500FE7C}"
	echo EndProject
	echo Global
	echo		GlobalSection^(SolutionConfigurationPlatforms^) = preSolution
	echo			Debug^|Win32 = Debug^|Win32
	echo			Debug^|x64 = Debug^|x64
	echo			Release^|Win32 = Release^|Win32
	echo			Release^|x64 = Release^|x64
	echo		EndGlobalSection
	echo		GlobalSection^(ProjectConfigurationPlatforms^) = postSolution
	echo			{1BAD8409-F433-4B12-9563-531AD500FE7C}.Debug^|Win32.ActiveCfg = Debug^|Win32
	echo			{1BAD8409-F433-4B12-9563-531AD500FE7C}.Debug^|Win32.Build.0 = Debug^|Win32
	echo			{1BAD8409-F433-4B12-9563-531AD500FE7C}.Debug^|x64.ActiveCfg = Debug^|x64
	echo			{1BAD8409-F433-4B12-9563-531AD500FE7C}.Debug^|x64.Build.0 = Debug^|x64
	echo			{1BAD8409-F433-4B12-9563-531AD500FE7C}.Release^|Win32.ActiveCfg = Release^|Win32
	echo			{1BAD8409-F433-4B12-9563-531AD500FE7C}.Release^|Win32.Build.0 = Release^|Win32
	echo			{1BAD8409-F433-4B12-9563-531AD500FE7C}.Release^|x64.ActiveCfg = Release^|x64
	echo			{1BAD8409-F433-4B12-9563-531AD500FE7C}.Release^|x64.Build.0 = Release^|x64
	echo		EndGlobalSection
	echo		GlobalSection^(SolutionProperties^) = preSolution
	echo			HideSolutionNode = FALSE
	echo		EndGlobalSection
	echo EndGlobal

	@echo off
	exit /b %ERRORLEVEL%
)

rem -----------------------------------------

if "%STDOUT_REDIRECTED%" == "" (
	rem
	rem перенаправление вывода в makefile
	rem
	set STDOUT_REDIRECTED=%MAKEFILE_OUTPUT%
	cmd.exe /c %0 %* >%MAKEFILE_OUTPUT%

	rem
	rem перенаправление вывода в vcproj-файл
	rem
	set STDOUT_REDIRECTED=%VCPROJ_OUTPUT%
	cmd.exe /c %0 %* >%VCPROJ_OUTPUT%

	rem
	rem перенаправление вывода в sln-файл
	rem
	set STDOUT_REDIRECTED=%SLN_OUTPUT%
	cmd.exe /c %0 %* >%SLN_OUTPUT%

rem	call %VCDIR%\bin\vcvars32.bat
rem	nmake -f %MAKEFILE_OUTPUT%	
rem	del %MAKEFILE_OUTPUT%

call "%VS100COMNTOOLS%vsvars32.bat" || goto fail

del /f /q %APP_NAME%.vcxproj
del /f /q %APP_NAME%.vcxproj.filters
del /f /q debug\%APP_NAME%.exe
del /f /q debug64\%APP_NAME%.exe

call devenv /upgrade %APP_NAME%.sln || goto fail

del /f /q %APP_NAME%.vcproj
del /f /q UpgradeLog.XML
rd /s /q _UpgradeReport_Files
rd /s /q Backup

call devenv /nologo %APP_NAME%.sln /build "Debug|Win32" > build.x86.log || goto fail
call devenv /nologo %APP_NAME%.sln /build "Debug|x64" > build.x64.log || goto fail

rem @echo on
rem cd ..
rem mkdir "%DIR_NAME%" || goto fail
rem copy src\debug\%APP_NAME%.exe "%DIR_NAME%\%APP_NAME%.x86.exe" || goto fail
rem copy src\debug64\%APP_NAME%.exe "%DIR_NAME%\%APP_NAME%.x64.exe" || goto fail
rem copy src\data2.bin "%DIR_NAME%\data2.bin" || goto fail

echo OK

	exit /b %ERRORLEVEL%
	
)

rem -----------------------------------------

goto end
:fail
echo FAIL
:end
exit /b %ERRORLEVEL%

endlocal