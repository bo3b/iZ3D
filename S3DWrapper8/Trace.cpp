/* IZ3D_FILE: $Id$ 
*
* Project : iZ3D Stereo Driver
* Copyright (C) iZ3D Inc. 2002 - 2010
*
* $Author$
* $Revision$
* $Date$
* $LastChangedBy$
* $URL$
*/
#include "stdafx.h"
#include "d3d8shader.h"

#define NSBUFFERLENGTH 4096

void PrintMatrix( CONST D3DMATRIX * mat ) {
	DEBUG_MESSAGE( "%12.7f %12.7f %12.7f %12.7f\n"
		"%12.7f %12.7f %12.7f %12.7f\n"
		"%12.7f %12.7f %12.7f %12.7f\n"
		"%12.7f %12.7f %12.7f %12.7f\n\n",
		mat->_11, mat->_12, mat->_13, mat->_14,
		mat->_21, mat->_22, mat->_23, mat->_24,
		mat->_31, mat->_32, mat->_33, mat->_34,
		mat->_41, mat->_42, mat->_43, mat->_44 );
}

const char *GetResultString( HRESULT hr ) 
{
	return DXGetErrorStringA( hr );
}

const char *GetPrimitiveTypeString( D3DPRIMITIVETYPE PrimitiveType ) 
{
	if( PrimitiveType == 1)
		return "POINTLIST";
	else if( PrimitiveType == 2)
		return "LINELIST";
	else if( PrimitiveType == 3)
		return "LINESTRIP";
	else if( PrimitiveType == 4)
		return "TRIANGLELIST";
	else if( PrimitiveType == 5)
		return "TRIANGLESTRIP";
	else if( PrimitiveType == 6)
		return "TRIANGLEFAN";
	else
		return "Wrong type";
}

const char *GetPoolString( DWORD Pool ) {
	if( Pool == D3DPOOL_DEFAULT ) 
		return "DEFAULT";
	else if( Pool == D3DPOOL_MANAGED ) 
		return "MANAGED";
	else if( Pool == D3DPOOL_SYSTEMMEM ) 
		return "SYSTEMMEMM";
	else if( Pool == D3DPOOL_SCRATCH ) 
		return "SCRATCH";
	else
		return"Wrong Pool";
}

const char *GetUsageString( DWORD Usage ) {
	static char res_str[NSBUFFERLENGTH];
	res_str[0] = '\0';

	if (!Usage) return "0";

	if ( Usage & D3DUSAGE_RENDERTARGET ) 
		strcat( res_str, "RENDERTARGET|" );
	if ( Usage & D3DUSAGE_DYNAMIC ) 
		strcat( res_str, "DYNAMIC|" );
	if ( Usage & D3DUSAGE_RTPATCHES ) 
		strcat( res_str, "RTPATCHES|" );
	if ( Usage & D3DUSAGE_DEPTHSTENCIL ) 
		strcat( res_str, "DEPTHSTENCIL|" );
	if ( Usage & D3DUSAGE_DONOTCLIP ) 
		strcat( res_str, "DONOTCLIP|" );
	if ( Usage & D3DUSAGE_POINTS ) 
		strcat( res_str, "POINTS|" );
	if ( Usage & D3DUSAGE_NPATCHES ) 
		strcat( res_str, "NPATCHES|" );
	if ( Usage & D3DUSAGE_SOFTWAREPROCESSING ) 
		strcat( res_str, "SOFTWAREPROCESSING|" );
	if ( Usage & D3DUSAGE_WRITEONLY ) 
		strcat( res_str, "WRITEONLY|" );
	return res_str;
}

