#include "stdafx.h"
#include "BinaryDumper.h"
#include "D3DDeviceWrapper.h"
#include "Commands\IaSetVertexBuffers.h"
#include <fstream>

#ifndef FINAL_RELEASE

#define THREADS_COUNT	5

CBinaryDumper CBinaryDumper::m_inst;

CBinaryDumper::CBinaryDumper() 
{ 
	m_nextIndex = 0;
	m_tasksTotalSize = 0;

	m_bThreadsCreated = FALSE;
	m_bStopWorkerThreads = FALSE;
	m_indexWeAreWaitingFor = 1;

	m_bDataBinCreated = FALSE;
	m_bIndexBinCreated = FALSE;
}

CBinaryDumper::~CBinaryDumper()
{
	DEBUG_TRACE1(L"Destroying CBinaryDumper...\n");

	m_bStopWorkerThreads = TRUE;

	{
		CriticalSectionLocker lock(csLastResources);
		for (auto &it = m_lastResources.begin();it != m_lastResources.end();++it)
		{
			delete it->second;
		}
	}
	
	for(unsigned i = 0;i < hThreads.size();i++)
	{
		WaitForSingleObject(hThreads[i],INFINITE);
	}

	DEBUG_TRACE1(L"Destroying CBinaryDumper success!\n");
}

std::vector<int> CBinaryDumper::AddTask(D3DDeviceWrapper *pWrapper,D3D10DDI_HRESOURCE hStagingBuffer,D3D10DDI_HRESOURCE hres,const D3D11DDIARG_CREATERESOURCE &cres)
{
	UINT count = cres.ArraySize * cres.MipLevels;
	
	Task task;
	task.hbuffer = hStagingBuffer;
	task.hres = hres;
	task.resdim = cres.ResourceDimension;
	
	task.indexes.resize(count);
	task.subs.resize(count);
	task.poses.resize(count);
	task.subresourcesize.resize(count);

	task.pWrapper = pWrapper;
	task.threadId = GetCurrentThreadId();

	task.taskSize = 0;
	for(UINT i = 0;i < count;i++)
	{
		task.indexes[i] = InterlockedIncrement(&m_nextIndex);
		//DEBUG_TRACE1(L"Created next index: %d\n",task.indexes[i]);

		task.pWrapper->OriginalDeviceFuncs.
			pfnStagingResourceMap(pWrapper->hDevice,hStagingBuffer,i,D3D10_DDI_MAP_READ,0,&task.subs[i]);

		if (task.subs[i].pData == 0 || task.subs[i].RowPitch == 0 || task.subs[i].DepthPitch == 0)
		{
			_ASSERT(FALSE);
			break;
		}

		UINT size;
		switch(task.resdim)
		{
			case D3D10DDIRESOURCE_BUFFER:
				size = task.subs[i].RowPitch;
				break;
			case D3D10DDIRESOURCE_TEXTURE1D:		
				size = task.subs[i].RowPitch;
				break;
			case D3D10DDIRESOURCE_TEXTURE2D:
				size = task.subs[i].DepthPitch;
				break;
			case D3D10DDIRESOURCE_TEXTURE3D:
				size = task.subs[i].DepthPitch * cres.pMipInfoList[i % cres.MipLevels].PhysicalDepth;
				break;
			case D3D10DDIRESOURCE_TEXTURECUBE:		
				size = task.subs[i].DepthPitch;
				break;
			default:
				_ASSERT(0);
		}

		task.subresourcesize[i] = size;
		task.taskSize += task.subresourcesize[i];
	}

	{ 
		CriticalSectionLocker lock(csTasks);
		Tasks.push_back(task);
	}

	InterlockedExchangeAdd(&m_tasksTotalSize,task.taskSize);
	
	UINT _sleep = 100;
	for(;;)
	{
		LONG tasksSize = DoUnmapnDestroy(pWrapper);
		//DEBUG_TRACE1(L"Tasks size is %d\n",tasksSize);
		if (tasksSize < 300 * 1024 * 1024) break; // ~300 mb
		
		//DEBUG_TRACE1(L"Wait for %d milliseconds\n",_sleep);
		Sleep(_sleep *= 2);
	}

	return task.indexes;
}

