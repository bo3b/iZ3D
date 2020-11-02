
@set PATH=..\..\bin\others\;%PATH%
@md Shaders_dx10
@del Shaders_dx10\*.h
@set PARAMS=/nologo /Tps_4_0 /Qstrip_reflect
@fxc.exe Shaders_dx10.psh %PARAMS% /EshaderS3D /FhShaders_dx10\shaderS3D.h
@fxc.exe Shaders_dx10.psh %PARAMS% /EshaderS3DCFL /FhShaders_dx10\shaderS3DCFL.h
@fxc.exe Shaders_dx10.psh %PARAMS% /EshaderS3DAndGamma /FhShaders_dx10\shaderS3DAndGamma.h
@fxc.exe Shaders_dx10.psh %PARAMS% /EshaderS3DCFLAndGamma /FhShaders_dx10\shaderS3DCFLAndGamma.h
@fxc.exe Shaders_dx10.psh %PARAMS% /EshaderS3DBWCFL /FhShaders_dx10\shaderS3DBWCFL.h
@fxc.exe Shaders_dx10.psh %PARAMS% /EshaderS3DBWCFLAndGamma /FhShaders_dx10\shaderS3DBWCFLAndGamma.h
@fxc.exe Shaders_dx10.psh %PARAMS% /EshaderS3DFBCFL /FhShaders_dx10\shaderS3DFBCFL.h
@fxc.exe Shaders_dx10.psh %PARAMS% /EshaderS3DFBCFLAndGamma /FhShaders_dx10\shaderS3DFBCFLAndGamma.h

@fxc.exe Shaders_dx10.psh %PARAMS% /EshaderS3DTable /FhShaders_dx10\shaderS3DTable.h
@fxc.exe Shaders_dx10.psh %PARAMS% /EshaderS3DTableCFL /FhShaders_dx10\shaderS3DTableCFL.h
@fxc.exe Shaders_dx10.psh %PARAMS% /EshaderS3DTableAndGamma /FhShaders_dx10\shaderS3DTableAndGamma.h
@fxc.exe Shaders_dx10.psh %PARAMS% /EshaderS3DTableCFLAndGamma /FhShaders_dx10\shaderS3DTableCFLAndGamma.h
@fxc.exe Shaders_dx10.psh %PARAMS% /EshaderS3DTableBWCFL /FhShaders_dx10\shaderS3DTableBWCFL.h
@fxc.exe Shaders_dx10.psh %PARAMS% /EshaderS3DTableBWCFLAndGamma /FhShaders_dx10\shaderS3DTableBWCFLAndGamma.h

@set PARAMS2=/nologo /Tvs_4_0 /Qstrip_reflect
@fxc.exe Shaders_dx10.vsh %PARAMS2% /EshaderFullScreenQuadVS /FhShaders_dx10\shaderFullScreenQuadVS.h
@pause