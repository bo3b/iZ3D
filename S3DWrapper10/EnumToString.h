#pragma once

#include <windows.h>
#include <d3d10.h>
#include <dxgiddi.h>
#include "Commands\Command.h"

#define UNPACK_VALUE(VALUE) case VALUE: return #VALUE;

static const char* EnumToString( SHADER_PIPELINE nVal )
{
	switch (nVal)
	{
		UNPACK_VALUE(SP_NONE);
		UNPACK_VALUE(SP_VS);
		UNPACK_VALUE(SP_GS);
		UNPACK_VALUE(SP_DS);
		UNPACK_VALUE(SP_HS);
		UNPACK_VALUE(SP_PS);
		UNPACK_VALUE(SP_CS);
		UNPACK_VALUE(SP_COUNT);
	}

	assert(!"Invalid shader pipeline value");
	return "!!!INVALID!!!";
}

static const char* EnumToString( TextureType nVal )
{
	static const char *enum_name[] = {	
		"TT_MONO",
		"TT_STEREO",
		"TT_CLEARED",
		"TT_SETLIKESRC"
	};
	return enum_name[nVal];
}

static const char* EnumToString( VIEWINDEX nVal )
{
	static const char *enum_name[] = {	
		"VIEWINDEX_LEFT",
		"VIEWINDEX_RIGHT",
		"VIEWINDEX_MONO"
	};
	return enum_name[nVal];
}

static const char* EnumToString( DXGI_DDI_MODE_SCANLINE_ORDER nVal )
{
	static const char *enum_name[] = {	
		"DXGI_DDI_MODE_SCANLINE_ORDER_UNSPECIFIED",
		"DXGI_DDI_MODE_SCANLINE_ORDER_PROGRESSIVE",
		"DXGI_DDI_MODE_SCANLINE_ORDER_UPPER_FIELD_FIRST",
		"DXGI_DDI_MODE_SCANLINE_ORDER_LOWER_FIELD_FIRST"
	};
	return enum_name[nVal];
}

static const char* EnumToString( DXGI_DDI_MODE_ROTATION nVal  )
{
	static const char *enum_name[] = {	
		"DXGI_DDI_MODE_ROTATION_UNSPECIFIED",
		"DXGI_DDI_MODE_ROTATION_IDENTITY",
		"DXGI_DDI_MODE_ROTATION_ROTATE90",
		"DXGI_DDI_MODE_ROTATION_ROTATE180",
		"DXGI_DDI_MODE_ROTATION_ROTATE270"
	};
	return enum_name[nVal];
}

static const char* EnumToString( DXGI_DDI_MODE_SCALING nVal  )
{
	static const char *enum_name[] = {	
		"DXGI_DDI_MODE_SCALING_UNSPECIFIED",
		"DXGI_DDI_MODE_SCALING_STRETCHED",
		"DXGI_DDI_MODE_SCALING_CENTERED"
	};
	return enum_name[nVal];
}


static const char* EnumToString( DXGI_MODE_SCALING nVal  )
{
	static const char *enum_name[] = {	
		"DXGI_MODE_SCALING_UNSPECIFIED",
		"DXGI_MODE_SCALING_STRETCHED",
		"DXGI_MODE_SCALING_CENTERED"
	};
	return enum_name[nVal];
}

static const char* EnumToString( DXGI_MODE_SCANLINE_ORDER nVal  )
{
	static const char *enum_name[] = {	
		"DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED",
		"DXGI_MODE_SCANLINE_ORDER_PROGRESSIVE",
		"DXGI_MODE_SCANLINE_ORDER_UPPER_FIELD_FIRST",
		"DXGI_MODE_SCANLINE_ORDER_LOWER_FIELD_FIRST"
	};
	return enum_name[nVal];
}

static const char* EnumToString( DXGI_SWAP_EFFECT nVal  )
{
	static const char *enum_name[] = {	
		"DXGI_SWAP_EFFECT_DISCARD",
		"DXGI_SWAP_EFFECT_SEQUENTIAL"
	};
	return enum_name[nVal];
}

static const char* EnumToString( D3D10DDI_SHADERUNITTYPE nVal  )
{
	static const char *enum_name[] = {	
		"D3D10DDISHADERUNITTYPE_UNDEFINED",
		"D3D10DDISHADERUNITTYPE_GEOMETRY",
		"D3D10DDISHADERUNITTYPE_VERTEX",
		"D3D10DDISHADERUNITTYPE_PIXEL",
		"D3D11DDISHADERUNITTYPE_HULL",
		"D3D11DDISHADERUNITTYPE_DOMAIN",
		"D3D11DDISHADERUNITTYPE_COMPUTE"
	};
	return enum_name[nVal];
}

