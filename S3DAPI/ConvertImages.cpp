#include "stdafx.h"
#include "ConvertImages.h"
#include <D3DX9math.h>
#include <crtdbg.h>
#include <emmintrin.h>
#include <smmintrin.h>
#include "..\CommonUtils\System.h"
#include <squish.h>

#ifdef TEST
#define DNew new
#define DEBUG_MESSAGE __noop
#endif

bool	S3DAPI_API IsCompressedFormat( DXGI_FORMAT format )
{
	switch( format )
	{			
		case DXGI_FORMAT_BC1_TYPELESS:
		case DXGI_FORMAT_BC1_UNORM:
		case DXGI_FORMAT_BC1_UNORM_SRGB:
		case DXGI_FORMAT_BC2_TYPELESS:
		case DXGI_FORMAT_BC2_UNORM:
		case DXGI_FORMAT_BC2_UNORM_SRGB:
		case DXGI_FORMAT_BC3_TYPELESS:
		case DXGI_FORMAT_BC3_UNORM:
		case DXGI_FORMAT_BC3_UNORM_SRGB:
		case DXGI_FORMAT_BC4_TYPELESS:
		case DXGI_FORMAT_BC4_UNORM:
		case DXGI_FORMAT_BC4_SNORM:
		case DXGI_FORMAT_BC5_TYPELESS:
		case DXGI_FORMAT_BC5_UNORM:
		case DXGI_FORMAT_BC5_SNORM:
		case DXGI_FORMAT_BC6H_TYPELESS:
		case DXGI_FORMAT_BC6H_UF16:
		case DXGI_FORMAT_BC6H_SF16:
		case DXGI_FORMAT_BC7_TYPELESS:
		case DXGI_FORMAT_BC7_UNORM:
		case DXGI_FORMAT_BC7_UNORM_SRGB:
			return true;
	}
	return false;
}


size_t	S3DAPI_API GetBitWidthOfDXGIFormat( DXGI_FORMAT format )
{
	switch( format )
	{			
	case DXGI_FORMAT_R32G32B32A32_TYPELESS:
	case DXGI_FORMAT_R32G32B32A32_FLOAT:
	case DXGI_FORMAT_R32G32B32A32_UINT:
	case DXGI_FORMAT_R32G32B32A32_SINT:
		return 128;

	case DXGI_FORMAT_R32G32B32_TYPELESS:
	case DXGI_FORMAT_R32G32B32_FLOAT:
	case DXGI_FORMAT_R32G32B32_UINT:
	case DXGI_FORMAT_R32G32B32_SINT:
		return 96;

	case DXGI_FORMAT_R16G16B16A16_TYPELESS:
	case DXGI_FORMAT_R16G16B16A16_FLOAT:
	case DXGI_FORMAT_R16G16B16A16_UNORM:
	case DXGI_FORMAT_R16G16B16A16_UINT:
	case DXGI_FORMAT_R16G16B16A16_SNORM:
	case DXGI_FORMAT_R16G16B16A16_SINT:
	case DXGI_FORMAT_R32G32_TYPELESS:
	case DXGI_FORMAT_R32G32_FLOAT:
	case DXGI_FORMAT_R32G32_UINT:
	case DXGI_FORMAT_R32G32_SINT:
	case DXGI_FORMAT_R32G8X24_TYPELESS:
	case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
	case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
	case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
		return 64;

	case DXGI_FORMAT_R10G10B10A2_TYPELESS:
	case DXGI_FORMAT_R10G10B10A2_UNORM:
	case DXGI_FORMAT_R10G10B10A2_UINT:
	case DXGI_FORMAT_R11G11B10_FLOAT:
	case DXGI_FORMAT_R8G8B8A8_TYPELESS:
	case DXGI_FORMAT_R8G8B8A8_UNORM:
	case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
	case DXGI_FORMAT_R8G8B8A8_UINT:
	case DXGI_FORMAT_R8G8B8A8_SNORM:
	case DXGI_FORMAT_R8G8B8A8_SINT:
	case DXGI_FORMAT_R16G16_TYPELESS:
	case DXGI_FORMAT_R16G16_FLOAT:
	case DXGI_FORMAT_R16G16_UNORM:
	case DXGI_FORMAT_R16G16_UINT:
	case DXGI_FORMAT_R16G16_SNORM:
	case DXGI_FORMAT_R16G16_SINT:
	case DXGI_FORMAT_R32_TYPELESS:
	case DXGI_FORMAT_D32_FLOAT:
	case DXGI_FORMAT_R32_FLOAT:
	case DXGI_FORMAT_R32_UINT:
	case DXGI_FORMAT_R32_SINT:
	case DXGI_FORMAT_R24G8_TYPELESS:
	case DXGI_FORMAT_D24_UNORM_S8_UINT:
	case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
	case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
	case DXGI_FORMAT_B8G8R8A8_UNORM:
	case DXGI_FORMAT_B8G8R8X8_UNORM:
	case DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM:
	case DXGI_FORMAT_B8G8R8A8_TYPELESS:
	case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
	case DXGI_FORMAT_B8G8R8X8_TYPELESS:
	case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
		return 32;

	case DXGI_FORMAT_R8G8_TYPELESS:
	case DXGI_FORMAT_R8G8_UNORM:
	case DXGI_FORMAT_R8G8_UINT:
	case DXGI_FORMAT_R8G8_SNORM:
	case DXGI_FORMAT_R8G8_SINT:
	case DXGI_FORMAT_R16_TYPELESS:
	case DXGI_FORMAT_R16_FLOAT:
	case DXGI_FORMAT_D16_UNORM:
	case DXGI_FORMAT_R16_UNORM:
	case DXGI_FORMAT_R16_UINT:
	case DXGI_FORMAT_R16_SNORM:
	case DXGI_FORMAT_R16_SINT:
	case DXGI_FORMAT_B5G6R5_UNORM:
	case DXGI_FORMAT_B5G5R5A1_UNORM:
		return 16;

	case DXGI_FORMAT_R8_TYPELESS:
	case DXGI_FORMAT_R8_UNORM:
	case DXGI_FORMAT_R8_UINT:
	case DXGI_FORMAT_R8_SNORM:
	case DXGI_FORMAT_R8_SINT:
	case DXGI_FORMAT_A8_UNORM:
		return 8;

	// Compressed format; http://msdn2.microsoft.com/en-us/library/bb694531(VS.85).aspx
	case DXGI_FORMAT_BC2_TYPELESS:
	case DXGI_FORMAT_BC2_UNORM:
	case DXGI_FORMAT_BC2_UNORM_SRGB:
	case DXGI_FORMAT_BC3_TYPELESS:
	case DXGI_FORMAT_BC3_UNORM:
	case DXGI_FORMAT_BC3_UNORM_SRGB:
	case DXGI_FORMAT_BC5_TYPELESS:
	case DXGI_FORMAT_BC5_UNORM:
	case DXGI_FORMAT_BC5_SNORM:
		return 128;

	// Compressed format; http://msdn2.microsoft.com/en-us/library/bb694531(VS.85).aspx
	case DXGI_FORMAT_R1_UNORM:
	case DXGI_FORMAT_BC1_TYPELESS:
	case DXGI_FORMAT_BC1_UNORM:
	case DXGI_FORMAT_BC1_UNORM_SRGB:
	case DXGI_FORMAT_BC4_TYPELESS:
	case DXGI_FORMAT_BC4_UNORM:
	case DXGI_FORMAT_BC4_SNORM:
		return 64;

		// Compressed format; http://msdn.microsoft.com/en-us/library/ee416559%28VS.85%29.aspx
	case DXGI_FORMAT_BC6H_TYPELESS:
	case DXGI_FORMAT_BC6H_UF16:
	case DXGI_FORMAT_BC6H_SF16:
		return 128;

		// Compressed format; http://msdn.microsoft.com/en-us/library/ee416559%28VS.85%29.aspx
	case DXGI_FORMAT_BC7_TYPELESS:
	case DXGI_FORMAT_BC7_UNORM:
	case DXGI_FORMAT_BC7_UNORM_SRGB:
		return 128;

	// Compressed format; http://msdn2.microsoft.com/en-us/library/bb694531(VS.85).aspx
	case DXGI_FORMAT_R9G9B9E5_SHAREDEXP:
		return 32;

	// These are compressed, but bit-size information is unclear.
	case DXGI_FORMAT_R8G8_B8G8_UNORM:
	case DXGI_FORMAT_G8R8_G8B8_UNORM:
		return 32;

	case DXGI_FORMAT_UNKNOWN:
		break;
	}
	_ASSERT(FALSE);
	return 0;
}

