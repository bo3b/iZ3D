/* 
* Project : iZ3D Stereo Driver
* Copyright (C) iZ3D Inc. 2002 - 2010
*/

#pragma once

#include <queue>
#include <boost\scoped_array.hpp>
#include "..\CommonUtils\Singleton.h"

class DumpManager;

class DumpData
{
public:
	DumpData() { pSurface = NULL; pData = NULL; lockedRect.pBits = NULL; }
	std::wstring		fileName;
	D3DSURFACE_DESC		desc;
	IDirect3DSurface9*	pSurface; // only for multi threaded devices
	D3DLOCKED_RECT		lockedRect;
	void*				pData;
};

#ifndef FINAL_RELEASE

class DumpManager 
	: public iz3d::Singleton<DumpManager>
{
public:
	DumpManager();
	~DumpManager();

	bool AddSurface(LPTSTR pFileName, IDirect3DSurface9* pSurf);

	bool UseOldMethod( DumpData* pData, IDirect3DSurface9* pSrcSurface );
	void SetMTDevice() { m_bMTDevice = true; }
	void WaitWhenFinish();

protected:
	bool DecompressData( DumpData* pData, IDirect3DSurface9* pSurf );
	bool ConvertDataByGPU( DumpData* pData, IDirect3DSurface9* pSurf );
	bool ResolveRT(DumpData* pData, IDirect3DSurface9* pSurf);
	void ProcessDumpData(DumpData* pData);
	bool SaveImageToFile(DumpData* pData);
	void StartThread();
	void DumpThread();
	static  DWORD WINAPI DumpThreadProc(DumpManager* manager);
	bool	m_bMTDevice;
	HANDLE  m_hDumpThread;
	CriticalSection m_Access;
	std::queue<DumpData>	m_Data;
};

#endif
