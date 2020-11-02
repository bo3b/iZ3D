@set PATH=..\..\bin\others\;..\bin\others\;%PATH%

@md Shaders
@del Shaders\*.h
@set PARAMS=/nologo /Tps_4_0 /Qstrip_reflect
@fxc.exe shaders_dx10.psh %PARAMS% /EPSFont /FhShaders\PSFont_dx10.h
@fxc.exe shaders_dx10.psh %PARAMS% /EPSBlack /FhShaders\PSBlack_dx10.h

@set PARAMS2=/nologo /Tvs_4_0 /Qstrip_reflect
@fxc.exe shaders_dx10.vsh %PARAMS2% /EVSFont /FhShaders\VSFont_dx10.h
@pause