DXGI_FORMAT S3DAPI_API GetTypedDXGIFormat( DXGI_FORMAT format )
{
	switch( format )
	{
	case	DXGI_FORMAT_R32G32B32A32_TYPELESS:		format = DXGI_FORMAT_R32G32B32A32_FLOAT;		break;
	case	DXGI_FORMAT_R32G32B32_TYPELESS:			format = DXGI_FORMAT_R32G32B32_FLOAT;			break;
	case	DXGI_FORMAT_R16G16B16A16_TYPELESS:		format = DXGI_FORMAT_R16G16B16A16_FLOAT;		break;
	case	DXGI_FORMAT_R32G32_TYPELESS:			format = DXGI_FORMAT_R32G32_FLOAT;				break;

	case	DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
	case	DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
	case	DXGI_FORMAT_R32G8X24_TYPELESS:			format = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;		break;

	case	DXGI_FORMAT_R10G10B10A2_TYPELESS:		format = DXGI_FORMAT_R10G10B10A2_UNORM;			break;
	case	DXGI_FORMAT_R8G8B8A8_TYPELESS:			format = DXGI_FORMAT_R8G8B8A8_UNORM;			break;
	case	DXGI_FORMAT_R16G16_TYPELESS:			format = DXGI_FORMAT_R16G16_FLOAT;				break;
	case	DXGI_FORMAT_R32_TYPELESS:				format = DXGI_FORMAT_R32_FLOAT;					break;

	case	DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
	case	DXGI_FORMAT_X24_TYPELESS_G8_UINT:
	case	DXGI_FORMAT_R24G8_TYPELESS:				format = DXGI_FORMAT_D24_UNORM_S8_UINT;			break;

	case	DXGI_FORMAT_R8G8_TYPELESS:				format = DXGI_FORMAT_R8G8_UNORM;				break;
	case	DXGI_FORMAT_R16_TYPELESS:				format = DXGI_FORMAT_R16_FLOAT;					break;
	case	DXGI_FORMAT_R8_TYPELESS:				format = DXGI_FORMAT_R8_UNORM;					break;
	case	DXGI_FORMAT_BC1_TYPELESS:				format = DXGI_FORMAT_BC1_UNORM;					break;
	case	DXGI_FORMAT_BC2_TYPELESS:				format = DXGI_FORMAT_BC2_UNORM;					break;
	case	DXGI_FORMAT_BC3_TYPELESS:				format = DXGI_FORMAT_BC3_UNORM;					break;
	case	DXGI_FORMAT_BC4_TYPELESS:				format = DXGI_FORMAT_BC4_UNORM;					break;
	case	DXGI_FORMAT_BC5_TYPELESS:				format = DXGI_FORMAT_BC5_UNORM;					break;
	}
	return format;
}