static const char* EnumToString( D3D10_DDI_RESOURCE_USAGE nVal  )
{
	static const char *enum_name[] = {	
		"D3D10_DDI_USAGE_DEFAULT",
		"D3D10_DDI_USAGE_IMMUTABLE",
		"D3D10_DDI_USAGE_DYNAMIC",
		"D3D10_DDI_USAGE_STAGING"
	};
	return enum_name[nVal];
}

static const char* EnumToString( D3D10_DDI_MAP nVal  )
{
	static const char *enum_name[] = {	"none",		// enums are 1-based!
			"D3D10_DDI_MAP_READ",
			"D3D10_DDI_MAP_WRITE",
			"D3D10_DDI_MAP_READWRITE",
			"D3D10_DDI_MAP_WRITE_DISCARD",
			"D3D10_DDI_MAP_WRITE_NOOVERWRITE",
	};
	return enum_name[nVal];
}

static const char* EnumToString( D3D10_DDI_BLEND nVal  )
{
	static const char *enum_name[] = {	"",		// enums are 1-based!
		"D3D10_DDI_BLEND_ZERO",
		"D3D10_DDI_BLEND_ONE",
		"D3D10_DDI_BLEND_SRC_COLOR",
		"D3D10_DDI_BLEND_INV_SRC_COLOR",
		"D3D10_DDI_BLEND_SRC_ALPHA",
		"D3D10_DDI_BLEND_INV_SRC_ALPHA",
		"D3D10_DDI_BLEND_DEST_ALPHA",
		"D3D10_DDI_BLEND_INV_DEST_ALPHA",
		"D3D10_DDI_BLEND_DEST_COLOR",
		"D3D10_DDI_BLEND_INV_DEST_COLOR",
		"D3D10_DDI_BLEND_SRC_ALPHASAT",
		"D3D10_DDI_BLEND_BLEND_FACTOR",
		"D3D10_DDI_BLEND_INVBLEND_FACTOR",
		"D3D10_DDI_BLEND_SRC1_COLOR",
		"D3D10_DDI_BLEND_INV_SRC1_COLOR",
		"D3D10_DDI_BLEND_SRC1_ALPHA",
		"D3D10_DDI_BLEND_INV_SRC1_ALPHA",
	};	
	return enum_name[nVal];
}

static const char* EnumToString( D3D10_DDI_BLEND_OP nVal  )
{
	static const char *enum_name[] = {	"",		// enums are 1-based!
		"D3D10_DDI_BLEND_OP_ADD",
		"D3D10_DDI_BLEND_OP_SUBTRACT",
		"D3D10_DDI_BLEND_OP_REV_SUBTRACT",
		"D3D10_DDI_BLEND_OP_MIN",
		"D3D10_DDI_BLEND_OP_MAX"
	};	
	return enum_name[nVal];
}

static const char* EnumToString( D3D10_DDI_COMPARISON_FUNC nVal  )
{
	static const char *enum_name[] = {	"",		// enums are 1-based!
		"D3D10_DDI_COMPARISON_NEVER",
		"D3D10_DDI_COMPARISON_LESS",
		"D3D10_DDI_COMPARISON_EQUAL",
		"D3D10_DDI_COMPARISON_LESS_EQUAL",
		"D3D10_DDI_COMPARISON_GREATER",
		"D3D10_DDI_COMPARISON_NOT_EQUAL",
		"D3D10_DDI_COMPARISON_GREATER_EQUAL",
		"D3D10_DDI_COMPARISON_ALWAYS"
	};
	return enum_name[nVal];
}

static const char* EnumToString( D3D10_DDI_PRIMITIVE_TOPOLOGY nVal  )
{
	static const char *enum_name[] = 
	{
		"D3D10_DDI_PRIMITIVE_TOPOLOGY_UNDEFINED",
		"D3D10_DDI_PRIMITIVE_TOPOLOGY_POINTLIST",
		"D3D10_DDI_PRIMITIVE_TOPOLOGY_LINELIST",
		"D3D10_DDI_PRIMITIVE_TOPOLOGY_LINESTRIP",
		"D3D10_DDI_PRIMITIVE_TOPOLOGY_TRIANGLELIST",
		"D3D10_DDI_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP",
		"D3D10_DDI_PRIMITIVE_TOPOLOGY_LINELIST_ADJ",
		"D3D10_DDI_PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ",
		"D3D10_DDI_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ",
		"D3D10_DDI_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ"
	};
	return enum_name[(UINT)nVal];
}