LONG CBinaryDumper::DoUnmapnDestroy(D3DDeviceWrapper *pWrapper)
{
	CriticalSectionLocker lock(csUnmapnDestroy);

	DWORD id = GetCurrentThreadId();
	LONG res = InterlockedExchangeAdd(&m_tasksTotalSize,0);
	
	for(int i = (int)UnmapnDestroy.size() - 1;i >= 0;--i)
	{
		Task &t = UnmapnDestroy[i];
		if (t.threadId != id) continue; // is it important?

		for(UINT j = 0;j < t.indexes.size();j++)
		{
			pWrapper->OriginalDeviceFuncs.
				pfnStagingResourceUnmap(t.pWrapper->hDevice,t.hbuffer,j);
		}

		pWrapper->OriginalDeviceFuncs.
			pfnDestroyResource(t.pWrapper->hDevice,t.hbuffer);

		DELETE_HANDLE(t.hbuffer);

		res = InterlockedExchangeAdd(&m_tasksTotalSize,0 - t.taskSize) - t.taskSize;

		UnmapnDestroy.erase(UnmapnDestroy.begin() + i);
	}

	pWrapper->CallFlush();

	return res;
}

static UINT StartWorkerThread(void *param)
{
	_ASSERT(param);
	return ((CBinaryDumper *)param)->WorkerThread();
}

std::vector<int> CBinaryDumper::SaveResourceToBinaryFileEx(D3DDeviceWrapper *pWrapper,D3D10DDI_HRESOURCE hres)
{
	if (!hres.pDrvPrivate) return std::vector<int>();

	ResourceWrapper* pRes = (ResourceWrapper *)hres.pDrvPrivate;
	D3D10DDI_HRESOURCE hres_ = pRes->GetHandle();

	D3D11DDIARG_CREATERESOURCE cres = pRes->m_CreateResource11;
	BOOL bMultiSampled = cres.SampleDesc.Count > 1;

	// videodriver crash when we try dump this resource in Dragon Age 2
	/*if (cres.Usage == D3D10_DDI_USAGE_DEFAULT && 
		cres.Format == DXGI_FORMAT_R32_FLOAT && 
		cres.pMipInfoList[0].PhysicalWidth == 4096 && 
		cres.pMipInfoList[0].PhysicalHeight == 4096) return std::vector<int>();*/

	cres.Usage     = D3D10_DDI_USAGE_STAGING;
	cres.MapFlags  = D3D10_DDI_MAP_READ;
	cres.BindFlags = 0;				// Don't change this - we can't bind our copy to the graphics pipeline!
	cres.pPrimaryDesc = NULL;		// Don't change this - we can't present our copy!
	cres.SampleDesc.Count	= 1;	// Don't change this - we can't map copy-and-map multi-sampled resources!
	cres.SampleDesc.Quality = 0;
	cres.MiscFlags = 0;

	D3D10DDI_HRESOURCE		hStagingResource = { NULL };
	D3D10DDI_HRTRESOURCE	hRTResource = { NULL }; // pRes->m_hRTResource

	//DEBUG_TRACE1(_T("--> SaveResource: Creating resource width = %d\n"),cres.pMipInfoList->TexelWidth);

	pWrapper->CreateResource(hStagingResource,&cres,hRTResource);

#ifdef _DEBUG
	if (*(DWORD *)hStagingResource.pDrvPrivate == 0xbaadf00d) __debugbreak();
#endif // _DEBUG

	auto& originalFunc = pWrapper->OriginalDeviceFuncs;

	if (bMultiSampled)
	{
		// multi-sampled resource must be resolved prior to it being copied!
		// resolve the multi-sampled resource
		D3D11DDIARG_CREATERESOURCE cres2 = cres;
		cres2.Usage     = D3D10_DDI_USAGE_DEFAULT;
		cres2.MapFlags  = 0;

		D3D10DDI_HRESOURCE		hDefaultResource;
		pWrapper->CreateResource(hDefaultResource,&cres2,hRTResource);

		DXGI_FORMAT format_ = GetTypedDXGIFormat(cres2.Format);
		for(UINT i=0; i < cres.ArraySize * cres.MipLevels; i++)
		{
			originalFunc.pfnResourceResolveSubresource(pWrapper->hDevice,hDefaultResource,i,hres_,i,format_);
		}

		originalFunc.pfnResourceCopy(pWrapper->hDevice,hStagingResource,hDefaultResource);

		originalFunc.pfnDestroyResource(pWrapper->hDevice,hDefaultResource);
		DELETE_HANDLE(hDefaultResource);
	}
	else
	{
		originalFunc.pfnResourceCopy(pWrapper->hDevice,hStagingResource,hres_);
	}

	if (!InterlockedExchange(&m_bThreadsCreated,TRUE))
	{
		for(int i = 0;i < THREADS_COUNT;i++)
		{
			unsigned ThreadId;
			HANDLE hThread = (HANDLE)_beginthreadex( NULL, 0, (unsigned(__stdcall*)(void *))StartWorkerThread, 
				this, 0, &ThreadId);
			hThreads.push_back(hThread);
		}
	}

	return AddTask(pWrapper,hStagingResource,hres,cres);
}