DXGI_FORMAT S3DAPI_API ConvertFormatD3D9ToDXGI( D3DFORMAT fmt )
{
	switch( fmt )
	{
	case D3DFMT_UNKNOWN:
		return DXGI_FORMAT_UNKNOWN;
	case D3DFMT_R8G8B8:
	case D3DFMT_A8R8G8B8:
	case D3DFMT_X8R8G8B8:
		return DXGI_FORMAT_R8G8B8A8_UNORM;
	case D3DFMT_R5G6B5:
		return DXGI_FORMAT_B5G6R5_UNORM;
	case D3DFMT_X1R5G5B5:
	case D3DFMT_A1R5G5B5:
		return DXGI_FORMAT_B5G5R5A1_UNORM;
	case D3DFMT_A4R4G4B4:
		return DXGI_FORMAT_R8G8B8A8_UNORM;
	case D3DFMT_R3G3B2:
		return DXGI_FORMAT_R8G8B8A8_UNORM;
	case D3DFMT_A8:
		return DXGI_FORMAT_A8_UNORM;
	case D3DFMT_A8R3G3B2:
		return DXGI_FORMAT_R8G8B8A8_UNORM;
	case D3DFMT_X4R4G4B4:
		return DXGI_FORMAT_R8G8B8A8_UNORM;
	case D3DFMT_A2B10G10R10:
		return DXGI_FORMAT_R10G10B10A2_UNORM;
	case D3DFMT_A8B8G8R8:
	case D3DFMT_X8B8G8R8:
		return DXGI_FORMAT_B8G8R8A8_UNORM;
	case D3DFMT_G16R16:
		return DXGI_FORMAT_R16G16_UNORM;
	case D3DFMT_A2R10G10B10:
		return DXGI_FORMAT_R10G10B10A2_UNORM;
	case D3DFMT_A16B16G16R16:
		return DXGI_FORMAT_R16G16B16A16_UNORM;
	case D3DFMT_R16F:
		return DXGI_FORMAT_R16_FLOAT;
	case D3DFMT_G16R16F:
		return DXGI_FORMAT_R16G16_FLOAT;
	case D3DFMT_A16B16G16R16F:
		return DXGI_FORMAT_R16G16B16A16_FLOAT;
	case D3DFMT_R32F:
		return DXGI_FORMAT_R32_FLOAT;
	case D3DFMT_G32R32F:
		return DXGI_FORMAT_R32G32_FLOAT;
	case D3DFMT_A32B32G32R32F:
		return DXGI_FORMAT_R32G32B32A32_FLOAT;
	}
	return DXGI_FORMAT_UNKNOWN;
}


D3DFORMAT S3DAPI_API ConvertFormatDXGIToD3D9( DXGI_FORMAT fmt )
{
	switch( fmt )
	{
	case DXGI_FORMAT_UNKNOWN:
		return D3DFMT_UNKNOWN;
	case DXGI_FORMAT_R8G8B8A8_UNORM:
		return D3DFMT_A8R8G8B8;
	case DXGI_FORMAT_B5G6R5_UNORM:
		return D3DFMT_R5G6B5;
	case DXGI_FORMAT_B5G5R5A1_UNORM:
		return D3DFMT_A1R5G5B5;
	case DXGI_FORMAT_A8_UNORM:
		return D3DFMT_A8;
	case DXGI_FORMAT_R10G10B10A2_UNORM:
		return D3DFMT_A2B10G10R10;
	case DXGI_FORMAT_B8G8R8A8_UNORM:
		return D3DFMT_A8B8G8R8;
	case DXGI_FORMAT_R16G16_UNORM:
		return D3DFMT_G16R16;
	case DXGI_FORMAT_R16G16B16A16_UNORM:
		return D3DFMT_A16B16G16R16;
	case DXGI_FORMAT_R16_FLOAT:
		return D3DFMT_R16F;
	case DXGI_FORMAT_R16G16_FLOAT:
		return D3DFMT_G16R16F;
	case DXGI_FORMAT_R16G16B16A16_FLOAT:
		return D3DFMT_A16B16G16R16F;
	case DXGI_FORMAT_R32_FLOAT:
		return D3DFMT_R32F;
	case DXGI_FORMAT_R32G32_FLOAT:
		return D3DFMT_G32R32F;
	case DXGI_FORMAT_R32G32B32A32_FLOAT:
		return D3DFMT_A32B32G32R32F;
	}
	return D3DFMT_UNKNOWN;
}

void S3DAPI_API SaveImageToBMP( const TCHAR * filename, DWORD width, DWORD height, char * bmpBuffer )
{
	_ASSERT(bmpBuffer);
	FILE *filePtr;	
	if (_tfopen_s(&filePtr, filename, _T("wb")))
		return;

	BITMAPFILEHEADER bitmapFileHeader;
	bitmapFileHeader.bfType			= 0x4D42; //"BM"
	bitmapFileHeader.bfSize			= width * height * 4;
	bitmapFileHeader.bfReserved1	= 0;
	bitmapFileHeader.bfReserved2	= 0;
	bitmapFileHeader.bfOffBits		= sizeof(BITMAPFILEHEADER) + sizeof(BITMAPV4HEADER);

	BITMAPV4HEADER bitmapInfoHeader;
	bitmapInfoHeader.bV4Size			= sizeof(BITMAPV4HEADER);
	bitmapInfoHeader.bV4Width			= width;
	bitmapInfoHeader.bV4Height			= height;
	bitmapInfoHeader.bV4Planes			= 1;
	bitmapInfoHeader.bV4BitCount		= 32;
	bitmapInfoHeader.bV4V4Compression	= BI_BITFIELDS;
	bitmapInfoHeader.bV4SizeImage		= width * height * 4;	// must be zero for BI_RGB
	bitmapInfoHeader.bV4XPelsPerMeter	= 0;
	bitmapInfoHeader.bV4YPelsPerMeter	= 0;
	bitmapInfoHeader.bV4ClrUsed			= 0;
	bitmapInfoHeader.bV4ClrImportant	= 0;
	bitmapInfoHeader.bV4RedMask			= 0xff <<  0;
	bitmapInfoHeader.bV4GreenMask		= 0xff <<  8;
	bitmapInfoHeader.bV4BlueMask		= 0xff << 16;
	bitmapInfoHeader.bV4AlphaMask		= 0xff << 24;
	bitmapInfoHeader.bV4CSType			= LCS_CALIBRATED_RGB;
	memset( &bitmapInfoHeader.bV4Endpoints, 0, sizeof(CIEXYZTRIPLE) );
	bitmapInfoHeader.bV4GammaRed		= 0;
	bitmapInfoHeader.bV4GammaGreen		= 0;
	bitmapInfoHeader.bV4GammaBlue		= 0;

	fwrite(&bitmapFileHeader, sizeof(BITMAPFILEHEADER),	1, filePtr);
	fwrite(&bitmapInfoHeader, sizeof(BITMAPV4HEADER),	1, filePtr);
	// flip image vertically
	bmpBuffer += (height-1) * width * 4;
	for( DWORD i=0; i<height; ++i )
	{
		fwrite(bmpBuffer, width * 4, 1, filePtr);
		bmpBuffer -= width * 4;
	}
	fclose(filePtr);
}

