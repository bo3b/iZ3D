#pragma once

#include "..\CommonUtils\System.h"

#define FOURCC_DF16 MAKEFOURCC( 'D', 'F', '1', '6' )
#define FOURCC_DF24 MAKEFOURCC( 'D', 'F', '2', '4' )
#define FOURCC_RAWZ MAKEFOURCC( 'R', 'A', 'W', 'Z' )
#define FOURCC_INTZ MAKEFOURCC( 'I', 'N', 'T', 'Z' )
#define FOURCC_NVCS MAKEFOURCC( 'N', 'V', 'C', 'S' )
#define FOURCC_ATI1 MAKEFOURCC( 'A', 'T', 'I', '1' )
#define FOURCC_ATI2 MAKEFOURCC( 'A', 'T', 'I', '2' )
#define FOURCC_AI44 MAKEFOURCC( 'A', 'I', '4', '4' )
#define FOURCC_IA44 MAKEFOURCC( 'I', 'A', '4', '4' )
#define FOURCC_NULL MAKEFOURCC( 'N', 'U', 'L', 'L' )
#define FOURCC_YV12 MAKEFOURCC( 'Y', 'V', '1', '2' )

UINT BitsPerPixel( D3DFORMAT fmt );

bool IsCompressedFormat( D3DFORMAT Format );

void ConvertCaps(  D3DCAPS9 &caps9, D3DCAPS8* pCaps );

void ConvertPresentParameters(D3DPRESENT_PARAMETERS8 *pPpDx8, D3DPRESENT_PARAMETERS *pPpDx9);

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