void CBinaryDumper::TaskIsReady(Task &task)
{
	{ 
		CriticalSectionLocker lock(csReadyTasks);

		for(UINT i = 0;i < task.indexes.size();i++)
		{
			ReadyTasks[task.indexes[i]] = task.poses[i];
		}

		while (ReadyTasks.find(m_indexWeAreWaitingFor) != ReadyTasks.end())
		{
			UINT64 pos = ReadyTasks[m_indexWeAreWaitingFor];			
			ReadyTasks.erase(m_indexWeAreWaitingFor);

			UINT res = SaveToIndexFile(pos);
			if (res != (m_indexWeAreWaitingFor - 1) * sizeof(UINT64))
			{
				DEBUG_TRACE1(L"Pos: %d, index: %d, waiting for: %d\n",res,res / 8 + 1,m_indexWeAreWaitingFor);
				__debugbreak();
			}

			m_indexWeAreWaitingFor++;
		}
	}

	{ 
		CriticalSectionLocker lock(csUnmapnDestroy);
		UnmapnDestroy.push_back(task);
	}
}

BOOL CBinaryDumper::GetTask(Task &task)
{
	CriticalSectionLocker lock(csTasks);

	if (Tasks.size() > 0)
	{
		task = Tasks.front();
		Tasks.pop_front();
		return TRUE;
	}

	return FALSE;
}

UINT CBinaryDumper::WorkerThread()
{
	DEBUG_TRACE1(L"WorkerThread start\n");

	for(;;)
	{
		Task task;
		if (GetTask(task))
		{
			// store
			for(UINT i = 0;i < task.indexes.size();i++)
			{
				if (!task.subs[i].RowPitch) __debugbreak();
				if (!task.subs[i].DepthPitch) __debugbreak();

				task.poses[i] = SaveToBinaryFile(
					task.subs[i].pData,task.subresourcesize[i],
					task.subs[i].RowPitch,task.subs[i].DepthPitch);
			
				//DEBUG_TRACE1(L"Saved data for index %d at pos %u\n",task.indexes[i],(UINT)task.poses[i]);
			}

			// add to queue to store to index file
			TaskIsReady(task);
		}
		else
		{
			if (m_bStopWorkerThreads) break;
			Sleep(100);
		}
	}

	DEBUG_TRACE1(L"WorkerThread end\n");

	return 0;
}

