@md Shaders_arb
@del Shaders_arb\*.arbf
@set PARAMS=-profile arbfp1
@cgc.exe Shaders.cg %PARAMS% -entry shaderS3D -o Shaders_arb\shaderS3D.arbf
@cgc.exe Shaders.cg %PARAMS% -entry shaderS3DCFL -o Shaders_arb\shaderS3DCFL.arbf
@cgc.exe Shaders.cg %PARAMS% -entry shaderS3DAndGamma -o Shaders_arb\shaderS3DAndGamma.arbf
@cgc.exe Shaders.cg %PARAMS% -entry shaderS3DCFLAndGamma -o Shaders_arb\shaderS3DCFLAndGamma.arbf

@cgc.exe Shaders.cg %PARAMS% -entry shaderS3D1st -o Shaders_arb\shaderS3D1st.arbf
@cgc.exe Shaders.cg %PARAMS% -entry shaderS3D2nd -o Shaders_arb\shaderS3D2nd.arbf
@cgc.exe Shaders.cg %PARAMS% -entry shaderS3DCFL2nd -o Shaders_arb\shaderS3DCFL2nd.arbf
@cgc.exe Shaders.cg %PARAMS% -entry shaderS3DAndGamma1st -o Shaders_arb\shaderS3DAndGamma1st.arbf
@cgc.exe Shaders.cg %PARAMS% -entry shaderS3DAndGamma2nd -o Shaders_arb\shaderS3DAndGamma2nd.arbf
@cgc.exe Shaders.cg %PARAMS% -entry shaderS3DCFLAndGamma2nd -o Shaders_arb\shaderS3DCFLAndGamma2nd.arbf

@cgc.exe Shaders.cg %PARAMS% -entry shaderS3DTable -o Shaders_arb\shaderS3DTable.arbf
@cgc.exe Shaders.cg %PARAMS% -entry shaderS3DTableCFL -o Shaders_arb\shaderS3DTableCFL.arbf
@cgc.exe Shaders.cg %PARAMS% -entry shaderS3DTableAndGamma -o Shaders_arb\shaderS3DTableAndGamma.arbf
@cgc.exe Shaders.cg %PARAMS% -entry shaderS3DTableCFLAndGamma -o Shaders_arb\shaderS3DTableCFLAndGamma.arbf
@pause