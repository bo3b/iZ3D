@set PATH=..\..\bin\others\;%PATH%
@md Shaders_dx9
@del Shaders_dx9\*.h
@set PARAMS=/nologo /Tps_2_0
@set PARAMS2=/nologo /Tps_1_4 /LD
@fxc.exe shaders_dx9.psh %PARAMS% /EshaderDLP3D /FhShaders_dx9\shaderDLP3D.h
@pause