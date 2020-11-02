@set PATH=..\..\bin\others\;%PATH%
@md Shaders_dx10
@del Shaders_dx10\*.h
@set PARAMS=/nologo /Tps_4_0 /Qstrip_reflect
@fxc.exe shaders_dx10.psh %PARAMS% /EshaderHorizontalInterlaced /FhShaders_dx10\shaderHorizontalInterlaced.h
@fxc.exe shaders_dx10.psh %PARAMS% /EshaderHorizontalInterlacedOptimized /FhShaders_dx10\shaderHorizontalInterlacedOptimized.h
@fxc.exe shaders_dx10.psh %PARAMS% /EshaderMono /FhShaders_dx10\shaderMono.h

@set PARAMS2=/nologo /Tvs_4_0 /Qstrip_reflect
@fxc.exe shaders_dx10.vsh %PARAMS2% /EshaderFullScreenQuadVS /FhShaders_dx10\shaderFullScreenQuadVS.h
@pause