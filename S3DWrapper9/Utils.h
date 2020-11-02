#pragma once

#include "..\CommonUtils\System.h"

#define DUMP_FILE_FORMAT		D3DXIFF_JPG
#define DUMP_FILE_EXT			L"jpg"
#define DUMP_FILE_FLOATFORMAT	D3DXIFF_HDR
#define DUMP_FILE_FLOATEXT		L"hdr"
#define DUMP_FILE_STEREOFORMAT	D3DXIFF_JPG
#define DUMP_FILE_STEREOEXT		L"jps"

inline const TCHAR* GetFileNameFromPath(const TCHAR* path)
{
	const TCHAR* fn = _tcschr(path, '\\');
	if (fn)
		return fn + 1;
	else
		return path;
}

#define FOURCC_DF16 ((D3DFORMAT) MAKEFOURCC( 'D', 'F', '1', '6' ))
#define FOURCC_DF24 ((D3DFORMAT) MAKEFOURCC( 'D', 'F', '2', '4' ))
#define FOURCC_RAWZ ((D3DFORMAT) MAKEFOURCC( 'R', 'A', 'W', 'Z' ))
#define FOURCC_INTZ ((D3DFORMAT) MAKEFOURCC( 'I', 'N', 'T', 'Z' ))
#define FOURCC_NVCS ((D3DFORMAT) MAKEFOURCC( 'N', 'V', 'C', 'S' ))
#define FOURCC_ATI1 ((D3DFORMAT) MAKEFOURCC( 'A', 'T', 'I', '1' ))
#define FOURCC_ATI2 ((D3DFORMAT) MAKEFOURCC( 'A', 'T', 'I', '2' ))
#define FOURCC_AI44 ((D3DFORMAT) MAKEFOURCC( 'A', 'I', '4', '4' ))
#define FOURCC_IA44 ((D3DFORMAT) MAKEFOURCC( 'I', 'A', '4', '4' ))
#define FOURCC_NULL ((D3DFORMAT) MAKEFOURCC( 'N', 'U', 'L', 'L' ))
#define FOURCC_YV12 ((D3DFORMAT) MAKEFOURCC( 'Y', 'V', '1', '2' ))

CONST TCHAR* GetStereoImageFileExtension( );
CONST TCHAR* GetImageFileExtension( );
D3DXIMAGE_FILEFORMAT GetImageFileFormat( );
DWORD GetILImageFileFormat( );

HRESULT	SaveTextureToFile(LPTSTR pDestFile, LPDIRECT3DBASETEXTURE9 pSrcTexture, D3DXIMAGE_FILEFORMAT DestFormat = D3DXIFF_FORCE_DWORD);
HRESULT	SaveSurfaceToFile(LPTSTR pDestFile, LPDIRECT3DSURFACE9 pSrcSurface, D3DXIMAGE_FILEFORMAT DestFormat = D3DXIFF_FORCE_DWORD, const RECT *pSrcRect = 0);
HRESULT	SaveVolumeToFile(LPTSTR pDestFile, LPDIRECT3DVOLUME9 pSrcVolume, D3DXIMAGE_FILEFORMAT DestFormat = D3DXIFF_FORCE_DWORD);
HRESULT LoadTextureFromResource(LPDIRECT3DDEVICE9 pd3dDevice, HMODULE hModule, TCHAR* strResName, TCHAR* strResType, D3DXIMAGE_INFO* pSrcInfo, LPDIRECT3DTEXTURE9* ppTex);

UINT BitsPerPixel( D3DFORMAT fmt );
bool IsCompressedFormat( D3DFORMAT Format );
UINT GetVertexCount(D3DPRIMITIVETYPE PrimitiveType, UINT PrimitiveCount);

inline UINT GetSurfaceSize(D3DSURFACE_DESC &Desc)
{
	UINT Width = Desc.Width;
	UINT Height = Desc.Height;
	if (IsCompressedFormat(Desc.Format))
	{
		Width = RoundUp(Width, 4);
		Height = RoundUp(Height, 4);
	}
	DWORD MSAA = (Desc.MultiSampleType == D3DMULTISAMPLE_NONE ? 1 : Desc.MultiSampleType);
	return MSAA * BitsPerPixel(Desc.Format) * Width * Height / 8;
}

inline UINT GetVolumeSize(D3DVOLUME_DESC &Desc)
{
	UINT Width = Desc.Width;
	UINT Height = Desc.Height;
	if (IsCompressedFormat(Desc.Format))
	{
		Width = RoundUp(Width, 4);
		Height = RoundUp(Height, 4);
	}
	return BitsPerPixel(Desc.Format) * Width * Height * Desc.Depth/ 8;
}

inline BOOL WINAPI SetRect(LPRECT lprc, int xLeft, int yTop, int xRight, int yBottom)
{
	lprc->left = xLeft;
	lprc->top = yTop;
	lprc->right = xRight;
	lprc->bottom = yBottom;
	return TRUE;
}

inline BOOL WINAPI OffsetRect(LPRECT lprc, int dx, int dy)
{
	lprc->left += dx;
	lprc->right += dx;
	lprc->top += dy;
	lprc->bottom += dy;
	return TRUE;
}

