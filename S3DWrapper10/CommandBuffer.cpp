#include "StdAfx.h"
#include "D3DDeviceWrapper.h"
#include "D3DSwapChain.h"
#include "CommandBuffer.h"
#include "Commands\CommandSet.h"
#include "RenderTargetViewWrapper.h"
#include <numeric>

// Trace message and send to WriteStreamer if necessary
void DumpMessage(D3DDeviceWrapper *pWrapper, const char* format, ...)
{
#ifndef FINAL_RELEASE
	static const size_t buffer_size = 512;
	static char			buffer[buffer_size];
		
	va_list args;
	va_start(args, format);
	vsnprintf(buffer, buffer_size, format, args);
	va_end(args);

	DEBUG_TRACE3(_T("%S"), buffer);
	if (GINFO_DUMP_ON && pWrapper->m_DumpType >= dtTrace)
		WriteStreamer::Comment(buffer);
#endif
}

CommandBuffer::CommandBuffer()
:	m_pWrapper			(NULL)
,	m_bHaveDrawCommand	(false)
,	m_WantProcess		(false)
,	m_WantFinalize		(false)
,	m_bProcessing		(false)
,	m_FinalizeCommand	(NULL)
{
	//m_LeftBuffer.reserve( m_MaxBufferSize );
}
	
CommandBuffer::~CommandBuffer()
{
}

void CommandBuffer::Init( class D3DDeviceWrapper* pWrapper_ )
{
	m_pWrapper = pWrapper_;
}

void CommandBuffer::WriteCommand( const Commands::TCmdPtr& cmd_ )
{
#ifndef FINAL_RELEASE
	_ASSERT( cmd_ );

	std::ostringstream ss;
	cmd_->Dump(ss);
	DEBUG_TRACE2( _T("\t%S"), ss.str().c_str() );

	if(GINFO_DUMP_ON && WriteStreamer::IsOK())
	{
		if ( m_pWrapper->m_DumpType >= dtTrace )
			cmd_->WriteToFile( m_pWrapper );
		else
			cmd_->WriteToFileSimple( m_pWrapper );
	}
#endif
}

#ifndef FINAL_RELEASE

void CommandBuffer::ClearStatistics()
{
	/* nothing to do */
}

void CommandBuffer::DumpStatistics()
{
	/* nothing to do */
}
#endif

void CommandBuffer::DumpDestParam( const Commands::TCmdPtr& cmd_, bool b2RT, DumpView view )
{
#ifndef FINAL_RELEASE
	if (!cmd_)
		return;

	ResourceWrapper* pWrapper;
	ResourceWrapper* pSrcWrapper;
	if (GetCommandResources(cmd_.get(), pWrapper, pSrcWrapper))
	{
		if (pSrcWrapper)
		{
			int srcResID = m_pWrapper->m_ResourceManager.GetID(pSrcWrapper->GetHandle());
			DEBUG_MESSAGE(_T("\t\tSrc.%s%d\n"), pSrcWrapper->GetResourceName(), srcResID);
		}

		TCHAR szFileName[MAX_PATH];
		TCHAR resourceName[128];
		TCHAR EIDText[20];
		EIDText[0] = '\0';
		if (gInfo.EID != 0)
			_stprintf_s(EIDText, L"[%.4I64d].", gInfo.EID);
		int resID = m_pWrapper->m_ResourceManager.GetID(pWrapper->GetHandle());
		if (view == dvBoth)
		{
			_stprintf_s(szFileName, L"%s\\%s%.4d.%S.Dst.", 
				m_pWrapper->m_DumpDirectory, EIDText, m_pWrapper->m_CBCount, CommandIDToString(cmd_->CommandId));
		}
		else
		{
			_stprintf_s(szFileName, L"%s\\%s%.4d.%S.%.4d.%S.Dst.", 
				m_pWrapper->m_DumpDirectory, EIDText, m_pWrapper->m_CBCount, view == 1 ? "L" : "R",
				m_pWrapper->m_CBSubIndexCount, CommandIDToString(cmd_->CommandId));
			m_pWrapper->m_CBSubIndexCount++;
		}
		_stprintf_s(resourceName, L"%s%d", pWrapper->GetResourceName(), resID);
		pWrapper->DumpToFile(m_pWrapper, szFileName, resourceName, b2RT, false, view);
	}
#endif
}

void CommandBuffer::Finalize() 
{
	if ( !Empty() ) {
		FlushAll();
	}

	m_bProcessing = false; 
	m_WantFinalize = false; 
}

