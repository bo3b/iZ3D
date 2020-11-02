/* 
* Project : iZ3D Stereo Driver
* Copyright (C) iZ3D Inc. 2002 - 2010
*/

#pragma once

#if defined(_X86_) || defined(_IA64_)
#pragma pack(4)
#endif

// Caps2
#define D3DCAPS2_CANRENDERWINDOWED      0x00080000L
#define D3DCAPS2_NO2DDURING3DSCENE      0x00000002L

// PrimitiveMiscCaps
#define D3DPMISCCAPS_LINEPATTERNREP     0x00000004L

// RasterCaps
#define D3DPRASTERCAPS_PAT              0x00000008L
#define D3DPRASTERCAPS_ZBIAS            0x00004000L

// VertexProcessingCaps
#define D3DVTXPCAPS_NO_VSDT_UBYTE4      0x00000080L /* device does not support D3DVSDT_UBYTE4 */

typedef struct _D3DCAPS8
{
    // Device Info /
    D3DDEVTYPE  DeviceType;
    UINT    AdapterOrdinal;
	
    // Caps from DX7 Draw /
    DWORD   Caps;
    DWORD   Caps2;
    DWORD   Caps3;
    DWORD   PresentationIntervals;
	
    // Cursor Caps /
    DWORD   CursorCaps;
	
    // 3D Device Caps /
    DWORD   DevCaps;
	
    DWORD   PrimitiveMiscCaps;
    DWORD   RasterCaps;
    DWORD   ZCmpCaps;
    DWORD   SrcBlendCaps;
    DWORD   DestBlendCaps;
    DWORD   AlphaCmpCaps;
    DWORD   ShadeCaps;
    DWORD   TextureCaps;
    DWORD   TextureFilterCaps;          // D3DPTFILTERCAPS for IDirect3DTexture8's
    DWORD   CubeTextureFilterCaps;      // D3DPTFILTERCAPS for IDirect3DCubeTexture8's
    DWORD   VolumeTextureFilterCaps;    // D3DPTFILTERCAPS for IDirect3DVolumeTexture8's
    DWORD   TextureAddressCaps;         // D3DPTADDRESSCAPS for IDirect3DTexture8's
    DWORD   VolumeTextureAddressCaps;   // D3DPTADDRESSCAPS for IDirect3DVolumeTexture8's
	
    DWORD   LineCaps;                   // D3DLINECAPS
	
    DWORD   MaxTextureWidth, MaxTextureHeight;
    DWORD   MaxVolumeExtent;
	
    DWORD   MaxTextureRepeat;
    DWORD   MaxTextureAspectRatio;
    DWORD   MaxAnisotropy;
    float   MaxVertexW;
	
    float   GuardBandLeft;
    float   GuardBandTop;
    float   GuardBandRight;
    float   GuardBandBottom;
	
    float   ExtentsAdjust;
    DWORD   StencilCaps;
	
    DWORD   FVFCaps;
    DWORD   TextureOpCaps;
    DWORD   MaxTextureBlendStages;
    DWORD   MaxSimultaneousTextures;
	
    DWORD   VertexProcessingCaps;
    DWORD   MaxActiveLights;
    DWORD   MaxUserClipPlanes;
    DWORD   MaxVertexBlendMatrices;
    DWORD   MaxVertexBlendMatrixIndex;
	
    float   MaxPointSize;
	
    DWORD   MaxPrimitiveCount;          // max number of primitives per DrawPrimitive call
    DWORD   MaxVertexIndex;
    DWORD   MaxStreams;
    DWORD   MaxStreamStride;            // max stride for SetStreamSource
	
    DWORD   VertexShaderVersion;
    DWORD   MaxVertexShaderConst;       // number of vertex shader constant registers
	
    DWORD   PixelShaderVersion;
    float   MaxPixelShaderValue;        // max value of pixel shader arithmetic component
	
} D3DCAPS8;

