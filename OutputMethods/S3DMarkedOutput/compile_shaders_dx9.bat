@echo off
@set PATH=..\..\bin\others\;%PATH%
md Shaders_dx9
del Shaders_dx9\*.h
REM /DDYNAMIC_CONSTANTS
set PARAMS=/nologo /Tps_2_0
set PARAMS2=/nologo /Tps_3_0 /Dtex=tex2Dlod

fxc.exe shaders_dx9.psh %PARAMS% /EshaderS3D /FhShaders_dx9\shaderS3D.h
fxc.exe shaders_dx9.psh %PARAMS% /EshaderS3DCFL /FhShaders_dx9\shaderS3DCFL.h
fxc.exe shaders_dx9.psh %PARAMS% /EshaderS3DAndGamma /FhShaders_dx9\shaderS3DAndGamma.h
fxc.exe shaders_dx9.psh %PARAMS% /EshaderS3DCFLAndGamma /FhShaders_dx9\shaderS3DCFLAndGamma.h

fxc.exe shaders_dx9.psh %PARAMS2% /EshaderS3DTable1st /FhShaders_dx9\shaderS3DTable1st.h
fxc.exe shaders_dx9.psh %PARAMS2% /EshaderS3DTable2nd /FhShaders_dx9\shaderS3DTable2nd.h
fxc.exe shaders_dx9.psh %PARAMS2% /EshaderS3DTableCFL2nd /FhShaders_dx9\shaderS3DTableCFL2nd.h
fxc.exe shaders_dx9.psh %PARAMS2% /EshaderS3DTableAndGamma1st /FhShaders_dx9\shaderS3DTableAndGamma1st.h
fxc.exe shaders_dx9.psh %PARAMS2% /EshaderS3DTableAndGamma2nd /FhShaders_dx9\shaderS3DTableAndGamma2nd.h
fxc.exe shaders_dx9.psh %PARAMS2% /EshaderS3DTableCFLAndGamma2nd /FhShaders_dx9\shaderS3DTableCFLAndGamma2nd.h

fxc.exe shaders_dx9.psh %PARAMS2% /EshaderS3DTable /FhShaders_dx9\shaderS3DTable.h
fxc.exe shaders_dx9.psh %PARAMS2% /EshaderS3DTableCFL /FhShaders_dx9\shaderS3DTableCFL.h
fxc.exe shaders_dx9.psh %PARAMS2% /EshaderS3DTableAndGamma /FhShaders_dx9\shaderS3DTableAndGamma.h
fxc.exe shaders_dx9.psh %PARAMS2% /EshaderS3DTableCFLAndGamma /FhShaders_dx9\shaderS3DTableCFLAndGamma.h

pause