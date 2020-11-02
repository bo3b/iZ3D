@set PATH=..\..\bin\others\;%PATH%
@md Shaders_dx10
@del Shaders_dx10\*.h
@set PARAMS=/nologo /Tps_4_0 /Qstrip_reflect
@fxc.exe shaders_dx10.psh %PARAMS% /EshaderAnaglyph /FhShaders_dx10\shaderAnaglyph.h
@fxc.exe shaders_dx10.psh %PARAMS% /EshaderColorAnaglyph /FhShaders_dx10\shaderColorAnaglyph.h
@fxc.exe shaders_dx10.psh %PARAMS% /EshaderYellowBlueAnaglyph /FhShaders_dx10\shaderYellowBlueAnaglyph.h
@fxc.exe shaders_dx10.psh %PARAMS% /EshaderOptAnaglyph /FhShaders_dx10\shaderOptAnaglyph.h
@fxc.exe shaders_dx10.psh %PARAMS% /EshaderOptAnaglyphAndGamma /FhShaders_dx10\shaderOptAnaglyphAndGamma.h

@set PARAMS2=/nologo /Tvs_4_0 /Qstrip_reflect
@fxc.exe shaders_dx10.vsh %PARAMS2% /EshaderFullScreenQuadVS /FhShaders_dx10\shaderFullScreenQuadVS.h
@pause