inline void InitViewport(D3DVIEWPORT9* pVp, DWORD X, DWORD Y, DWORD Width, DWORD Height, FLOAT MinZ = 0.0f, FLOAT MaxZ = 1.0f)
{
	pVp->X = X;
	pVp->Y = Y;
	pVp->Width = Width;
	pVp->Height = Height;
	pVp->MinZ = MinZ;
	pVp->MaxZ = MaxZ;
}

void	SetObjectName(IDirect3DSurface9 *pSurface, LPCTSTR Name);
LPCTSTR	GetObjectName(IDirect3DSurface9 *pSurface);
void	SetObjectName(IDirect3DVolume9 *pSurface, LPCTSTR Name);
LPCTSTR	GetObjectName(IDirect3DVolume9 *pSurface);
void	SetObjectName(IDirect3DTexture9 *pTexture, LPCTSTR Name);
void	SetObjectName(IDirect3DVolumeTexture9 *pTexture, LPCTSTR Name);
void	SetObjectName(IDirect3DCubeTexture9 *pTexture, LPCTSTR Name);
void	SetObjectName(IDirect3DBaseTexture9 *pTexture, LPCTSTR Name);
LPCTSTR	GetObjectName(IDirect3DBaseTexture9 *pTexture);

bool	IsMcClaud(IDirect3DSurface9 *pSurface);
HRESULT	SetMcClaudFlag(IDirect3DSurface9 *pSurface);
HRESULT	ClearMcClaudFlag(IDirect3DSurface9 *pSurface);

inline LPCTSTR GetObjectName(IDirect3DSurface9 *pSurface, HRESULT hr)
{
	if(SUCCEEDED(hr))
		return GetObjectName(pSurface);
	return _T("");
}

inline LPCTSTR GetObjectName(IDirect3DTexture9 *pTexture, HRESULT hr)
{
	if(SUCCEEDED(hr))
		return GetObjectName(pTexture);
	return _T("");
}

inline LPCTSTR GetObjectName(IDirect3DBaseTexture9 *pTexture, HRESULT hr)
{
	if(SUCCEEDED(hr))
		return GetObjectName(pTexture);
	return _T("");
}

IDirect3DBaseTexture9 *	GetParentTexture(IDirect3DSurface9* pSurf, IDirect3DBaseTexture9** ppTex);
IDirect3DTexture9 *		GetParentTexture(IDirect3DSurface9* pSurf, IDirect3DTexture9** ppTex);

void	SetSurfaceRevision(IDirect3DSurface9* pSurf, DWORD rev);
DWORD	GetSurfaceRevision(IDirect3DSurface9* pSurf);
void	IncSurfaceRevision(IDirect3DSurface9* pSurf);

TextureType	GetTextureType(IDirect3DBaseTexture9 *pTexture);
bool		SetTextureType(IDirect3DBaseTexture9 *pTexture, TextureType type);
TextureType	GetSurfaceType(IDirect3DSurface9 *pTexture);
bool		SetSurfaceType(IDirect3DSurface9 *pTexture, TextureType type);

HRESULT	GetStereoObject(IDirect3DBaseTexture9 *pLeft, IDirect3DBaseTexture9 **ppRight);
HRESULT	GetStereoObject(IDirect3DTexture9 *pLeft, IDirect3DTexture9 **ppRight);
HRESULT	GetStereoObject(IDirect3DSurface9 *pLeft, IDirect3DSurface9 **ppRight);
HRESULT	SetStereoObject(IDirect3DBaseTexture9 *pLeft, IDirect3DBaseTexture9 *pRight);
HRESULT	SetStereoObject(IDirect3DSurface9 *pLeft, IDirect3DSurface9 *pRight);
HRESULT	SetStereoObject(IDirect3DTexture9 *pLeft, IDirect3DTexture9 *pRight);
void	FreeStereoObject(IDirect3DBaseTexture9 *pLeft);
void	FreeStereoObject(IDirect3DTexture9 *pLeft);
void	FreeStereoObject(IDirect3DSurface9 *pLeft);

UINT    GetTextureFirstSurfaceSize(IDirect3DBaseTexture9* pTexture);
UINT	GetTextureSize(IDirect3DTexture9 *pTex);
UINT	GetTextureSize(IDirect3DBaseTexture9* pTexture);
UINT	GetSurfaceSize(IDirect3DSurface9 *pSurf);
UINT	GetCubeTextureSize(IDirect3DCubeTexture9 *pTex);
UINT	GetVolumeSize(IDirect3DVolume9 *pSurf);
UINT	GetVolumeTextureSize(IDirect3DVolumeTexture9 *pTex);

inline bool S3DMatrixIsIdentity
( CONST D3DXMATRIX *pM )
{
	return pM->m[0][0] == 1.0f && pM->m[0][1] == 0.0f && pM->m[0][2] == 0.0f && pM->m[0][3] == 0.0f &&
		pM->m[1][0] == 0.0f && pM->m[1][1] == 1.0f && pM->m[1][2] == 0.0f && pM->m[1][3] == 0.0f &&
		pM->m[2][0] == 0.0f && pM->m[2][1] == 0.0f && pM->m[2][2] == 1.0f && pM->m[2][3] == 0.0f;
}