const CHAR *GetFormatString( D3DFORMAT format ) 
{
	switch ( format ) 
	{
	case D3DFMT_R8G8B8: return ("R8G8B8");
	case D3DFMT_A8R8G8B8: return ("A8R8G8B8");
	case D3DFMT_X8R8G8B8: return ("X8R8G8B8");
	case D3DFMT_R3G3B2: return ("R3G3B2");
	case D3DFMT_R5G6B5: return ("R5G6B5");
	case D3DFMT_A1R5G5B5: return ("A1R5G5B5");
	case D3DFMT_A4R4G4B4: return ("A4R4G4B4");
	case D3DFMT_A8: return ("A8");
	case D3DFMT_A8R3G3B2: return ("A8R3G3B2");
	case D3DFMT_X4R4G4B4: return ("X4R4G4B4");
	case D3DFMT_A2B10G10R10: return ("A2B10G10R10");
	case D3DFMT_DXT1: return ("DXT1");
	case D3DFMT_DXT2: return ("DXT2");
	case D3DFMT_DXT3: return ("DXT3");
	case D3DFMT_DXT4: return ("DXT4");
	case D3DFMT_DXT5: return ("DXT5");
	case D3DFMT_FORCE_DWORD: return ("FORCE_DWORD");
	case D3DFMT_UNKNOWN: return ("UNKNOWN");
	case D3DFMT_UYVY: return ("UYVY");
	case D3DFMT_YUY2: return ("YUY2");
	case D3DFMT_X1R5G5B5: return ("X1R5G5B5");
	case D3DFMT_G16R16: return ("G16R16");
	case D3DFMT_A8P8: return ("A8P8");
	case D3DFMT_A8L8: return ("A8L8");
	case D3DFMT_A4L4: return ("A4L4");
	case D3DFMT_V8U8: return ("V8U8");
	case D3DFMT_L6V5U5: return ("L6V5U5");
	case D3DFMT_X8L8V8U8: return ("X8L8V8U8");
	case D3DFMT_Q8W8V8U8: return ("Q8W8V8U8");
	case D3DFMT_V16U16: return ("V16U16");
		//  case 65 /*D3DFMT_W11V11U10*/: return "W11V11U10";
	case D3DFMT_A2W10V10U10: return ("A2W10V10U10");
	case D3DFMT_D32: return ("D32");
	case D3DFMT_D15S1: return ("D15S1");
	case D3DFMT_D24S8: return ("D24S8");
	case D3DFMT_D24X8: return ("D24X8");
	case D3DFMT_D24X4S4: return ("D24X4S4");
	case D3DFMT_D16: return ("D16");
	case D3DFMT_D16_LOCKABLE: return ("D16_LOCKABLE");
	case D3DFMT_P8: return ("P8");
	case D3DFMT_L8: return ("L8");
	case D3DFMT_VERTEXDATA: return ("VERTEXDATA");
	case D3DFMT_INDEX16: return ("INDEX16");
	case D3DFMT_INDEX32: return ("INDEX32");
	}
	return ("UNDEFINED D3DFMT!");
}

#define UNPACK_VALUE(VALUE) case VALUE: return #VALUE;

const char * GetVSDEString( DWORD VertexRegister )
{
	switch(VertexRegister)
	{
		UNPACK_VALUE(D3DVSDE_POSITION);
		UNPACK_VALUE(D3DVSDE_BLENDWEIGHT);
		UNPACK_VALUE(D3DVSDE_BLENDINDICES);
		UNPACK_VALUE(D3DVSDE_NORMAL);
		UNPACK_VALUE(D3DVSDE_PSIZE);
		UNPACK_VALUE(D3DVSDE_DIFFUSE);
		UNPACK_VALUE(D3DVSDE_SPECULAR);
		UNPACK_VALUE(D3DVSDE_TEXCOORD0);
		UNPACK_VALUE(D3DVSDE_TEXCOORD1);
		UNPACK_VALUE(D3DVSDE_TEXCOORD2);
		UNPACK_VALUE(D3DVSDE_TEXCOORD3);
		UNPACK_VALUE(D3DVSDE_TEXCOORD4);
		UNPACK_VALUE(D3DVSDE_TEXCOORD5);
		UNPACK_VALUE(D3DVSDE_TEXCOORD6);
		UNPACK_VALUE(D3DVSDE_TEXCOORD7);
		UNPACK_VALUE(D3DVSDE_POSITION2);
		UNPACK_VALUE(D3DVSDE_NORMAL2);
	};
	return "UNKNOWN";
}

const char * GetVSDTString( DWORD Type )
{
	switch(Type)
	{
		UNPACK_VALUE(D3DVSDT_FLOAT1);
		UNPACK_VALUE(D3DVSDT_FLOAT2);
		UNPACK_VALUE(D3DVSDT_FLOAT3);
		UNPACK_VALUE(D3DVSDT_FLOAT4);
		UNPACK_VALUE(D3DVSDT_D3DCOLOR);
		UNPACK_VALUE(D3DVSDT_UBYTE4);
		UNPACK_VALUE(D3DVSDT_SHORT2);
		UNPACK_VALUE(D3DVSDT_SHORT4);
	};
	return "UNKNOWN";
}