UINT64 CBinaryDumper::SaveToBinaryFileInternal(char *fname,void *data,DWORD len,DWORD *params,DWORD count,BOOL bAlign)
{
	std::vector<DWORD> key;
	key.push_back(CalculateCRC32CFast(data,len));
	key.push_back(len);
	for(unsigned i = 0;i < count;i++)
		key.push_back(params[i]);

	{ 
		CriticalSectionLocker lock(csDataBin);

		auto &it = m_storedDataCache.find(key);
		if (it != m_storedDataCache.end()) return it->second;

		using namespace std;
		ofstream myFile;
		
		for(int i = 0;;i++)
		{
			if (i > 100) __debugbreak();

			myFile.open(fname, ios::out | ios::binary | (m_bDataBinCreated ? ios::app : ios::trunc));
			if (!myFile.is_open())
			{
				DEBUG_TRACE1(L"SaveToIndexFile: file open failed, errno = %d\n",errno);
				::Sleep(100);
				continue;
			}

			myFile.seekp(0,m_bDataBinCreated ? ios::end : ios::beg);
			if (myFile.fail())
			{
				DEBUG_TRACE1(L"SaveToIndexFile: seekp failed, errno = %d\n",errno);

				myFile.close();
				::Sleep(100);
				continue;
			}

			// ok
			break;
		}
		
		if (!m_bDataBinCreated)
		{
			const char buf[] = { 0,0,0,0,'i','Z','3','D',0,0,0,2 };
			myFile.write(buf,sizeof(buf));
		}
		

		UINT64 res = (UINT)myFile.tellp();
		if ((LONG64)res < 0)
		{
			DEBUG_TRACE1(L"tellp return value %d < 0, errno = %d\n",(int)res,errno);
			__debugbreak();

			res = (UINT)myFile.tellp();
			DEBUG_TRACE1(L"second try: %d, %d\n",(int)res,errno);
		}
		DWORD offset = sizeof(count) + count * sizeof(DWORD) + sizeof(len);
		if (bAlign && (((res + offset) & 0xF) != 0))
		{
			static DWORD ReservedBytes[4] = { 0, 0, 0, 0 };
			DWORD offsetLen = 16 - ((res + offset) & 0xF);
			myFile.write((const char *)&ReservedBytes,offsetLen);
			res = (UINT)myFile.tellp();
		}

		myFile.write((const char *)&count,sizeof(count));
		for(unsigned i = 0;i < count;i++)
		{
			myFile.write((const char *)(params + i),sizeof(DWORD));
		}

		myFile.write((const char *)&len,sizeof(DWORD));
		myFile.write((const char *)data,len);

		myFile.close();
		m_bDataBinCreated = TRUE;

		m_storedDataCache[key] = res;

		return res;
	}
}

UINT CBinaryDumper::SaveToBinaryFile(void *data,DWORD len,DWORD pitch1,DWORD pitch2)
{
	char fname[MAX_PATH];
	sprintf_s(fname, MAX_PATH,"%S\\data.bin", gData.DumpDirectory);

	DWORD params[] = { pitch1, pitch2 };
	return (UINT)SaveToBinaryFileInternal(fname,data,len,params,2);
}

UINT64 CBinaryDumper::SaveToBinaryFile2(void *data,DWORD len,DWORD pitch1,DWORD pitch2,BOOL bAlign)
{
	DWORD params[] = { pitch1, pitch2 };
	UINT64 pos = SaveToBinaryFileInternal("src\\data2.bin",data,len,params,2,bAlign);

	return pos;
}

UINT CBinaryDumper::SaveToIndexFile(UINT64 data)
{
	char fname[MAX_PATH];
	sprintf_s(fname, MAX_PATH,"%S\\index.bin", gData.DumpDirectory);

	{
		CriticalSectionLocker lock(csIndexBin);

		using namespace std;
		ofstream myFile;

		if (!m_bIndexBinCreated)
		{
			DEBUG_TRACE1(L"SaveToIndexFile: Creating new index file, threadid = %d\n",GetCurrentThreadId());
		}
	
		for(int i = 0;;i++)
		{
			if (i > 100) __debugbreak();

			myFile.open(fname, ios::out | ios::binary | (m_bIndexBinCreated ? ios::app : ios::trunc));
			if (!myFile.is_open())
			{
				DEBUG_TRACE1(L"SaveToIndexFile: file open failed, errno = %d\n",errno);
				::Sleep(100);
				continue;
			}

			myFile.seekp(0,m_bIndexBinCreated ? ios::end : ios::beg);
			if (myFile.fail())
			{
				DEBUG_TRACE1(L"SaveToIndexFile: seekp failed, errno = %d\n",errno);

				myFile.close();
				::Sleep(100);
				continue;
			}

			// ok
			break;
		}

		UINT res = (UINT)myFile.tellp();
		if ((LONG64)res < 0)
		{
			DEBUG_TRACE1(L"SaveToIndexFile: tellp return value %d < 0, errno = %d\n",(int)res,errno);
			__debugbreak();

			res = (UINT)myFile.tellp();
			DEBUG_TRACE1(L"SaveToIndexFile: second try: %d, %d\n",(int)res,errno);
		}

		myFile.write((const char *)&data,sizeof(data));
		myFile.close();

		m_bIndexBinCreated = TRUE;

		//DEBUG_TRACE1(L"Index %u has been written to index.bin\n",res / 8 + 1);

		return res;
	}
}

