#include <Windows.h>
#include <fstream>
#include <vector>
#include <map>
#include <d3dx9.h>
#include <dxerr.h>
#include <atlcomcli.h>
#include "IStereoAPIInternal.h"

#pragma warning(disable:4996)

#ifdef _DEBUG
#define V_(x)			{ hr = (x); _ASSERT_EXPR(SUCCEEDED(hr), DXGetErrorString(hr)); }
#define V_RETURN_(x)    { hr = (x); if( FAILED(hr) ) { _ASSERT_EXPR(SUCCEEDED(hr), DXGetErrorString(hr)); return hr; } }
#else
#define V_(x)			{ hr = (x); }
#define V_RETURN_(x)    { hr = (x); if( FAILED(hr) ) { return hr; } }
#endif

extern LPDIRECT3D9             g_pD3D;
extern LPDIRECT3DDEVICE9       device;
extern D3DPRESENT_PARAMETERS   dpp;
extern IDirect3DStateBlock9*   defaultState;
extern CComQIPtr<IStereoAPIInternal2> g_pAPI2;

#define FOURCC_DF16 ((D3DFORMAT) MAKEFOURCC( 'D', 'F', '1', '6' ))
#define FOURCC_DF24 ((D3DFORMAT) MAKEFOURCC( 'D', 'F', '2', '4' ))
#define FOURCC_RAWZ ((D3DFORMAT) MAKEFOURCC( 'R', 'A', 'W', 'Z' ))
#define FOURCC_INTZ ((D3DFORMAT) MAKEFOURCC( 'I', 'N', 'T', 'Z' ))
#define FOURCC_NVCS ((D3DFORMAT) MAKEFOURCC( 'N', 'V', 'C', 'S' ))
#define FOURCC_ATI1 ((D3DFORMAT) MAKEFOURCC( 'A', 'T', 'I', '1' ))
#define FOURCC_ATI2 ((D3DFORMAT) MAKEFOURCC( 'A', 'T', 'I', '2' ))
#define FOURCC_NULL ((D3DFORMAT) MAKEFOURCC( 'N', 'U', 'L', 'L' ))

class Poses : public std::vector<UINT64>
{
public:
	Poses(UINT pos,...)
	{
		va_list args;
		va_start(args,pos);
		while(pos)
		{
			push_back(pos);
			pos = va_arg(args,UINT);
		}
		va_end(args);
	}
};

char *GetFromFile(UINT64 pos,DWORD *pLen = 0,DWORD *pitch1 = 0,DWORD *pitch2 = 0);