float F16_To_F32(unsigned short s, unsigned short e, unsigned short m)
{
	union
	{
		unsigned int i;
		float f;
	} fl;
	const unsigned int mBits = 10;
	const unsigned int mMask = 1 << mBits;
	if (e == 0)
	{
		if (m == 0)
		{
			// Plus zero
			fl.i = s << 31;
			return fl.f;
		}
		else
		{
			// Denormalized number -- renormalize it
			while (!(m & mMask))
			{
				m <<= 1;
				e -=  1;
			}

			e += 1;
			m &= ~mMask;
		}
	}
	else if (e == 31)
	{
		if (m == 0)
		{
			// Positive or negative infinity
			fl.i = (s << 31) | 0x7f800000;
			return fl.f;
		}
		else
		{
			// Nan -- preserve sign and significant bits
			fl.i = (s << 31) | 0x7f800000 | (m << (23 - mBits));
			return fl.f;
		}
	}

	// Normalized number
	e = e + (127 - 15);
	m = m << (23 - mBits);

	// Assemble s, e and m.
	fl.i = (s << 31) | (e << 23) | m;
	return fl.f;
}

float F11_To_F32(unsigned short e, unsigned short m)
{
	union
	{
		unsigned int i;
		float f;
	} fl;
	const unsigned int s = 0;
	const unsigned int mBits = 6;
	const unsigned int mMask = 1 << mBits;
	if (e == 0)
	{
		if (m == 0)
		{
			// Plus zero
			fl.i = s << 31;
			return fl.f;
		}
		else
		{
			// Denormalized number -- renormalize it
			while (!(m & mMask))
			{
				m <<= 1;
				e -=  1;
			}
 
			e += 1;
			m &= ~mMask;
		}
	}
	else if (e == 31)
	{
		if (m == 0)
		{
			// Positive or negative infinity
			fl.i = (s << 31) | 0x7f800000;
			return fl.f;
		}
		else
		{
			// Nan -- preserve sign and significant bits
			fl.i = (s << 31) | 0x7f800000 | (m << (23 - mBits));
			return fl.f;
		}
	}

	// Normalized number
	e = e + (127 - 15);
	m = m << (23 - mBits);

	// Assemble s, e and m.
	fl.i = (s << 31) | (e << 23) | m;
	return fl.f;
}

float F10_To_F32(unsigned short e, unsigned short m)
{	
	union
	{
		unsigned int i;
		float f;
	} fl;
	const unsigned int s = 0;
	const unsigned int mBits = 5;
	const unsigned int mMask = 1 << mBits;
	if (e == 0)
	{
		if (m == 0)
		{
			// Plus zero
			fl.i = s << 31;
			return fl.f;
		}
		else
		{
			// Denormalized number -- renormalize it
			while (!(m & mMask))
			{
				m <<= 1;
				e -=  1;
			}
 
			e += 1;
			m &= ~mMask;
		}
	}
	else 
	if (e == 31)
	{
		if (m == 0)
		{
			// Positive or negative infinity
			fl.i = (s << 31) | 0x7f800000;
			return fl.f;
		}
		else
		{
			// Nan -- preserve sign and significant bits
			fl.i = (s << 31) | 0x7f800000 | (m << (23 - mBits));
			return fl.f;
		}
	}

	// Normalized number
	e = e + (127 - 15);
	m = m << (23 - mBits);

	// Assemble s, e and m.
	fl.i = (s << 31) | (e << 23) | m;
	return fl.f;
}

void S3DAPI_API ConvertImageFrom_R16_FLOAT_To_R32_FLOAT(void* __restrict srcBuffer, void* __restrict dstBuffer, DWORD width, DWORD height, UINT rowPitch)
{
#if 0 // C version of code, only for debug
	struct SINGLE_FLOAT
	{
		union {
			struct {
				unsigned __int32 R_m : 23;
				unsigned __int32 R_e : 8;
				unsigned __int32 R_s : 1;
			};
			struct {
				float r;
			};
		};
	};
	C_ASSERT(sizeof(SINGLE_FLOAT) == 4);
	struct HALF_FLOAT
	{
		unsigned __int16 R_m : 10;
		unsigned __int16 R_e : 5;
		unsigned __int16 R_s : 1;
	};
	C_ASSERT(sizeof(HALF_FLOAT) == 2);
	SINGLE_FLOAT* d = (SINGLE_FLOAT*)dstBuffer;
	for(DWORD j = 0; j< height; j++)
	{
		HALF_FLOAT* s = (HALF_FLOAT*)((char*)srcBuffer + rowPitch * j);
		for(DWORD i = 0; i< width; i++)
		{
			d->R_s = s->R_s;
			d->R_e = s->R_e - 15 + 127;
			d->R_m = s->R_m << (23-10);
			d++;
			s++;
		}
	}
#else
	float* d = (float*)dstBuffer;
	D3DXFLOAT16* s = (D3DXFLOAT16*)srcBuffer;
	for(DWORD j = 0; j< height; j++)
	{
		D3DXFloat16To32Array(d, s, width);
		s = (D3DXFLOAT16*)((char*)s + rowPitch);
		d += width;
	}
#endif
}

