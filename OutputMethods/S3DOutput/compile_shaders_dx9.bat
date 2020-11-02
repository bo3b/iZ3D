@echo off
@set PATH=..\..\bin\others\;%PATH%
md Shaders_dx9
del Shaders_dx9\*.h
REM /DDYNAMIC_CONSTANTS
set PARAMS=/nologo /Tps_2_0
set PARAMS3=/nologo /Tps_3_0 /Dtex=tex2Dlod
set PARAMS2=/nologo /Tps_1_4 /LD
fxc.exe shaders_dx9.psh %PARAMS% /EshaderS3D /FhShaders_dx9\shaderS3D.h
fxc.exe shaders_dx9.psh %PARAMS% /EshaderS3DCFL /FhShaders_dx9\shaderS3DCFL.h
fxc.exe shaders_dx9.psh %PARAMS% /EshaderS3DAndGamma /FhShaders_dx9\shaderS3DAndGamma.h
fxc.exe shaders_dx9.psh %PARAMS% /EshaderS3DCFLAndGamma /FhShaders_dx9\shaderS3DCFLAndGamma.h
fxc.exe shaders_dx9.psh %PARAMS% /EshaderS3DBWCFL /FhShaders_dx9\shaderS3DBWCFL.h
fxc.exe shaders_dx9.psh %PARAMS% /EshaderS3DBWCFLAndGamma /FhShaders_dx9\shaderS3DBWCFLAndGamma.h
fxc.exe shaders_dx9.psh %PARAMS% /EshaderS3DFBCFL /FhShaders_dx9\shaderS3DFBCFL.h
fxc.exe shaders_dx9.psh %PARAMS% /EshaderS3DFBCFLAndGamma /FhShaders_dx9\shaderS3DFBCFLAndGamma.h


fxc.exe shaders_dx9.psh %PARAMS2% /EshaderS3D1st /FhShaders_dx9\shaderS3D1st.h
fxc.exe shaders_dx9.psh %PARAMS% /EshaderS3D2nd /FhShaders_dx9\shaderS3D2nd.h
fxc.exe shaders_dx9.psh %PARAMS% /EshaderS3DCFL2nd /FhShaders_dx9\shaderS3DCFL2nd.h
fxc.exe shaders_dx9.psh %PARAMS% /EshaderS3DAndGamma1st /FhShaders_dx9\shaderS3DAndGamma1st.h
fxc.exe shaders_dx9.psh %PARAMS% /EshaderS3DAndGamma2nd /FhShaders_dx9\shaderS3DAndGamma2nd.h
fxc.exe shaders_dx9.psh %PARAMS% /EshaderS3DCFLAndGamma2nd /FhShaders_dx9\shaderS3DCFLAndGamma2nd.h

fxc.exe shaders_dx9.psh %PARAMS3% /EshaderS3DTable /FhShaders_dx9\shaderS3DTable.h
fxc.exe shaders_dx9.psh %PARAMS3% /EshaderS3DTableCFL /FhShaders_dx9\shaderS3DTableCFL.h
fxc.exe shaders_dx9.psh %PARAMS3% /EshaderS3DTableAndGamma /FhShaders_dx9\shaderS3DTableAndGamma.h
fxc.exe shaders_dx9.psh %PARAMS3% /EshaderS3DTableCFLAndGamma /FhShaders_dx9\shaderS3DTableCFLAndGamma.h

fxc.exe shaders_dx9.psh %PARAMS3% /EshaderS3DTableCFL2 /FhShaders_dx9\shaderS3DTableCFL2.h
fxc.exe shaders_dx9.psh %PARAMS3% /EshaderS3DTableCFLAndGamma2 /FhShaders_dx9\shaderS3DTableCFLAndGamma2.h

fxc.exe shaders_dx9.psh %PARAMS3% /EshaderS3DTableBWCFL /FhShaders_dx9\shaderS3DTableBWCFL.h
fxc.exe shaders_dx9.psh %PARAMS3% /EshaderS3DTableBWCFLAndGamma /FhShaders_dx9\shaderS3DTableBWCFLAndGamma.h

fxc.exe shaders_dx9.psh %PARAMS% /EshaderS3DInterlaced /FhShaders_dx9\shaderS3DInterlaced.h
fxc.exe shaders_dx9.psh %PARAMS% /EshaderS3DCheckerboard /FhShaders_dx9\shaderS3DCheckerboard.h
pause