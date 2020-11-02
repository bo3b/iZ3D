/* 
* Project : iZ3D Stereo Driver
* Copyright (C) iZ3D Inc. 2002 - 2010
*/

#pragma once

static char g_szRAWLeftShaderText[] = "					\n\
uniform sampler2D sL;									\n\
uniform sampler2D sR;									\n\
														\n\
void main() {											\n\
	vec4 cL = texture2D(sL, gl_TexCoord[0].xy);			\n\
	vec3 gamma = vec3(2.2);								\n\
	cL.rgb = pow(cL.rgb, gamma);						\n\
														\n\
	vec4 cRes = cL;										\n\
	cRes.rgb = pow(cRes.rgb, 1/gamma);					\n\
	gl_FragData[0] = cRes;								\n\
}";

static char g_szRAWRightShaderText[] = "				\n\
uniform sampler2D sL;									\n\
uniform sampler2D sR;									\n\
														\n\
void main() {											\n\
	vec4 cR = texture2D(sR, gl_TexCoord[0].xy);			\n\
	vec3 gamma = vec3(2.2);								\n\
	cR.rgb = pow(cR.rgb, gamma);						\n\
														\n\
	vec4 cRes = cR;										\n\
	cRes.rgb = pow(cRes.rgb, 1/gamma);					\n\
	gl_FragData[0] = cRes;								\n\
}";

static char g_szBackScreenShaderText[] = "				\n\
uniform sampler2D sL;									\n\
uniform sampler2D sR;									\n\
														\n\
vec4 primaryColor( in vec4 cL, in vec4 cR ) {			\n\
	return (cL + cR) * 0.5f;							\n\
}														\n\
														\n\
void main() {											\n\
	vec4 cL = texture2D(sL, gl_TexCoord[0].xy);			\n\
	vec4 cR = texture2D(sR, gl_TexCoord[0].xy);			\n\
	vec3 gamma = vec3(2.2);								\n\
	cL.rgb = pow(cL.rgb, gamma);						\n\
	cR.rgb = pow(cR.rgb, gamma);						\n\
														\n\
	vec4 cRes = primaryColor(cL, cR);					\n\
	cRes.rgb = pow(cRes.rgb, 1/gamma);					\n\
	gl_FragData[0] = cRes;								\n\
}";

static char g_szFrontScreenShaderText[] = "				\n\
uniform sampler2D sL;									\n\
uniform sampler2D sR;									\n\
														\n\
vec4 secondaryColor( in vec4 cL, in vec4 cR ) {			\n\
	vec4 Sum = (cL + cR);								\n\
	vec3 rVal = vec3( (cR / Sum));						\n\
	vec4 res;											\n\
	res.r = (Sum.r >= 0.0000001f ? rVal.r : 0.5f);		\n\
	res.g = (Sum.g >= 0.0000001f ? rVal.g : 0.5f);		\n\
	res.b = (Sum.b >= 0.0000001f ? rVal.b : 0.5f);		\n\
	res.a = cR.a;										\n\
	return res;											\n\
}														\n\
														\n\
void main() {											\n\
	vec4 cL = texture2D(sL, gl_TexCoord[0].xy);			\n\
	vec4 cR = texture2D(sR, gl_TexCoord[0].xy);			\n\
	vec3 gamma = vec3(2.2);								\n\
	cL.rgb = pow(cL.rgb, gamma);						\n\
	cR.rgb = pow(cR.rgb, gamma);						\n\
														\n\
	vec4 cRes = secondaryColor( cL, cR);				\n\
	cRes.rgb = pow(cRes.rgb, 1/gamma);					\n\
	gl_FragData[0] = cRes;								\n\
}";

static char g_szFrontScreenCFLShaderText[] = "			\n\
uniform sampler2D sL;									\n\
uniform sampler2D sR;									\n\
														\n\
vec4 secondaryCFLColor( in vec4 cL, in vec4 cR ) {		\n\
	float rcSum = dot( (cL + cR).xyz , vec3( 1.0f ));	\n\
	float rcR = dot( cR.xyz , vec3( 1.0f ));			\n\
	float rVal = (rcR / rcSum);							\n\
	return vec4( rcSum >= 0.003f ? rVal : 0.5f );		\n\
}														\n\
														\n\
void main() {											\n\
	vec4 cL = texture2D(sL, gl_TexCoord[0].xy);			\n\
	vec4 cR = texture2D(sR, gl_TexCoord[0].xy);			\n\
	vec3 gamma = vec3(2.2);								\n\
	cL.rgb = pow(cL.rgb, gamma);						\n\
	cR.rgb = pow(cR.rgb, gamma);						\n\
														\n\
	vec4 cRes = secondaryCFLColor( cL, cR);				\n\
	cRes.rgb = pow(cRes.rgb, 1/gamma);					\n\
	gl_FragData[0] = cRes;								\n\
}";

static char g_szVertexShaderText[] = "					\n\
void main() {											\n\
	gl_Position = gl_Vertex;							\n\
	gl_TexCoord[0] = gl_MultiTexCoord0;					\n\
}";