union SINGLE_FLOAT4
{
	struct {
		unsigned __int32 R_m : 23;
		unsigned __int32 R_e : 8;
		unsigned __int32 R_s : 1;
		unsigned __int32 G_m : 23;
		unsigned __int32 G_e : 8;
		unsigned __int32 G_s : 1;
		unsigned __int32 B_m : 23;
		unsigned __int32 B_e : 8;
		unsigned __int32 B_s : 1;
		unsigned __int32 A_m : 23;
		unsigned __int32 A_e : 8;
		unsigned __int32 A_s : 1;
	};
	struct {
		float r;
		float g;
		float b;
		float a;
	};
};
C_ASSERT(sizeof(SINGLE_FLOAT4) == 4 * 4);

void S3DAPI_API ConvertImageFrom_R16_FLOAT_To_R32G32B32A32_FLOAT(void* __restrict srcBuffer, void* __restrict dstBuffer, DWORD width, DWORD height, UINT rowPitch)
{
	struct HALF_FLOAT
	{
		unsigned __int16 R_m : 10;
		unsigned __int16 R_e : 5;
		unsigned __int16 R_s : 1;
	};
	C_ASSERT(sizeof(HALF_FLOAT) == 2);
	SINGLE_FLOAT4* d = (SINGLE_FLOAT4*)dstBuffer;
	for(DWORD j = 0; j< height; j++)
	{
		HALF_FLOAT* s = (HALF_FLOAT*)((char*)srcBuffer + rowPitch * j);
		for(DWORD i = 0; i< width; i++)
		{
			d->R_s = s->R_s;
			d->R_e = s->R_e != 0x1F ? s->R_e - 15 + 127 : 0xFF;
			d->R_m = s->R_m << (23-10);
			d->g = d->r;
			d->b = d->r;
			d->a = 1.0f;
			d++;
			s++;
		}
	}
}

void S3DAPI_API ConvertImageFrom_R16G16_FLOAT_To_R32G32B32A32_FLOAT(void* __restrict srcBuffer, void* __restrict dstBuffer, DWORD width, DWORD height, UINT rowPitch)
{
	struct HALF_FLOAT2
	{
		unsigned __int16 R_m : 10;
		unsigned __int16 R_e : 5;
		unsigned __int16 R_s : 1;
		unsigned __int16 G_m : 10;
		unsigned __int16 G_e : 5;
		unsigned __int16 G_s : 1;
	};
	C_ASSERT(sizeof(HALF_FLOAT2) == 4);
	SINGLE_FLOAT4* d = (SINGLE_FLOAT4*)dstBuffer;
	for(DWORD j = 0; j< height; j++)
	{
		HALF_FLOAT2* s = (HALF_FLOAT2*)((char*)srcBuffer + rowPitch * j);
		for(DWORD i = 0; i< width; i++)
		{
			d->R_s = s->R_s;
			d->R_e = s->R_e != 0x1F ? s->R_e - 15 + 127 : 0xFF;
			d->R_m = s->R_m << (23-10);
			d->G_s = s->G_s;
			d->G_e = s->G_e != 0x1F ? s->G_e - 15 + 127 : 0xFF;
			d->G_m = s->G_m << (23-10);
			d->b = 0.0f;
			d->a = 1.0f;
			d++;
			s++;
		}
	}
}

void S3DAPI_API ConvertImageFrom_R16G16B16A16_FLOAT_To_R32G32B32A32_FLOAT(void* __restrict srcBuffer, void* __restrict dstBuffer, DWORD width, DWORD height, UINT rowPitch)
{
#if 0
	struct HALF_FLOAT4
	{
		unsigned __int16 R_m : 10;
		unsigned __int16 R_e : 5;
		unsigned __int16 R_s : 1;
		unsigned __int16 G_m : 10;
		unsigned __int16 G_e : 5;
		unsigned __int16 G_s : 1;
		unsigned __int16 B_m : 10;
		unsigned __int16 B_e : 5;
		unsigned __int16 B_s : 1;
		unsigned __int16 A_m : 10;
		unsigned __int16 A_e : 5;
		unsigned __int16 A_s : 1;
	};
	C_ASSERT(sizeof(HALF_FLOAT4) == 8);
	SINGLE_FLOAT4* d = (SINGLE_FLOAT4*)dstBuffer;
	for(DWORD j = 0; j< height; j++)
	{
		HALF_FLOAT4* s = (HALF_FLOAT4*)((char*)srcBuffer + rowPitch * j);
		for(DWORD i = 0; i< width; i++)
		{
			d->R_s = s->R_s;
			d->R_e = s->R_e != 0x1F ? s->R_e - 15 + 127 : 0xFF;
			d->R_m = s->R_m << (23-10);
			d->G_s = s->G_s;
			d->G_e = s->G_e != 0x1F ? s->G_e - 15 + 127 : 0xFF;
			d->G_m = s->G_m << (23-10);
			d->B_s = s->B_s;
			d->B_e = s->B_e != 0x1F ? s->B_e - 15 + 127 : 0xFF;
			d->B_m = s->B_m << (23-10);
			d->A_s = s->A_s;
			d->A_e = s->A_e != 0x1F ? s->A_e - 15 + 127 : 0xFF;
			d->A_m = s->A_m << (23-10);
			d++;
			s++;
		}
	}
#else
	float* d = (float*)dstBuffer;
	D3DXFLOAT16* s = (D3DXFLOAT16*)srcBuffer;
	for(DWORD j = 0; j< height; j++)
	{
		D3DXFloat16To32Array(d, s, width * 4);
		s = (D3DXFLOAT16*)((char*)s + rowPitch);
		d += width * 4;
	}
#endif
}