const char * GetVEDTypeString( BYTE Type )
{
	switch(Type)
	{
		UNPACK_VALUE(D3DDECLTYPE_FLOAT1);
		UNPACK_VALUE(D3DDECLTYPE_FLOAT2);
		UNPACK_VALUE(D3DDECLTYPE_FLOAT3);
		UNPACK_VALUE(D3DDECLTYPE_FLOAT4);
		UNPACK_VALUE(D3DDECLTYPE_D3DCOLOR);
		UNPACK_VALUE(D3DDECLTYPE_UBYTE4);
		UNPACK_VALUE(D3DDECLTYPE_SHORT2);
		UNPACK_VALUE(D3DDECLTYPE_SHORT4);
	};
	return "New D3D9 Type";
}

const char * GetVEDMethodString( BYTE Method )
{
	switch(Method)
	{
		UNPACK_VALUE(D3DDECLMETHOD_DEFAULT);
		UNPACK_VALUE(D3DDECLMETHOD_PARTIALU);
		UNPACK_VALUE(D3DDECLMETHOD_PARTIALV);
		UNPACK_VALUE(D3DDECLMETHOD_CROSSUV);
		UNPACK_VALUE(D3DDECLMETHOD_UV);
		UNPACK_VALUE(D3DDECLMETHOD_LOOKUP);
		UNPACK_VALUE(D3DDECLMETHOD_LOOKUPPRESAMPLED);
	};
	return "UNKNOWN";
}

const char * GetVEDUsageString( BYTE Usage )
{
	switch(Usage)
	{
		UNPACK_VALUE(D3DDECLUSAGE_POSITION);
		UNPACK_VALUE(D3DDECLUSAGE_BLENDWEIGHT);
		UNPACK_VALUE(D3DDECLUSAGE_BLENDINDICES);
		UNPACK_VALUE(D3DDECLUSAGE_NORMAL);
		UNPACK_VALUE(D3DDECLUSAGE_PSIZE);
		UNPACK_VALUE(D3DDECLUSAGE_TEXCOORD);
		UNPACK_VALUE(D3DDECLUSAGE_TANGENT);
		UNPACK_VALUE(D3DDECLUSAGE_BINORMAL);
		UNPACK_VALUE(D3DDECLUSAGE_TESSFACTOR);
		UNPACK_VALUE(D3DDECLUSAGE_POSITIONT);
		UNPACK_VALUE(D3DDECLUSAGE_COLOR);
		UNPACK_VALUE(D3DDECLUSAGE_FOG);
		UNPACK_VALUE(D3DDECLUSAGE_DEPTH);
		UNPACK_VALUE(D3DDECLUSAGE_SAMPLE);
	};
	return "UNKNOWN";
}

const char *GetDisplayModeString(const D3DDISPLAYMODE *pMode)
{
	if (!pMode)
	{
		return "NULL";
	}
	static char szBuffer[NSBUFFERLENGTH];
	ZeroMemory(szBuffer, sizeof(szBuffer));
	_snprintf_s(szBuffer, _TRUNCATE, 
		"Format = %s, Width = %d, Height = %d, RefreshRate = %d", 
		GetFormatString(pMode->Format), pMode->Width, pMode->Height, pMode->RefreshRate);
	return szBuffer;
}

const char * GetRectString( CONST RECT * pRect )
{
	if (pRect == NULL)
	{
		return "NULL";
	}
	static char szBuffer[NSBUFFERLENGTH];
	_snprintf_s(szBuffer, _TRUNCATE, 
		"(%ld,%ld)-(%ld,%ld)", pRect->left, pRect->top, pRect->right, pRect->bottom);
	return szBuffer;
}

const char * GetLockedRectString( CONST D3DLOCKED_RECT * pLockedRect )
{
	static char szBuffer[NSBUFFERLENGTH];
	_snprintf_s(szBuffer, _TRUNCATE, 
		"(pBits = %p, Pitch = %d)", pLockedRect->pBits, pLockedRect->Pitch);
	return szBuffer;
}

const char *GetSurfaceDescString(CONST D3DSURFACE_DESC8 *pDesc)
{
	static char szBuffer[NSBUFFERLENGTH];	
	_snprintf_s(szBuffer, _TRUNCATE, 
		"(Format=%s, MultiSampleType=%d, Pool=%s, Type=%d, Usage=%d, Width=%d, Height=%d, Size=%d)", 
		GetFormatString(pDesc->Format), pDesc->MultiSampleType, GetPoolString(pDesc->Pool), 
		pDesc->Type, pDesc->Usage, pDesc->Width, pDesc->Height, pDesc->Size);
	return szBuffer;
}
