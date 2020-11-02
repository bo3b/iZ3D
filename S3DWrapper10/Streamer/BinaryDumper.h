#pragma once

#ifndef FINAL_RELEASE

#include <list>
#include "..\CommonUtils\System.h"

namespace Commands
{
	struct IaBuffer;
}
class D3DDeviceWrapper;

class CBinaryDumper
{
public:
	static CBinaryDumper &GetInstance() { return m_inst; }

	std::vector<int> SaveResourceToBinaryFileEx(D3DDeviceWrapper *pWrapper,D3D10DDI_HRESOURCE hres);

	UINT SaveToBinaryFile(void *data,DWORD len,DWORD pitch1 = 0,DWORD pitch2 = 0);
	UINT64 SaveToBinaryFile2(void *data,DWORD len,DWORD pitch1,DWORD pitch2,BOOL bAlign = FALSE);

	void SetResourceOfView(D3D10DDI_HSHADERRESOURCEVIEW hview,D3D10DDI_HRESOURCE hres);
	D3D10DDI_HRESOURCE GetResourceOfView(D3D10DDI_HSHADERRESOURCEVIEW hview);

	void SetLastResources(D3DDeviceWrapper *pWrapper,UINT cmdId,D3D10DDI_HRESOURCE *resources,UINT count,UINT startSlot);
	void SetLastResources(D3DDeviceWrapper *pWrapper,UINT cmdId,D3D10DDI_HSHADERRESOURCEVIEW *views,UINT count,UINT startSlot);
	void SetLastResources(D3DDeviceWrapper *pWrapper,UINT cmdId,Commands::IaBuffer *resources,UINT count,UINT startSlot);
	void GetLastResourcesArray(D3DDeviceWrapper *pWrapper,std::vector<D3D10DDI_HRESOURCE> &ResourcesSrc);

protected:
	struct Task
	{
		std::vector<int> indexes;

		D3D10DDI_HRESOURCE hbuffer;
		D3D10DDI_HRESOURCE hres;
		D3D10DDIRESOURCE_TYPE resdim;
		UINT taskSize;

		DWORD threadId;

		D3DDeviceWrapper *pWrapper;

		std::vector<D3D10DDI_MAPPED_SUBRESOURCE> subs;
		std::vector<UINT64> poses;
		std::vector<UINT> subresourcesize;

		explicit Task()  { }

		explicit Task(const Task &t) 
		{ 
			*this = t; 
		}

		void operator =(const Task &t)
		{
			indexes = t.indexes;
			subs = t.subs;
			poses = t.poses;

			hbuffer = t.hbuffer;
			hres = t.hres;
			resdim = t.resdim;

			taskSize = t.taskSize;
			subresourcesize = t.subresourcesize;

			pWrapper = t.pWrapper;
			threadId = t.threadId;
		}
	};

protected:
	CBinaryDumper();
	~CBinaryDumper();

	typedef std::map<UINT, std::vector<D3D10DDI_HRESOURCE> > LastResourcesMap;
	LastResourcesMap &GetLastResourcesMap(D3DDeviceWrapper *pWrapper);

	UINT64 SaveToBinaryFileInternal(char *fname,void *data,DWORD len,DWORD *params,DWORD count,BOOL bAlign = FALSE);
	UINT SaveToIndexFile(UINT64 data);

	std::vector<int> AddTask(D3DDeviceWrapper *pWrapper,D3D10DDI_HRESOURCE hbuffer,D3D10DDI_HRESOURCE hres,const D3D11DDIARG_CREATERESOURCE &cres);
	BOOL GetTask(Task &task);
	void TaskIsReady(Task &task);
	LONG DoUnmapnDestroy(D3DDeviceWrapper *pWrapper);
	UINT WorkerThread();

protected:
	CriticalSection csTasks;
	std::list<Task> Tasks;

	CriticalSection csUnmapnDestroy;
	std::vector<Task> UnmapnDestroy;

	CriticalSection csReadyTasks;
	std::map<int, UINT64> ReadyTasks;
	int m_indexWeAreWaitingFor;

	CriticalSection csDataBin;
	BOOL m_bDataBinCreated;
	std::map<std::vector<DWORD>,UINT64> m_storedDataCache;

	CriticalSection csIndexBin;
	BOOL m_bIndexBinCreated;

	CriticalSection csViewsResourcesMap;
	std::map<void *,void *> m_viewsResourcesMap;

	CriticalSection csLastResources;
	std::map<UINT_PTR,LastResourcesMap *> m_lastResources;

	LONG m_bThreadsCreated;
	std::vector<HANDLE> hThreads;
	BOOL m_bStopWorkerThreads;

	LONG m_nextIndex;
	LONG m_tasksTotalSize;

	static CBinaryDumper m_inst;

	friend UINT StartWorkerThread(void *param);
};

#endif // FINAL_RELEASE