void S3DAPI_API ConvertImageFrom_R11G11B10_FLOAT_To_R32G32B32A32_FLOAT(void* __restrict srcBuffer, void* __restrict dstBuffer, DWORD width, DWORD height, UINT rowPitch)
{
	union SINGLE_FLOAT4
	{
		struct {
			unsigned __int32 R_m : 23;
			unsigned __int32 R_e : 8;
			unsigned __int32 R_s : 1;
			unsigned __int32 G_m : 23;
			unsigned __int32 G_e : 8;
			unsigned __int32 G_s : 1;
			unsigned __int32 B_m : 23;
			unsigned __int32 B_e : 8;
			unsigned __int32 B_s : 1;
			unsigned __int32 A_m : 23;
			unsigned __int32 A_e : 8;
			unsigned __int32 A_s : 1;
		};
		struct {
			float r;
			float g;
			float b;
			float a;
		};
	};
	C_ASSERT(sizeof(SINGLE_FLOAT4) == 4 * 4);
	struct FLOAT_R11G11B10_4
	{
		unsigned __int16 R_m : 6;
		unsigned __int16 R_e : 5;
		unsigned __int16 G_mL : 5; // for correct size
		unsigned __int16 G_mH : 1;
		unsigned __int16 G_e : 5;
		unsigned __int16 B_m : 5;
		unsigned __int16 B_e : 5;
	};
	C_ASSERT(sizeof(FLOAT_R11G11B10_4) == 4);
	SINGLE_FLOAT4* d = (SINGLE_FLOAT4*)dstBuffer;
	for(DWORD j = 0; j< height; j++)
	{
		FLOAT_R11G11B10_4* s = (FLOAT_R11G11B10_4*)((char*)srcBuffer + rowPitch * j);
		for(DWORD i = 0; i< width; i++)
		{
			d->R_s = 0;
			d->R_e = s->R_e != 0x1F ? s->R_e - 15 + 127 : 0xFF;
			d->R_m = s->R_m << (23-6);
			d->G_s = 0;
			d->G_e = s->G_e != 0x1F ? s->G_e - 15 + 127 : 0xFF;
			d->G_m = (s->G_mH << 5 | s->G_mL) << (23-6);
			d->B_s = 0;
			d->B_e = s->B_e != 0x1F ? s->B_e - 15 + 127 : 0xFF;
			d->B_m = s->B_m << (23-5);
			d->a = 1.0f;
			d++;
			s++;
		}
	}
}

void S3DAPI_API ConvertImageFrom_R10G10B10A2_UNORM_To_R32G32B32A32_FLOAT(void* __restrict srcBuffer, void* __restrict dstBuffer, DWORD width, DWORD height, UINT rowPitch)
{
	union SINGLE_FLOAT4
	{
		struct {
			unsigned __int32 R_m : 23;
			unsigned __int32 R_e : 8;
			unsigned __int32 R_s : 1;
			unsigned __int32 G_m : 23;
			unsigned __int32 G_e : 8;
			unsigned __int32 G_s : 1;
			unsigned __int32 B_m : 23;
			unsigned __int32 B_e : 8;
			unsigned __int32 B_s : 1;
			unsigned __int32 A_m : 23;
			unsigned __int32 A_e : 8;
			unsigned __int32 A_s : 1;
		};
		struct {
			float r;
			float g;
			float b;
			float a;
		};
	};
	C_ASSERT(sizeof(SINGLE_FLOAT4) == 4 * 4);
#pragma pack(push, 1)
	struct UINT_R10G10B10A2_4
	{
		unsigned __int16 R : 10;
		unsigned __int16 G_L : 6;
		unsigned __int16 G_H : 4;
		unsigned __int16 B : 10;
		unsigned __int16 A : 2;
	};
#pragma pack(pop)
	C_ASSERT(sizeof(UINT_R10G10B10A2_4) == 4);
	const unsigned __int16 MAX_10BIT = 1023;
	const float fScale = 1.0f / MAX_10BIT;
	SINGLE_FLOAT4* d = (SINGLE_FLOAT4*)dstBuffer;
	for(DWORD j = 0; j< height; j++)
	{
		UINT_R10G10B10A2_4* s = (UINT_R10G10B10A2_4*)((char*)srcBuffer + rowPitch * j);
		for(DWORD i = 0; i< width; i++)
		{
			d->r = s->R * fScale;
			d->g = ((s->G_H << 6) | s->G_L) * fScale;
			d->b = s->B * fScale;
			d->a = s->A / 3.0f;
			d++;
			s++;
		}
	}
}

void S3DAPI_API ConvertImageFrom_R32_FLOAT_To_R32G32B32A32_FLOAT(void* __restrict srcBuffer, void* __restrict dstBuffer, DWORD width, DWORD height, UINT rowPitch)
{
	float* d = (float*)dstBuffer;
	for(DWORD j = 0; j< height; j++)
	{
		float* s = (float*)((char*)srcBuffer + rowPitch * j);
		for(DWORD i = 0; i< width; i++)
		{
			d[0] = d[1] = d[2] = *s;
			d[3] = 1.0f;
			d += 4;
			s++;
		}
	}
}

void S3DAPI_API ConvertImageFrom_R32G32_FLOAT_To_R32G32B32A32_FLOAT(void* __restrict srcBuffer, void* __restrict dstBuffer, DWORD width, DWORD height, UINT rowPitch)
{
	float* d = (float*)dstBuffer;
	for(DWORD j = 0; j< height; j++)
	{
		float* s = (float*)((char*)srcBuffer + rowPitch * j);
		for(DWORD i = 0; i< width; i++)
		{
			d[0] = *s;
			s++;
			d[1] = *s;
			s++;
			d[2] = 0.0f;
			d[3] = 1.0f;
			d += 4;
		}
	}
}

void S3DAPI_API ConvertImageFrom_R24G8_UNORM_To_R32G32B32A32_FLOAT(void* __restrict srcBuffer, void* __restrict dstBuffer, DWORD width, DWORD height, UINT rowPitch)
{
	float* d = (float*)dstBuffer;
	for(DWORD j = 0; j< height; j++)
	{
		DWORD* s = (DWORD*)((char*)srcBuffer + rowPitch * j);
		for(DWORD i = 0; i< width; i++)
		{
			d[0] = d[1] = d[2] = (*s & 0xFFFFFF) * 1.0f / 0xffffff;
			d[3] = (*s >> 24) * 1.0f / 255;
			d += 4;
			s++;
		}
	}
}