void CBinaryDumper::SetResourceOfView(D3D10DDI_HSHADERRESOURCEVIEW hview,D3D10DDI_HRESOURCE hres)
{
	CriticalSectionLocker lock(csViewsResourcesMap);
	m_viewsResourcesMap[hview.pDrvPrivate] = hres.pDrvPrivate;
}

D3D10DDI_HRESOURCE CBinaryDumper::GetResourceOfView(D3D10DDI_HSHADERRESOURCEVIEW hview)
{
	if (!hview.pDrvPrivate || m_viewsResourcesMap.find(hview.pDrvPrivate) == m_viewsResourcesMap.end())
	{
		D3D10DDI_HRESOURCE hres = { 0 };
		return hres;
	}

	D3D10DDI_HRESOURCE hres = { m_viewsResourcesMap[hview.pDrvPrivate] };
	return hres;
}

void CBinaryDumper::SetLastResources(D3DDeviceWrapper *pWrapper,UINT cmdId,D3D10DDI_HRESOURCE *resources,UINT count,UINT startSlot)
{
	if (!resources) return;

	CriticalSectionLocker lock(csLastResources);
	LastResourcesMap &map = GetLastResourcesMap(pWrapper);

	std::vector<D3D10DDI_HRESOURCE> *data = &map[cmdId];
	if (data->size() < startSlot + count) data->resize(startSlot + count);
	for(UINT i = 0;i < count;i++)
	{
		(*data)[startSlot + i] = resources[i];
	}
}

void CBinaryDumper::SetLastResources(D3DDeviceWrapper *pWrapper,UINT cmdId,D3D10DDI_HSHADERRESOURCEVIEW *views,UINT count,UINT startSlot)
{
	if (!views) return;

	D3D10DDI_HRESOURCE *res = new D3D10DDI_HRESOURCE[count];
	csViewsResourcesMap.Enter();
	for(UINT i = 0;i < count;i++)
	{
		res[i] = GetResourceOfView(views[i]);
	}
	csViewsResourcesMap.Leave();
	SetLastResources(pWrapper,cmdId,res,count,startSlot);
	delete[]res;
}

void CBinaryDumper::SetLastResources( D3DDeviceWrapper *pWrapper,UINT cmdId,Commands::IaBuffer *resources,UINT count,UINT startSlot )
{
	if (!resources) return;

	CriticalSectionLocker lock(csLastResources);
	LastResourcesMap &map = GetLastResourcesMap(pWrapper);

	std::vector<D3D10DDI_HRESOURCE> *data = &map[cmdId];
	if (data->size() < startSlot + count) data->resize(startSlot + count);
	for(UINT i = 0;i < count;i++)
	{
		(*data)[startSlot + i] = resources[i].buffer;
	}
}

void CBinaryDumper::GetLastResourcesArray(D3DDeviceWrapper *pWrapper,std::vector<D3D10DDI_HRESOURCE> &ResourcesSrc)
{
	ResourcesSrc.clear();

	CriticalSectionLocker lock(csLastResources);
	LastResourcesMap &map = GetLastResourcesMap(pWrapper);

	std::set<void *> already;
	for(auto it = map.begin();it != map.end();++it)
	{
		std::vector<D3D10DDI_HRESOURCE> &data = it->second;
		for(UINT i = 0;i < data.size();i++)
		{
			if (data[i].pDrvPrivate && already.find(data[i].pDrvPrivate) == already.end())
			{
				ResourcesSrc.push_back(data[i]);
				already.insert(data[i].pDrvPrivate);
			}
		}
	}
}

CBinaryDumper::LastResourcesMap &CBinaryDumper::GetLastResourcesMap(D3DDeviceWrapper *pWrapper)
{ 
	UINT_PTR id = (UINT_PTR)pWrapper->hDevice.pDrvPrivate;
	auto &it = m_lastResources.find(id);

	if (it == m_lastResources.end())
	{
		LastResourcesMap *map = new LastResourcesMap;
		m_lastResources[id] = map;
		return *map;
	}
	else
	{
		return *it->second;
	}
}

#endif // FINAL_RELEASE