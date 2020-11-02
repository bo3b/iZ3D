@set PATH=..\..\bin\others\;%PATH%
@md Shaders_dx9
@del Shaders_dx9\*.h
@set PARAMS=/nologo /Tps_2_0
@set PARAMS2=/nologo /Tps_1_4 /LD
@fxc.exe shaders_dx9.psh %PARAMS2% /EshaderMono /FhShaders_dx9\shaderMono.h
@fxc.exe shaders_dx9.psh %PARAMS2% /EshaderAnaglyph /FhShaders_dx9\shaderAnaglyph.h
@fxc.exe shaders_dx9.psh %PARAMS2% /EshaderColorAnaglyph /FhShaders_dx9\shaderColorAnaglyph.h
@fxc.exe shaders_dx9.psh %PARAMS2% /EshaderYellowBlueAnaglyph /FhShaders_dx9\shaderYellowBlueAnaglyph.h
@fxc.exe shaders_dx9.psh %PARAMS% /EshaderMonoAndGamma /FhShaders_dx9\shaderMonoAndGamma.h
@fxc.exe shaders_dx9.psh %PARAMS% /EshaderOptAnaglyph /FhShaders_dx9\shaderOptAnaglyph.h
@fxc.exe shaders_dx9.psh %PARAMS% /EshaderOptAnaglyphAndGamma /FhShaders_dx9\shaderOptAnaglyphAndGamma.h
@pause