@set PATH=..\..\bin\others\;%PATH%
@md Shaders_dx9
@del Shaders_dx9\*.h
REM /DDYNAMIC_CONSTANTS
@set PARAMS=/nologo /Tps_2_0
@set PARAMS2=/nologo /Tps_1_4 /LD
@fxc.exe shaders_dx9.psh %PARAMS% /EshaderStereoMirror /FhShaders_dx9\shaderStereoMirror.h
@pause