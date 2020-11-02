/* 
* Project : iZ3D Stereo Driver
* Copyright (C) iZ3D Inc. 2002 - 2010
*/

#pragma once

#include <d3dumddi.h>
#include "boost/unordered_map.hpp"
#include <hash_map>
#include <vector>

struct	_D3DDDIARG_OPENADAPTER;
extern	HRESULT (WINAPI *OpenAdapterNext)(_D3DDDIARG_OPENADAPTER*);
HRESULT	WINAPI OpenAdapterCallback(_D3DDDIARG_OPENADAPTER* pOpenData );
struct	_D3DKMT_CREATEDEVICE;
extern	HRESULT (APIENTRY *D3DKMTCreateDeviceNext)(_D3DKMT_CREATEDEVICE*);
HRESULT	APIENTRY D3DKMTCreateDeviceCallback(_D3DKMT_CREATEDEVICE* pData );
struct	_D3DKMT_PRESENT;
extern	HRESULT (APIENTRY *D3DKMTPresentNext)(CONST _D3DKMT_PRESENT*);
HRESULT	APIENTRY D3DKMTPresentCallback(CONST _D3DKMT_PRESENT* pData );

extern D3DKMT_HANDLE g_hKMAdapter;
extern D3DKMT_HANDLE g_hKMDevice;
extern	_D3DKMT_PRESENT g_KMPresentData;

class UMWrapper
{
public:
	UMWrapper(_D3DDDIARG_OPENADAPTER* pOpenData);
	~UMWrapper() {};

	//--- Hooked functions ---
	HRESULT GetCaps(CONST D3DDDIARG_GETCAPS* pData);
	HRESULT CreateDevice(D3DDDIARG_CREATEDEVICE* pDevice);

//protected:
	HANDLE							m_hAdapter;
	D3DDDI_ADAPTERFUNCS				m_AdapterFunctions;
	DWORD							m_FormatsCount;

public:
	//--- data access from callback functions ---
	friend HANDLE						GetAdapter(HANDLE hAdapter);         
	friend D3DDDI_ADAPTERFUNCS* const	GetAdapterFunctions(HANDLE hAdapter);
};

inline HANDLE						GetAdapter(HANDLE hAdapter)          { return ((UMWrapper*)hAdapter)->m_hAdapter; } 
inline D3DDDI_ADAPTERFUNCS* const	GetAdapterFunctions(HANDLE hAdapter) { return &((UMWrapper*)hAdapter)->m_AdapterFunctions; }