typedef struct _D3DVIEWPORT8 {
    DWORD       X;
    DWORD       Y;            // Viewport Top left /
    DWORD       Width;
    DWORD       Height;       // Viewport Dimensions 
    float       MinZ;         // Min/max of clip Volume /
    float       MaxZ;
} D3DVIEWPORT8;

typedef struct _D3DCLIPSTATUS8 {
    DWORD ClipUnion;
    DWORD ClipIntersection;
} D3DCLIPSTATUS8;

typedef struct _D3DMATERIAL8 {
    D3DCOLORVALUE   Diffuse;        // Diffuse color RGBA //
    D3DCOLORVALUE   Ambient;        // Ambient color RGB //
    D3DCOLORVALUE   Specular;       // Specular 'shininess' //
    D3DCOLORVALUE   Emissive;       // Emissive color RGB //
    float           Power;          // Sharpness if specular highlight //
} D3DMATERIAL8;


typedef struct _D3DLIGHT8 {
    D3DLIGHTTYPE    Type;            // Type of light source //
    D3DCOLORVALUE   Diffuse;         // Diffuse color of light //
    D3DCOLORVALUE   Specular;        // Specular color of light //
    D3DCOLORVALUE   Ambient;         // Ambient color of light //
    D3DVECTOR       Position;         // Position in world space //
    D3DVECTOR       Direction;        // Direction in world space //
    float           Range;            // Cutoff range //
    float           Falloff;          // Falloff //
    float           Attenuation0;     // Constant attenuation //
    float           Attenuation1;     // Linear attenuation //
    float           Attenuation2;     // Quadratic attenuation //
    float           Theta;            // Inner angle of spotlight cone //
    float           Phi;              // Outer angle of spotlight cone //
} D3DLIGHT8;

#define MAX_DEVICE_IDENTIFIER_STRING        512 
typedef struct _D3DADAPTER_IDENTIFIER8
{
    char            Driver[MAX_DEVICE_IDENTIFIER_STRING];
    char            Description[MAX_DEVICE_IDENTIFIER_STRING];
	
#ifdef _WIN32
    LARGE_INTEGER   DriverVersion;            /* Defined for 32 bit components */
#else
    DWORD           DriverVersionLowPart;     /* Defined for 16 bit driver components */
    DWORD           DriverVersionHighPart;
#endif
	
    DWORD           VendorId;
    DWORD           DeviceId;
    DWORD           SubSysId;
    DWORD           Revision;
	
    GUID            DeviceIdentifier;
	
    DWORD           WHQLLevel;
	
} D3DADAPTER_IDENTIFIER8;

typedef struct _D3DPRESENT_PARAMETERS8_ {
	UINT                    BackBufferWidth;
	UINT                    BackBufferHeight;
	D3DFORMAT               BackBufferFormat;
	UINT                    BackBufferCount;

	D3DMULTISAMPLE_TYPE     MultiSampleType;

	D3DSWAPEFFECT           SwapEffect;
	HWND                    hDeviceWindow;
	BOOL                    Windowed;
	BOOL                    EnableAutoDepthStencil;
	D3DFORMAT               AutoDepthStencilFormat;
	DWORD                   Flags;

	UINT                    FullScreen_RefreshRateInHz;
	UINT                    FullScreen_PresentationInterval;

} D3DPRESENT_PARAMETERS8;

/* Surface Description */
typedef struct _D3DSURFACE_DESC8
{
	D3DFORMAT           Format;
	D3DRESOURCETYPE     Type;
	DWORD               Usage;
	D3DPOOL             Pool;
	UINT                Size;

	D3DMULTISAMPLE_TYPE MultiSampleType;
	UINT                Width;
	UINT                Height;
} D3DSURFACE_DESC8;

typedef struct _D3DVOLUME_DESC8
{
	D3DFORMAT           Format;
	D3DRESOURCETYPE     Type;
	DWORD               Usage;
	D3DPOOL             Pool;
	UINT                Size;

	UINT                Width;
	UINT                Height;
	UINT                Depth;
} D3DVOLUME_DESC8;

#pragma pack()
