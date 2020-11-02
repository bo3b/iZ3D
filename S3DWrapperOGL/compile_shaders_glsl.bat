@md Shaders_glsl
@del Shaders_glsl\*.glsl
@set PARAMS=-profile glslf
@cgc.exe Shaders.cg %PARAMS% -entry shaderS3D -o Shaders_glsl\shaderS3D.glsl
@cgc.exe Shaders.cg %PARAMS% -entry shaderS3DCFL -o Shaders_glsl\shaderS3DCFL.glsl
@cgc.exe Shaders.cg %PARAMS% -entry shaderS3DAndGamma -o Shaders_glsl\shaderS3DAndGamma.glsl
@cgc.exe Shaders.cg %PARAMS% -entry shaderS3DCFLAndGamma -o Shaders_glsl\shaderS3DCFLAndGamma.glsl

@cgc.exe Shaders.cg %PARAMS% -entry shaderS3D1st -o Shaders_glsl\shaderS3D1st.glsl
@cgc.exe Shaders.cg %PARAMS% -entry shaderS3D2nd -o Shaders_glsl\shaderS3D2nd.glsl
@cgc.exe Shaders.cg %PARAMS% -entry shaderS3DCFL2nd -o Shaders_glsl\shaderS3DCFL2nd.glsl
@cgc.exe Shaders.cg %PARAMS% -entry shaderS3DAndGamma1st -o Shaders_glsl\shaderS3DAndGamma1st.glsl
@cgc.exe Shaders.cg %PARAMS% -entry shaderS3DAndGamma2nd -o Shaders_glsl\shaderS3DAndGamma2nd.glsl
@cgc.exe Shaders.cg %PARAMS% -entry shaderS3DCFLAndGamma2nd -o Shaders_glsl\shaderS3DCFLAndGamma2nd.glsl

@cgc.exe Shaders.cg %PARAMS% -entry shaderS3DTable -o Shaders_glsl\shaderS3DTable.glsl
@cgc.exe Shaders.cg %PARAMS% -entry shaderS3DTableCFL -o Shaders_glsl\shaderS3DTableCFL.glsl
@cgc.exe Shaders.cg %PARAMS% -entry shaderS3DTableAndGamma -o Shaders_glsl\shaderS3DTableAndGamma.glsl
@cgc.exe Shaders.cg %PARAMS% -entry shaderS3DTableCFLAndGamma -o Shaders_glsl\shaderS3DTableCFLAndGamma.glsl
@pause