void S3DAPI_API ConvertImageFrom_BCx_To_A8R8G8B8_TYPELESS(void* __restrict srcBuffer, void* __restrict dstBuffer, DWORD width, DWORD height, UINT rowPitch, int flag)
{	
	DWORD line = height / 4 + (height % 4 > 0 ? 1 : 0);
	byte* d = (byte*)dstBuffer;
	for(DWORD j = 0; j< line; j++)
	{
		byte* s = (byte*)((char*)srcBuffer + rowPitch * j);
		squish::DecompressImage( (squish::u8*)d, width, 4, (squish::u8*)s, flag );
		d += 4 * width * 4;
	}
}

void S3DAPI_API ConvertImageFrom_8_BYTE_To_A8R8G8B8_TYPELESS(void* __restrict srcBuffer, void* __restrict dstBuffer, DWORD width, DWORD height, UINT rowPitch)
{
	byte* d = (byte*)dstBuffer;
	for(DWORD j = 0; j < height; j++)
	{
		byte* s = (byte*)((char*)srcBuffer + rowPitch * j);
		for(DWORD i = 0; i < width; i++)
		{
			d[0] = d[1] = d[2] = *s;
			d[3] = 255;
			d += 4;
			s++;
		}
	}
}

void S3DAPI_API ConvertImageFrom_R8G8_UNORM_To_A8R8G8B8_TYPELESS(void* __restrict srcBuffer, void* __restrict dstBuffer, DWORD width, DWORD height, UINT rowPitch)
{
	byte* d = (byte*)dstBuffer;
	for(DWORD j = 0; j< height; j++)
	{
		byte* s = (byte*)((char*)srcBuffer + rowPitch * j);
		for(DWORD i = 0; i< width; i++)
		{
			d[0] = s[0];
			d[1] = s[1];
			d[2] = 0;
			d[3] = 255;
			d += 4;
			s += 2;
		}
	}
}

void S3DAPI_API ConvertImageFrom_B8G8R8A8_UNORM_To_A8R8G8B8_TYPELESS(void* __restrict srcBuffer, void* __restrict dstBuffer, DWORD width, DWORD height, UINT rowPitch)
{
	byte* d = (byte*)dstBuffer;
	for(DWORD j = 0; j< height; j++)
	{
		byte* s = (byte*)((char*)srcBuffer + rowPitch * j);
		for(DWORD i = 0; i< width; i++)
		{
			d[0] = s[2];
			d[1] = s[1];
			d[2] = s[0];
			d[3] = s[3];
			d += 4;
			s += 4;
		}
	}
}

void S3DAPI_API ConvertImageFrom_R10G10B10A2_UNORM_To_A8R8G8B8_TYPELESS( void* __restrict srcBuffer, void* __restrict dstBuffer, DWORD width, DWORD height, UINT rowPitch )
{
#pragma pack(push, 1)
	struct UINT_R10G10B10A2_4
	{
		unsigned __int16 R : 10;
		unsigned __int16 G_L : 6;
		unsigned __int16 G_H : 4;
		unsigned __int16 B : 10;
		unsigned __int16 A : 2;
	};
#pragma pack(pop)
	C_ASSERT(sizeof(UINT_R10G10B10A2_4) == 4);
	byte* d = (byte*)dstBuffer;
	for(DWORD j = 0; j< height; j++)
	{
		UINT_R10G10B10A2_4* s = (UINT_R10G10B10A2_4*)((char*)srcBuffer + rowPitch * j);
		for(DWORD i = 0; i< width; i++)
		{
			d[0] = s->R >> 2;
			d[1] = ((s->G_H << 6) | s->G_L) >> 2;
			d[2] = s->B >> 2;
			d[3] = (byte)(s->A / 3.0f * 255);
			d += 4;
			s ++;
		}
	}
}

inline bool isinf(float val)
{
	return ((*(DWORD*)&val) & 0x7f800000) == 0x7f800000;
}

void FindMinMaxLum_R32_FLOAT( void* srcBuffer, DWORD width, DWORD height, UINT rowPitch, float &minLum, float &maxLum )
{
	maxLum = FLT_MIN;
	minLum = FLT_MAX;
	for(DWORD j = 0; j< height; j++)
	{
		float* s = (float*)((char*)srcBuffer + rowPitch * j);
		for(DWORD i = 0; i< width; i++)
		{
			float lum = s[0];
			if (!isinf(lum))
			{
				if(lum > maxLum)
					maxLum = lum;
				if(lum < minLum)
					minLum = lum;
			}
			s++;
		}	
	}
	DEBUG_MESSAGE(_T("ConvertImage: min = %f max = %f\n"), minLum, maxLum);
}

static byte powTable[512] = { 255 };

void InitPowTable()
{
	if (powTable[0] != 255)
		return;

	for (int i = 0; i < 512; i++)
		powTable[i] = byte(pow(i / 511.0f, 1.0f / 2.2f) * 255);
}

