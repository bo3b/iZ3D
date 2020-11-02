@set PATH=..\bin\others\;%PATH%
@md Shaders
REM /DDYNAMIC_CONSTANTS
@set PARAMS=/nologo /Tps_2_0
@set PARAMS2=/nologo /Tps_1_4 /LD
@del Shaders\*.h
@fxc.exe res\shaders.psh %PARAMS% /EshaderSimple /FhShaders\shaderSimple.h
@fxc.exe res\shaders.psh %PARAMS% /EshaderSimpleRAWZ /FhShaders\shaderSimpleRAWZ.h
@fxc.exe res\shaders.psh %PARAMS% /EshaderSimpleDepth /FhShaders\shaderSimpleDepth.h
@fxc.exe res\shaders.psh %PARAMS% /EshaderSimpleDepthRAWZ /FhShaders\shaderSimpleDepthRAWZ.h
@fxc.exe res\shaders.psh %PARAMS% /EshaderViewDepth /FhShaders\shaderViewDepth.h

@fxc.exe res\shaders.psh /nologo /Tvs_2_0 /EVSFont /FhShaders\VSFont.h
@fxc.exe res\shaders.psh %PARAMS% /EPSFont /FhShaders\PSFont.h
@pause