@set PATH=..\..\bin\others\;%PATH%
@md Shaders_dx9
@del Shaders_dx9\*.h
@set PARAMS=/nologo /Tps_2_0
@set PARAMS2=/nologo /Tps_1_4 /LD
@fxc.exe shaders_dx9.psh %PARAMS% /EshaderMono /FhShaders_dx9\shaderMono.h
@fxc.exe shaders_dx9.psh %PARAMS% /EshaderHorizontalInterlaced /FhShaders_dx9\shaderHorizontalInterlaced.h
@fxc.exe shaders_dx9.psh %PARAMS% /EshaderHorizontalInterlacedOptimized /FhShaders_dx9\shaderHorizontalInterlacedOptimized.h
@pause