void S3DAPI_API ConvertImageFrom_R32_FLOAT_To_A8R8G8B8_TYPELESS(void* __restrict srcBuffer, void* __restrict dstBuffer, DWORD width, DWORD height, UINT rowPitch, UINT dstPitch)
{
	float minLum;
	float maxLum;
	FindMinMaxLum_R32_FLOAT(srcBuffer, width, height, rowPitch, minLum, maxLum);

	InitPowTable();
	float* s = (float*)srcBuffer;
	byte* d = (byte*)dstBuffer;
	float rLum = ((maxLum - minLum) <= 0) ? 0.1f :  511.f / (maxLum - minLum);
	// TODO: Support dstPitch
	_ASSERT(dstPitch == width * 4);
#if 0 // C version of code, only for debug
	for (DWORD i = 0; i < width * height; i++)
	{
		float f = (s[0] - minLum) * rLum;
		UINT ind = (UINT)(f + 0.5f);
		ind = clamp<UINT>(ind, 0, 511);
		byte c = powTable[ind];
		/* Devil lib R */ d[0] = c; /* Dx10 R */
		/* Devil lib G */ d[1] = c; /* Dx10 G */
		/* Devil lib B */ d[2] = c; /* Dx10 B */
		/* Devil lib A */ d[3] = 255;
		s++;
		d += 4;
	}
#else
	__m128 minLumX = _mm_set1_ps(minLum);
	__m128 rLumX = _mm_set1_ps(rLum);
	__m128 valX = _mm_set1_ps(0.5f);
	__m128 minX = _mm_set1_ps(0.0f);
	__m128 maxX = _mm_set1_ps(511.0f);
	DWORD length = width * height;
	DWORD length4 = length / 4;
	for (DWORD i = 0; i < length4; i++)
	{
		__m128i temp = _mm_load_si128((__m128i *)s);
		__m128 t = _mm_castsi128_ps(temp);
		t = _mm_sub_ps(t, minLumX);
		t = _mm_mul_ps(t, rLumX);
		t = _mm_add_ps(t, valX);
		t = _mm_max_ps(t, minX);
		t = _mm_min_ps(t, maxX);
		__m128i indices = _mm_cvttps_epi32(t);
		byte c1 = powTable[indices.m128i_i32[0]];
		((DWORD*)d)[0] = MAKEFOURCC(c1, c1, c1, 255);
		byte c2 = powTable[indices.m128i_i32[1]];
		((DWORD*)d)[1] = MAKEFOURCC(c2, c2, c2, 255);
		byte c3 = powTable[indices.m128i_i32[2]];
		((DWORD*)d)[2] = MAKEFOURCC(c3, c3, c3, 255);
		byte c4 = powTable[indices.m128i_i32[3]];
		((DWORD*)d)[3] = MAKEFOURCC(c4, c4, c4, 255);
		s += 4;
		d += 4 * 4;
	}
	for (DWORD i = 0; i < length - (length4 * 4); i++)
	{
		float f = (s[0] - minLum) * rLum;
		UINT ind = (UINT)(f + 0.5f);
		ind = clamp<UINT>(ind, 0, 511);
		byte c = powTable[ind];
		/* Devil lib R */ d[0] = c;
		/* Devil lib G */ d[1] = c;
		/* Devil lib B */ d[2] = c;
		/* Devil lib A */ d[3] = 255;
		s++;
		d += 4;
	}
#endif
}

void FindMinMuxLum_R32G32B32A32_FLOAT( void* srcBuffer, DWORD dataSize, float &maxLum, float &minLum, float &maxA, float &minA )
{
	maxLum = FLT_MIN;
	minLum = FLT_MAX;
	maxA = FLT_MIN;
	minA = FLT_MAX;
	float* s = (float*)srcBuffer;
	for (DWORD i = 0; i < dataSize; i++)
	{
		float lum = s[0] * 0.2989f + s[1] * 0.5870f + s[2] * 0.1140f;
		if (!isinf(lum))
		{
			if(lum > maxLum)
				maxLum = lum;
			if(lum < minLum)
				minLum = lum;
		}
		if (!isinf(s[3]))
		{
			if(s[3] > maxA)
				maxA = s[3];
			if(s[3] < minA)
				minA = s[3];
		}
		s += 4;
	}	
	DEBUG_MESSAGE(_T("ConvertImage: min = %f max = %f, minA = %f maxA = %f\n"), minLum, maxLum, minA, maxA);
}

void S3DAPI_API ConvertImageFrom_R32G32B32A32_FLOAT_To_A8R8G8B8_TYPELESS(void* __restrict srcBuffer, void* __restrict dstBuffer, DWORD dataSize)
{
	float maxLum;
	float minLum;
	float maxA;
	float minA;
	FindMinMuxLum_R32G32B32A32_FLOAT(srcBuffer, dataSize, maxLum, minLum, maxA, minA);

	InitPowTable();
	float* s = (float*)srcBuffer;
	byte* d = (byte*)dstBuffer;
	float rLum = ((maxLum - minLum) <= 0) ? 0.1f :  511.f / (maxLum - minLum);
#if 0 // C version of code, only for debug
	for (DWORD i = 0; i < dataSize; i++)
	{
		float f = (s[0] - minLum) * rLum;
		UINT ind = (UINT)(f + 0.5f);
		ind = clamp<UINT>(ind, 0, 511);
		/* Devil lib R */ d[0] = powTable[ind]; /* Dx10 R */
		f = (s[1] - minLum) * rLum;
		ind = (UINT)(f + 0.5f);
		ind = clamp<UINT>(ind, 0, 511);
		/* Devil lib G */ d[1] = powTable[ind]; /* Dx10 G */
		f = (s[2] - minLum) * rLum;
		ind = (UINT)(f + 0.5f);
		ind = clamp<UINT>(ind, 0, 511);
		/* Devil lib B */ d[2] = powTable[ind]; /* Dx10 B */
		/* Devil lib A */ d[3] = (byte)(s[3] * 255);
		s += 4;
		d += 4;
	}
#else
	__m128 minLumX = _mm_setr_ps(minLum, minLum, minLum, 0.0f);
	__m128 rLumX = _mm_setr_ps(rLum, rLum, rLum, 255.0f);
	__m128 valX = _mm_set1_ps(0.5f);
	__m128 minX = _mm_set1_ps(0.0f);
	__m128 maxX = _mm_setr_ps(511.0f, 511.0f, 511.0f, 255.0f);
	for (DWORD i = 0; i < dataSize; i++)
	{
		__m128i temp = _mm_load_si128((__m128i *)s);
		__m128 t = _mm_castsi128_ps(temp);
		t = _mm_sub_ps(t, minLumX);
		t = _mm_mul_ps(t, rLumX);
		t = _mm_add_ps(t, valX);
		t = _mm_max_ps(t, minX);
		t = _mm_min_ps(t, maxX);
		__m128i indices = _mm_cvttps_epi32(t);
		byte c1 = powTable[indices.m128i_i32[0]]; /* Devil lib R */
		byte c2 = powTable[indices.m128i_i32[1]]; /* Devil lib G */
		byte c3 = powTable[indices.m128i_i32[2]]; /* Devil lib B */
		byte c4 = (byte)(indices.m128i_i32[3]); /* Devil lib A */
		((DWORD*)d)[0] = MAKEFOURCC(c1, c2, c3, c4);
		s += 4;
		d += 4;
	}
#endif
}

