/* 
* Project : iZ3D Stereo Driver
* Copyright (C) iZ3D Inc. 2002 - 2010
*/

#pragma once

#include <vector>
#include <d3dx9math.h>

enum PIPELINE {Shader, Fixed, RHW, RHWEmul};
enum TextureType {
	Mono,		// Only left RT contain correct image
	Stereo,		// Left and right RT's contain correct stereo images
	Cleared};	// Left and right RT's contain identical images

struct vbs
{
	IDirect3DVertexBuffer9* pVB;
	UINT Offset, Stride;
};

struct renderStates
{
	// Vertex State

	// Clip Planes
	BOOL			ClipPlaneEnable;

	// Fixed-Function Lighting States
	D3DCOLOR		Ambient;
	BOOL			ColorVertex;
	BOOL			Lighting;
	BOOL			LocalViewer;
	BOOL			NormalizeNormals;
	D3DMATERIALCOLORSOURCE	AmbientMaterialSource;
	D3DMATERIALCOLORSOURCE	DiffuseMaterialSource;
	D3DMATERIALCOLORSOURCE	SpecularMaterialSource;
	D3DMATERIALCOLORSOURCE	EmissiveMaterialSource;

	// Fixed-Function Transformation States
	BOOL				IndexedVertexBlendEnable;
	D3DVERTEXBLENDFLAGS	VertexBlend;
	FLOAT				TweenFactor;

	// Tesselation
	BOOL			EnableAdaptiveTesselation;
	FLOAT			AdaptiveTessX;
	FLOAT			AdaptiveTessY;
	FLOAT			AdaptiveTessZ;
	FLOAT			AdaptiveTessW;
	FLOAT			MinTessellationLevel;
	FLOAT			MaxTessellationLevel;
	D3DPATCHEDGESTYLE		PatchEdgeStyle;
	DWORD			PositionDegree;
	DWORD			NormalDegree;

	// Pixel State

	// Clipping, Culling, Scissor, Viewport
	BOOL			Clipping;
	D3DCULL			CullMode;
	BOOL			ScissorTestEnable;

	// Rasterization State
	BOOL			AntiAlisedLineEnable;
	DWORD			DepthBias;
	BOOL			DitherEnable;
	D3DFILLMODE		FillMode;
	BOOL			LastPixel;
	BOOL			MultiSampleAntiAlias;
	DWORD			MultiSampleMask;
	D3DSHADEMODE	ShadeMode;
	BOOL			SpecularEnable;

	// Alpha Test
	BOOL			AlphaTestEnable;
	DWORD			AlphaRef;
	D3DCMPFUNC		AlphaFunc;

	// Fog
	BOOL			FogEnable;
	BOOL			RangeFogEnable;
	D3DCOLOR		FogColor;
	D3DFOGMODE		FogTableMode;
	D3DFOGMODE		FogVertexMode;
	FLOAT			FogStart;
	FLOAT			FogEnd;
	FLOAT			FogDensity;

	// Point Sprites
	BOOL			PointSpriteEnable;
	BOOL			PointScaleEnable;
	FLOAT			PointScaleA;
	FLOAT			PointScaleB;
	FLOAT			PointScaleC;
	FLOAT			PointSize;
	FLOAT			PointSizeMin;
	FLOAT			PointSizeMax;

	// Texture Coordinate Wrapping
	DWORD			Wrap0;
	DWORD			Wrap1;
	DWORD			Wrap2;
	DWORD			Wrap3;
	DWORD			Wrap4;
	DWORD			Wrap5;
	DWORD			Wrap6;
	DWORD			Wrap7;
	DWORD			Wrap8;
	DWORD			Wrap9;
	DWORD			Wrap10;
	DWORD			Wrap11;
	DWORD			Wrap12;
	DWORD			Wrap13;
	DWORD			Wrap14;
	DWORD			Wrap15;

	// Texture Factor
	D3DCOLOR 		BlendFactor;

	// Output State

	// Depth Test
	D3DZBUFFERTYPE	ZEnable;
	BOOL			ZWriteEnable;
	D3DCMPFUNC		ZFunc;

	// Stencil Test
	BOOL			StencilEnable;
	D3DSTENCILOP	StencilFail;
	D3DSTENCILOP	StencilZFail;
	D3DSTENCILOP	StencilPass;
	D3DCMPFUNC		StencilFunc;
	BOOL			TwoSidedStensilMode;
	D3DSTENCILOP	CCWStencilFail;
	D3DSTENCILOP	CCWStencilZFail;
	D3DSTENCILOP	CCWStencilPass;
	D3DCMPFUNC		CCWStencilFunc;
	DWORD			StencilRef;			/* Reference value used in stencil test */
	DWORD			StencilMask;		/* Mask value used in stencil test */
	DWORD			StencilWriteMask;	/* Write mask applied to values written to stencil buffer */

	// Blending
	BOOL			AlphaBlendEnable;
	D3DBLENDOP		BlenOp;	
	D3DBLEND		SrcBlend;
	D3DBLEND		DestBlend;
	D3DCOLOR		TextureFactor;
	BOOL			SeparateAlphaBlendEnable;
	D3DBLEND 		SrcBlendAlpha;
	D3DBLEND 		DestBlendAlpha;
	D3DBLENDOP		BlendOpAlpha;

	// Gamma
	BOOL			SRGBWrite;

	// Render Targets
	BOOL			ColorWriteEnable1;
	BOOL			ColorWriteEnable2;
	BOOL			ColorWriteEnable3;

	DWORD			SlopeScaleDepthBias;

	D3DDEBUGMONITORTOKENS	DebugMonitorToken; 
};

struct samplerStates
{
	D3DTEXTUREADDRESS		AddressU;
	D3DTEXTUREADDRESS		AddressV;
	D3DTEXTUREADDRESS		AddressW;
	D3DCOLOR				BorderColor;
	D3DTEXTUREFILTERTYPE	MagFilter;
	D3DTEXTUREFILTERTYPE	MinFilter;
	D3DTEXTUREFILTERTYPE	MipFilter;
	DWORD					MipMapLodBias;
	DWORD					MaxMipLevel;
	DWORD					MaxAnisotropy;
	BOOL					SRGBTexture;
	DWORD					ElementIndex;
	DWORD					DMapOffset;
};

struct textureStageState
{
	D3DTEXTUREOP	ColorOp;
	DWORD			ColorArg0;
	DWORD			ColorArg1;
	DWORD			ColorArg2;
	D3DTEXTUREOP	AlphaOp;
	DWORD			AlphaArg0;
	DWORD			AlphaArg1;
	DWORD			AlphaArg2;
	DWORD			TexCoordIndex;
	DWORD			TextureTransformFlags;
	DWORD			ResultArg;
	DWORD			Constant;

	// Bump mapping
	FLOAT			BumpEnvMat00;
	FLOAT			BumpEnvMat01;
	FLOAT			BumpEnvMat10;
	FLOAT			BumpEnvMat11;
	FLOAT			BumpEnvLScale;
	FLOAT			BumpEnvLOffset;
};

struct AllStates
{
	renderStates rs;
	samplerStates ss[16];
	textureStageState tss[16];
	vbs vb[16];
	UINT divider[16];
	IDirect3DVertexShader9* pVS;
	IDirect3DIndexBuffer9* pIB;
	DWORD Fvf;
	IDirect3DVertexDeclaration9* pVD;
	FLOAT VSConsts[256][4];
	D3DVIEWPORT9 vp;
	RECT sr;
	void Clear()
	{
		ZeroMemory(this, sizeof AllStates);
	}
};