void CommandBuffer::Clear()
{
	m_LeftBuffer.clear();
	m_FinalizeCommand = NULL;
	m_WantProcess	  = false;
	m_bHaveDrawCommand = false;
}

void CommandBuffer::AddFinalizeCommand( Commands::Command* pCmd_, bool WantFinalize )
{
	_ASSERT(m_FinalizeCommand == NULL);
	m_FinalizeCommand = pCmd_;
	m_WantProcess = true;
	m_WantFinalize = WantFinalize;
}

void CommandBuffer::AddMonoCommand( Commands::Command* pCmd_, bool WantProcess )
{
	Commands::TCmdPtr ptr( pCmd_ );
	m_LeftBuffer.push_back( ptr );
	m_WantProcess = WantProcess;
}

bool CommandBuffer::GetCommandResources( const Commands::Command* cmd_, ResourceWrapper* &pDstResource, ResourceWrapper* &pSrcResource )
{
	pDstResource = NULL;
	pSrcResource = NULL;
	switch(cmd_->CommandId)
	{
	case idClearDepthStencilView:
		break;
	case idClearRenderTargetView:
		{
			const Commands::ClearRenderTargetView* pCmd = (const Commands::ClearRenderTargetView*)cmd_;
			RenderTargetViewWrapper* pRTVWrapper = NULL;
			InitWrapper(pCmd->hRenderTargetView_, pRTVWrapper);
			if (pRTVWrapper)
			{
				pDstResource = pRTVWrapper->GetResourceWrapper();
				return true;
			}
		}
		break;
	case idGenMips:
		break;
	case idResourceCopy:
		{
			const Commands::ResourceCopy* pCmd = (const Commands::ResourceCopy*)cmd_;
			InitWrapper(pCmd->hSrcResource_, pSrcResource);
			InitWrapper(pCmd->hDstResource_, pDstResource);
			return true;
		}
		break;
	case idResourceCopyRegion:
		{
			const Commands::ResourceCopyRegion* pCmd = (const Commands::ResourceCopyRegion*)cmd_;
			InitWrapper(pCmd->hSrcResource_, pSrcResource);
			InitWrapper(pCmd->hDstResource_, pDstResource);
			return true;
		}
		break;
	case idResourceReadAfterWriteHazard:
		break;
	case idResourceResolveSubresource:
		{
			const Commands::ResourceResolveSubresource* pCmd = (const Commands::ResourceResolveSubresource*)cmd_;
			InitWrapper(pCmd->hSrcResource_, pSrcResource);
			InitWrapper(pCmd->hDstResource_, pDstResource);
			return true;
		}
		break;
	case idShaderResourceViewReadAfterWriteHazard:
		break;
	case idResourceConvert10_1:
		{
			const Commands::ResourceConvert10_1* pCmd = (const Commands::ResourceConvert10_1*)cmd_;
			InitWrapper(pCmd->hSrcResource_, pSrcResource);
			InitWrapper(pCmd->hDstResource_, pDstResource);
			return true;
		}
		break;
	case idResourceConvertRegion10_1:
		{
			const Commands::ResourceConvertRegion10_1* pCmd = (const Commands::ResourceConvertRegion10_1*)cmd_;
			InitWrapper(pCmd->hSrcResource_, pSrcResource);
			InitWrapper(pCmd->hDstResource_, pDstResource);
			return true;
		}
	}

	return false;
}

void CommandBuffer::SetRTTextureType( TextureType tt )
{
	Commands::Command* cmd = m_pWrapper->m_DeviceState.m_OMRenderTargets.get();
	cmd->SetDestResourceType( tt );
#ifndef FINAL_RELEASE
	DestResourcesSet res;
	cmd->GetDestResources(res);
	int dstResID = -1;
	TextureType newtt = TT_SETLIKESRC;
	const wchar_t* name = L"";
	if (!res.empty())
	{
		ResourceWrapper* pDstWrapper = (ResourceWrapper*)(*res.cbegin()).pDrvPrivate;
		if (pDstWrapper)
		{
			dstResID = m_pWrapper->m_ResourceManager.GetID(pDstWrapper->GetHandle());
			newtt = pDstWrapper->m_Type;
			name = pDstWrapper->GetResourceName();
		}
	}
	DEBUG_TRACE3(L"\t\tSetRTTextureType (Dst %s%d) - %S (%S)\n", 
		name, dstResID, EnumToString(tt), EnumToString(newtt));
#endif
}
