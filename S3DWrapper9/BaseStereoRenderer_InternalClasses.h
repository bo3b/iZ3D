/* 
* Project : iZ3D Stereo Driver
* Copyright (C) iZ3D Inc. 2002 - 2010
*/

#pragma once

#include "DX9ShaderConverter.h"

enum VIEWINDEX
{
	VIEWINDEX_MONO  = 0,
	VIEWINDEX_LEFT  = 1,
	VIEWINDEX_RIGHT = 2
};


struct MONO_CAMERA
{
	MONO_CAMERA()
	{ 
		A = 0.0;
		B = 0.0;
		ZeroMemory(ProjectionMatrix,				sizeof(D3DXMATRIX));
		ZeroMemory(TransformBeforeViewport,			sizeof(D3DXMATRIX));
		ZeroMemory(TransformAfterViewport,			sizeof(D3DXMATRIX));
		ZeroMemory(TransformAfterViewportVS,		sizeof(D3DXMATRIX));
	}
	float A, B;
	D3DXMATRIX ProjectionMatrix;
	D3DXMATRIX TransformBeforeViewport;
	D3DXMATRIX TransformAfterViewport;
	D3DXMATRIX TransformAfterViewportVS;
};

struct STEREO_CAMERA
{
	MONO_CAMERA& GetCamera(VIEWINDEX index)
	{ return Camera[index - 1]; }
protected:
	MONO_CAMERA Camera[2];
};

// {9F665744-F53F-4a5b-9348-29DE11F4A751}
static const GUID YouTubeLeftTexture_GUID = 
{ 0x9f665744, 0xf53f, 0x4a5b, { 0x93, 0x48, 0x29, 0xde, 0x11, 0xf4, 0xa7, 0x51 } };
// {9F665744-F53F-4a5b-9348-29DE11F4A752}
static const GUID YouTubeRightTexture_GUID = 
{ 0x9f665744, 0xf53f, 0x4a5b, { 0x93, 0x48, 0x29, 0xde, 0x11, 0xf4, 0xa7, 0x52 } };

struct SurfaceData
{
public:
	SurfaceData();
	void Init(IDirect3DSurface9* pLeft);
	void Init(IDirect3DSurface9* pLeft, IDirect3DSurface9* pRight);
	void Clear();
	bool SetType(TextureType type);
	bool SetSurfaceType(TextureType type);
	TextureType GetType() { return type; }

	template<VIEWINDEX viewIndex>
	inline IDirect3DSurface9* GetViewSurf() 
	{ return (viewIndex == VIEWINDEX_RIGHT ? m_pRightSurf.p : m_pMainSurf); };

	IDirect3DSurface9*			m_pMainSurf;	// if WRT=1 set wide surface
	CComPtr<IDirect3DSurface9>	m_pRightSurf;
	IDirect3DTexture9*			m_pMainTex;		// support only IDirect3DTexture9 for now
private:
	TextureType type;
};

struct TexData
{
public:
	TexData();
	void Init(IDirect3DBaseTexture9* pTexture, IDirect3DBaseTexture9* pMainTexture);
	template<VIEWINDEX viewIndex>
	inline IDirect3DBaseTexture9* GetViewTex() 
	{ return (viewIndex == VIEWINDEX_RIGHT ? m_pRight.p : m_pLeft); };

	IDirect3DBaseTexture9*			m_pLeft;
	CComPtr<IDirect3DBaseTexture9>	m_pRight;

	// only for wide mode
	IDirect3DTexture9*				m_pWideTexture;
};

struct LockParams
{
	UINT OffsetToLock;
	UINT SizeToLock;
	VOID * pbData;
};

enum VBStates { vbVerticesChanged = 1, vbNotUnlocked = 2, vbModified = 4, vbDynamic = 8, vbDiscard = 16, vbNoOverwrite = 32 };

MIDL_INTERFACE("017EBD3D-9418-473f-B822-7845B27E0B52")
ILockVBData: public IUnknown
{
	CComPtr<IDirect3DVertexBuffer9>	m_pStereoVB;
	int m_VerticesChanges;
	int m_VBStates;
	LockParams m_LockParams;
	UINT m_VBSize;
	D3DMATRIX m_Matrix;
	const bool* m_pStereoActive;
public:
	inline void Init() {
		m_VBStates = vbVerticesChanged;
		m_VBSize = 0;
		ZeroMemory(&m_Matrix, sizeof D3DMATRIX);
	}
	inline int	GetStates() {
		return m_VBStates;
	}
	inline void	SetStates(int states) {
		m_VBStates = states;
	}
	inline void	GetLockParams(LockParams** ppLockParams) { 
		*ppLockParams = &m_LockParams;
	}
	inline void	SetLockParams(LockParams* pLockParams) {
		m_LockParams = *pLockParams;
	}
	inline UINT	GetVBSize() {
		return m_VBSize;
	}
	inline void	SetVBSize(UINT size) {
		m_VBSize = size;
	}
	inline void	GetMatrix(D3DMATRIX** ppMatrix)	{ // not used now
		*ppMatrix = &m_Matrix;
	}
	inline void	SetMatrix(D3DMATRIX* pMatrix) {
		m_Matrix = *pMatrix;
	}
	inline IDirect3DVertexBuffer9*	GetStereoVB() {
		return m_pStereoVB;
	}
	inline void	SetStereoVB(IDirect3DVertexBuffer9* pVB) {
		m_pStereoVB = pVB;
	}
	inline void	SetStereoActiveVariable(const bool* pStereoActive) 
	{
		m_pStereoActive = pStereoActive;
	}
	inline bool	GetStereoActive()
	{
		return *m_pStereoActive;
	}
};

template <class T, class Q>
class ShaderPipelineData
{
public:
	DWORD							m_MaxTextures;
	DWORD							m_StereoTexturesMask;
	std::vector<TexData>            m_Texture;
	std::vector<const ResourceCRCMultiplier*>		m_TextureCRCMultiplier;
	T								m_CurrentShaderData;
	const ShaderProfileData*		m_pProfile;

	std::vector<CAdapt<CComPtr<Q>>>	m_ShadersList;
	Q*								m_CurrentShader;
	Q*								m_CurrentStereoShader;
	BYTE*							m_pShaderFunctionOpCode;		// for optimization of function GetPSPrivateData()
	UINT							m_ShaderFunctionSize;
	BOOL                            m_bShaderDataAvailable;
	DWORD                           m_ShaderVersion;

	ShaderPipelineData();
	~ShaderPipelineData();
	void	SetMaxTextures( DWORD maxTextures );
	void	Init(DWORD textures);
	void	Clear();

	Q*		SetShader(Q* pShader, BOOL bStereoActive, ShaderProfileDataMap& map, DWORD defTextures);
	const ShaderProfileData* FindProfile( ShaderProfileDataMap &map );
	bool	IsTextureUsed( int index );
	BOOL	GetShaderPrivateData(Q* shader);
	float	GetMultiplayerFromTextures(float &convergenceShift);
	static DWORD	TexToSamplerIndex(DWORD Stage) 
	{ return T::stageOffset + Stage; }
	int		GetShaderIndex() 
	{ return m_CurrentShader && m_bShaderDataAvailable ? m_CurrentShaderData.shaderGlobalIndex : -1; }
};

#include "BaseStereoRenderer_InternalClasses-inl.h"