static const char* EnumToString( D3D10_DDI_STENCIL_OP nVal  )
{
	static const char *enum_name[] = 
	{
		"",
		"D3D10_DDI_STENCIL_OP_KEEP",
		"D3D10_DDI_STENCIL_OP_ZERO",
		"D3D10_DDI_STENCIL_OP_REPLACE",
		"D3D10_DDI_STENCIL_OP_INCR_SAT",
		"D3D10_DDI_STENCIL_OP_DECR_SAT",
		"D3D10_DDI_STENCIL_OP_INVERT",
		"D3D10_DDI_STENCIL_OP_INCR",
		"D3D10_DDI_STENCIL_OP_DECR",
	};

	return enum_name[nVal];
}

static const char* EnumToString( D3D10_DDI_FILTER nVal  )
{
	switch (nVal) 
	{
		UNPACK_VALUE(D3D10_FILTER_MIN_MAG_MIP_POINT);
		UNPACK_VALUE(D3D10_FILTER_MIN_MAG_POINT_MIP_LINEAR);
		UNPACK_VALUE(D3D10_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT);
		UNPACK_VALUE(D3D10_FILTER_MIN_POINT_MAG_MIP_LINEAR);
		UNPACK_VALUE(D3D10_FILTER_MIN_LINEAR_MAG_MIP_POINT);
		UNPACK_VALUE(D3D10_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR);
		UNPACK_VALUE(D3D10_FILTER_MIN_MAG_LINEAR_MIP_POINT);
		UNPACK_VALUE(D3D10_FILTER_MIN_MAG_MIP_LINEAR);
		UNPACK_VALUE(D3D10_FILTER_ANISOTROPIC);
		UNPACK_VALUE(D3D10_FILTER_COMPARISON_MIN_MAG_MIP_POINT);
		UNPACK_VALUE(D3D10_FILTER_COMPARISON_MIN_MAG_POINT_MIP_LINEAR);
		UNPACK_VALUE(D3D10_FILTER_COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT);
		UNPACK_VALUE(D3D10_FILTER_COMPARISON_MIN_POINT_MAG_MIP_LINEAR);
		UNPACK_VALUE(D3D10_FILTER_COMPARISON_MIN_LINEAR_MAG_MIP_POINT);
		UNPACK_VALUE(D3D10_FILTER_COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR);
		UNPACK_VALUE(D3D10_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT);
		UNPACK_VALUE(D3D10_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR);
		UNPACK_VALUE(D3D10_FILTER_COMPARISON_ANISOTROPIC);
		UNPACK_VALUE(D3D10_FILTER_TEXT_1BIT);
	}
	return "UNKNOWN";
}

static const char * EnumToString( SecondWindowMessage windowMessage )
{
	switch (windowMessage)
	{
		UNPACK_VALUE(SWM_MOVE_SECOND_WINDOW);
		UNPACK_VALUE(SWM_SHOW_SECOND_WINDOW);
		UNPACK_VALUE(SWM_HIDE_SECOND_WINDOW);
		UNPACK_VALUE(SWM_MOVE_APP_WINDOW);
		UNPACK_VALUE(SWM_RESTORE_APP_WINDOW);
	};
	return "UNKNOWN";
}

static const char* EnumToString( D3D10_DDI_TEXTURE_ADDRESS_MODE nVal  )
{
	static const char *enum_name[] = 
	{
		"",
		"D3D10_TEXTURE_ADDRESS_WRAP",
		"D3D10_TEXTURE_ADDRESS_MIRROR",
		"D3D10_TEXTURE_ADDRESS_CLAMP",
		"D3D10_TEXTURE_ADDRESS_BORDER",
		"D3D10_TEXTURE_ADDRESS_MIRROR_ONCE",
	};

	return enum_name[nVal];
}

static const char* EnumToString( D3D10DDIRESOURCE_TYPE nVal  )
{
	static const char *enum_name[] = 
	{
		"",
		"D3D10DDIRESOURCE_BUFFER",
		"D3D10DDIRESOURCE_TEXTURE1D",
		"D3D10DDIRESOURCE_TEXTURE2D",
		"D3D10DDIRESOURCE_TEXTURE3D",
		"D3D10DDIRESOURCE_TEXTURECUBE",
		"D3D11DDIRESOURCE_BUFFEREX",
	};

	return enum_name[nVal];
}

static const char* EnumToString( D3D10_DRIVER_TYPE nVal  )
{
	static const char *enum_name[] = 
	{
		"D3D10_DRIVER_TYPE_HARDWARE",
		"D3D10_DRIVER_TYPE_REFERENCE",
		"D3D10_DRIVER_TYPE_NULL",
		"D3D10_DRIVER_TYPE_SOFTWARE",
		"D3D10_DRIVER_TYPE_WARP",
	};
	return enum_name[nVal];
}

static const char* EnumToString( D3D10_SB_NAME nVal  )
{
	static const char *enum_name[] = 
	{
		"D3D10_SB_NAME_UNDEFINED",
		"D3D10_SB_NAME_POSITION",
		"D3D10_SB_NAME_CLIP_DISTANCE",
		"D3D10_SB_NAME_CULL_DISTANCE",
		"D3D10_SB_NAME_RENDER_TARGET_ARRAY_INDEX",
		"D3D10_SB_NAME_VIEWPORT_ARRAY_INDEX",
		"D3D10_SB_NAME_VERTEX_ID",
		"D3D10_SB_NAME_PRIMITIVE_ID",
		"D3D10_SB_NAME_INSTANCE_ID",
		"D3D10_SB_NAME_IS_FRONT_FACE",
		"D3D10_SB_NAME_SAMPLE_INDEX"
	};
	return enum_name[nVal];
}

static const char* EnumToString( DXGI_DDI_FLIP_INTERVAL_TYPE nVal  )
{
	static const char *enum_name[] = 
	{
		"DXGI_DDI_FLIP_INTERVAL_IMMEDIATE",
		"DXGI_DDI_FLIP_INTERVAL_ONE",
		"DXGI_DDI_FLIP_INTERVAL_TWO",
		"DXGI_DDI_FLIP_INTERVAL_THREE",
		"DXGI_DDI_FLIP_INTERVAL_FOUR"
	};

	return enum_name[nVal];
}

static const char* EnumToString( DXGI_DDI_RESIDENCY nVal  )
{
	static const char *enum_name[] = 
	{
		"",
		"DXGI_DDI_RESIDENCY_FULLY_RESIDENT",
		"DXGI_DDI_RESIDENCY_RESIDENT_IN_SHARED_MEMORY",
		"DXGI_DDI_RESIDENCY_EVICTED_TO_DISK",
	};

	return enum_name[nVal];
}

static const char* EnumToString( D3D11DDI_HANDLETYPE nVal  )
{
	switch (nVal) 
	{
		UNPACK_VALUE(D3D10DDI_HT_RESOURCE);
		UNPACK_VALUE(D3D10DDI_HT_SHADERRESOURCEVIEW);
		UNPACK_VALUE(D3D10DDI_HT_RENDERTARGETVIEW);
		UNPACK_VALUE(D3D10DDI_HT_DEPTHSTENCILVIEW);
		UNPACK_VALUE(D3D10DDI_HT_SHADER);
		UNPACK_VALUE(D3D10DDI_HT_ELEMENTLAYOUT);
		UNPACK_VALUE(D3D10DDI_HT_BLENDSTATE);
		UNPACK_VALUE(D3D10DDI_HT_DEPTHSTENCILSTATE);
		UNPACK_VALUE(D3D10DDI_HT_RASTERIZERSTATE);
		UNPACK_VALUE(D3D10DDI_HT_SAMPLERSTATE);
		UNPACK_VALUE(D3D10DDI_HT_QUERY);
		UNPACK_VALUE(D3D11DDI_HT_COMMANDLIST);
		UNPACK_VALUE(D3D11DDI_HT_UNORDEREDACCESSVIEW);
	}
	return "UNKNOWN";
}

const char* EnumToString( DXGI_FORMAT nVal  );

enum CommandsId;
const char* CommandIDToString(CommandsId id);

#undef UNPACK_VALUE

const TCHAR *GetDXGIModeString(const DXGI_MODE_DESC *pMode);
const TCHAR *GetDXGISwapChainDescString(const DXGI_SWAP_CHAIN_DESC *pDesc);
const TCHAR *GetDXGIBltArgString(const DXGI_DDI_ARG_BLT *pDesc);
const TCHAR *GetBoxString(CONST D3D10_BOX* pBox);
const TCHAR *GetBoxString(CONST D3D10_DDI_BOX* pBox);

const TCHAR *GetMatrixString(CONST D3DMATRIX *mat, bool bTranspose);
inline void TraceMatrix(CONST D3DMATRIX * mat, bool bTranspose = false) { DEBUG_TRACE3(_T("%s"), GetMatrixString(mat, bTranspose)); }
const TCHAR *GetFlipIntervalString( DXGI_DDI_FLIP_INTERVAL_TYPE Interval );