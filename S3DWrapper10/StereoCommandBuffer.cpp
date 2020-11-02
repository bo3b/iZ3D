#include "StdAfx.h"
#include "D3DDeviceWrapper.h"
#include "D3DSwapChain.h"
#include "ResourceWrapper.h"
#include "StereoCommandBuffer.h"
#include "Commands\CommandSet.h"
#include <numeric>

namespace {

	Commands::TCmdPtr DeviceViewMono( new Commands::ChangeDeviceView(VIEWINDEX_MONO) );
	Commands::TCmdPtr DeviceViewLeft( new Commands::ChangeDeviceView(VIEWINDEX_LEFT) );
	Commands::TCmdPtr DeviceViewRight( new Commands::ChangeDeviceView(VIEWINDEX_RIGHT) );
	Commands::TCmdPtr ShaderMatrixViewNone( new Commands::ChangeShaderMatrixView() );
	
} // anonymous namespace

StereoCommandBuffer::StereoCommandBuffer()
#ifndef FINAL_RELEASE
:	m_BufferDrawCount(0)
,	m_ViewChangeCount(0)
#endif
{
#ifndef FINAL_RELEASE
	std::fill(m_FinalizeCommandCount, m_FinalizeCommandCount + idTotal, 0);
#endif
}

StereoCommandBuffer::~StereoCommandBuffer()
{
}

void StereoCommandBuffer::Execute()
{
	_ASSERT( m_pWrapper );

#ifndef FINAL_RELEASE
	if (m_bHaveDrawCommand)
		m_pWrapper->m_StereoBuffersCount++;
	m_pWrapper->m_CBCount++;
	m_pWrapper->m_CBSubIndexCount = 0;
	DumpMessage(m_pWrapper, "######################## [%d] StereoCommandBuffer::Execute ########################", m_pWrapper->m_CBCount);
#endif

	for (auto it = m_ResourcesToRestoreType.begin(); it != m_ResourcesToRestoreType.end(); ++it) {
		static_cast<ResourceWrapper*>(it->pDrvPrivate)->StoreType();
	}

	DumpMessage(m_pWrapper, "######################## left buffer ########################");
	ExecuteBuffer(m_LeftBuffer, true, true);
#ifndef FINAL_RELEASE
	m_pWrapper->m_CBSubIndexCount = 0;
#endif
	DumpMessage(m_pWrapper, "######################## prologue buffer ########################");
	ExecutePrologueBuffer();
	DumpMessage(m_pWrapper, "######################## right buffer ########################");
	ExecuteBuffer(m_RightBuffer, true, false);
	DumpMessage(m_pWrapper, "######################## rest buffer ########################");
	ExecuteBuffer(m_RestBuffer, true, true);
	
	if (m_WantFinalize) 
	{
		DumpMessage(m_pWrapper, "######################## epilogue buffer ########################");
		ExecuteEpilogueBuffer();
	}

	if (m_FinalizeCommand)
	{
#ifndef FINAL_RELEASE
		DumpMessage(m_pWrapper, "######################## finalize ########################");
		m_pWrapper->m_StereoCommandsCount++;
#endif
		WriteCommand(m_FinalizeCommand);
		CriticalSectionLocker locker(m_pWrapper->m_CSUMCall);
		m_FinalizeCommand->Execute(m_pWrapper);
		if (m_FinalizeCommand->State_ & COMMAND_CHANGE_DEVICE_STATE) 
		{
			m_FinalizeCommand->UpdateDeviceState(m_pWrapper, &m_pWrapper->m_DeviceState);
#ifndef FINAL_RELEASE
			m_FinalizeCommand->UpdateDeviceState(m_pWrapper, &m_pWrapper->m_DeviceStateAtExecute);
			m_pWrapper->m_SetRenderTargetCount++;
#endif
		}
#ifndef FINAL_RELEASE
		if (GINFO_DUMP_ON && m_pWrapper->m_DumpType >= dtOnlyRT)
		{
			const Commands::DispatchBase11_0* pDispatchCmd = static_cast<const Commands::DispatchBase11_0*>( m_FinalizeCommand.get() );
			if( pDispatchCmd && (pDispatchCmd->State_ & COMMAND_DEPENDED_ON_COMPUTE_DEVICE_STATE) )
			{
				DumpView dview = (Commands::Command::CurrentView_ != VIEWINDEX_RIGHT) ? dvLeft : dvRight;
				m_pWrapper->DumpCSTextures(pDispatchCmd->m_bStereoDispatch, dview);
				m_pWrapper->DumpUAVs(true, pDispatchCmd->m_bStereoDispatch, dview);
			}
		}
#endif
	}

	DumpMessage(m_pWrapper, "######################## end ########################");
}

void StereoCommandBuffer::ExecuteBuffer(const TCommandsVector& buffer, bool bDump, bool bLeft)
{
#ifndef FINAL_RELEASE
	m_pWrapper->m_StereoCommandsCount += buffer.size();
#endif

	for(auto it = buffer.begin(); it != buffer.end(); ++it)
	{	
		WriteCommand(*it);
		CriticalSectionLocker locker(m_pWrapper->m_CSUMCall);
		(*it)->Execute( m_pWrapper );

#ifndef FINAL_RELEASE
		if ( (*it)->State_ & COMMAND_CHANGE_DEVICE_STATE ) {
			(*it)->UpdateDeviceState(m_pWrapper, &m_pWrapper->m_DeviceStateAtExecute);
		}
		if (bDump && GINFO_DUMP_ON && m_pWrapper->m_DumpType >= dtOnlyRT)
		{
			const Commands::DrawBase* pDrawCmd = static_cast<const Commands::DrawBase*>( (*it).get() );
			if( pDrawCmd && (pDrawCmd->State_ & COMMAND_DEPENDED_ON_DEVICE_STATE) )
			{
				DumpView dview = bLeft ? dvLeft : dvRight;
				m_pWrapper->DumpTextures(pDrawCmd->m_bStereoDraw, dview);
				m_pWrapper->DumpRT(true, pDrawCmd->m_bStereoDraw, dview);
				++m_pWrapper->m_CBSubIndexCount;
			}
			else if( (*it)->State_ & COMMAND_DEPENDED_ON_COMPUTE_DEVICE_STATE )
			{
				const Commands::DispatchBase11_0* pDispatchCmd = static_cast<const Commands::DispatchBase11_0*>( (*it).get() );
				DumpView dview = (Commands::Command::CurrentView_ != VIEWINDEX_RIGHT) ? dvLeft : dvRight;
				m_pWrapper->DumpCSTextures(pDispatchCmd->m_bStereoDispatch, dview);
				m_pWrapper->DumpUAVs(true, pDispatchCmd->m_bStereoDispatch, dview);
				++m_pWrapper->m_CBSubIndexCount;
			}
			else if( (*it)->State_ & COMMAND_MAY_USE_STEREO_RESOURCES ) {
				DumpDestParam(*it, true, bLeft ? dvLeft : dvRight);
			}
		}
#endif
	}
}

void StereoCommandBuffer::Clear()
{
#ifndef FINAL_RELEASE
	m_BufferDrawCount = 0;
#endif
	CommandBuffer::Clear();
	m_RightBuffer.clear();
	m_PrologueBuffer.clear();
	m_EpilogueBuffer.clear();
	m_RestBuffer.clear();
	m_StatesForRestoration.clear();
	m_ResourcesToRestoreType.clear();
	m_ResourcesForRestoration.clear();
	m_ResourcesUsedNotUpdated.clear();
	m_ResourcesUpdated.clear();
}

void StereoCommandBuffer::ExecutePrologueBuffer()
{
	// restore resource types
	for (auto it = m_ResourcesToRestoreType.begin(); it != m_ResourcesToRestoreType.end(); ++it) {
		static_cast<ResourceWrapper*>(it->pDrvPrivate)->RestoreType();
	}

	// restore states
	m_PrologueBuffer.push_back(DeviceViewRight);
	m_PrologueBuffer.push_back(ShaderMatrixViewNone);
	for (auto it = m_StatesForRestoration.begin(); it != m_StatesForRestoration.end(); ++it) {
		m_PrologueBuffer.push_back(it->second);
	}
	
	// restore resources
	for (auto it = m_ResourcesForRestoration.begin(); it != m_ResourcesForRestoration.end(); ++it) {
		m_PrologueBuffer.push_back(it->second);
	}

	ExecuteBuffer(m_PrologueBuffer);
}

void StereoCommandBuffer::ExecuteEpilogueBuffer()
{
	m_EpilogueBuffer.push_back(DeviceViewMono);
	m_EpilogueBuffer.push_back(m_pWrapper->m_DeviceState.m_OMRenderTargets);
	if (m_pWrapper->m_DeviceState.m_CsUnorderedAccessViews) {
		m_EpilogueBuffer.push_back(m_pWrapper->m_DeviceState.m_CsUnorderedAccessViews);
	}
	m_EpilogueBuffer.push_back(ShaderMatrixViewNone);

	// setup mono constant buffers/resources
	for (int i = 0; i<SP_COUNT; ++i)
	{
		ShaderPipelineStates* pl = m_pWrapper->m_DeviceState.GetShaderPipeline(SHADER_PIPELINE(i));
		if (pl)
		{
			m_EpilogueBuffer.push_back(pl->m_ConstantBuffers);
			m_EpilogueBuffer.push_back(pl->m_ShaderResources);
		}
	}

	ExecuteBuffer(m_EpilogueBuffer);
}

void StereoCommandBuffer::Begin()
{
	CommandBuffer::Begin();

	// init left
	Commands::TCmdPtr pOMRenderTargets = m_pWrapper->m_DeviceState.m_OMRenderTargets->Clone();
	Commands::TCmdPtr pUAVs = m_pWrapper->m_DeviceState.m_CsUnorderedAccessViews ? m_pWrapper->m_DeviceState.m_CsUnorderedAccessViews->Clone() : Commands::TCmdPtr();
	m_LeftBuffer.push_back(DeviceViewLeft);
	m_LeftBuffer.push_back(pOMRenderTargets);
	if (pUAVs) {
		m_LeftBuffer.push_back(pUAVs);
	}

	// setup left resources
	for (int i = 0; i<SP_COUNT; ++i)
	{
		ShaderPipelineStates* pl = m_pWrapper->m_DeviceState.GetShaderPipeline(SHADER_PIPELINE(i));
		if (pl) {
			m_LeftBuffer.push_back( pl->m_ShaderResources->Clone() ); // setup left resources
		}
	}

	// init right
	m_RightBuffer.push_back(DeviceViewRight);
	m_RightBuffer.push_back(pOMRenderTargets);
	if (pUAVs) {
		m_RightBuffer.push_back(pUAVs);
	}

	// setup right resources
	for (int i = 0; i<SP_COUNT; ++i)
	{
		ShaderPipelineStates* pl = m_pWrapper->m_DeviceState.GetShaderPipeline(SHADER_PIPELINE(i));
		if (pl) {
			m_RightBuffer.push_back( pl->m_ShaderResources->Clone() ); // setup left resources
		}
	}
}

void StereoCommandBuffer::FlushAll()
{
	CommandBuffer::FlushAll();

	Execute();
	Clear();
	
#ifndef FINAL_RELEASE
	DEBUG_TRACE3(_T("Draw count: %d\n"), m_BufferDrawCount);
#endif
}

//////////////////////////////////////////////////////////////////////////

void StereoCommandBuffer::AddChangeStateCommand( Commands::Command* pCmd_ )
{
	if (m_StatesForRestoration.count(pCmd_->CommandId) == 0) {
		m_StatesForRestoration[pCmd_->CommandId] = m_pWrapper->m_DeviceState.GetLastCommand(pCmd_->CommandId, true);
	}
	AddStereoCommand(pCmd_);
	pCmd_->UpdateDeviceState(m_pWrapper, &m_pWrapper->m_DeviceState);
}

void StereoCommandBuffer::AddCommandThatMayUseStereoResources( Commands::Command* pCmd_, int type )
{
	if (type != -1) {
		AddSetDestResourceTypeCommand(pCmd_, TextureType(type));
	}
	AddStereoCommand( pCmd_, pCmd_->IsUsedStereoResources(m_pWrapper) );
	// if command modifies mono resources, we need to flush buffer, because theese resources
	// could be used by previous draw calls (remove this if I wrong)
	//if ( pCmd_->IsUsedStereoResources(m_pWrapper) ) {
	//	AddStereoCommand( pCmd_, pCmd_->IsUsedStereoResources(m_pWrapper) );
	//}
	//else {
	//	AddFinalizeCommand(pCmd_);
	//}

	if (USE_UM_PRESENTER && m_pWrapper->CheckFlush())
	{
		m_pWrapper->m_bFlushCommands = true;
		m_WantProcess = true;
	}
}

void StereoCommandBuffer::AddDrawCommand(Commands::DrawBase* pCmd_, int drawType)
{
	// generate pre draw command sequence
	Commands::TChangeShaderMatrixViewCmdPtr ShaderMatrixView;
	for (int i = 0; i<SP_COUNT; ++i)
	{
		ShaderPipelineStates* pl = m_pWrapper->m_DeviceState.GetShaderPipeline(SHADER_PIPELINE(i));
		if (!pl || !pl->m_IsStereoShader) {
			continue;
		}

		Commands::xxSetShader* pShader = (Commands::xxSetShader*)pl->m_Shader.get();
		if (!pShader) {
			continue;
		}

		ShaderWrapper* pShaderWrapper = 0;
		InitWrapper(pShader->hShader_, pShaderWrapper);
		if (pShaderWrapper)
		{
			ProjectionShaderData*   pProjData = pShaderWrapper->m_ProjectionData.matrixData.cb.empty() ? 0 : &pShaderWrapper->m_ProjectionData;
			UnprojectionShaderData* pUnprojData = pShaderWrapper->m_UnprojectionData.matrixData.cb.empty() ? 0 : &pShaderWrapper->m_UnprojectionData;
			ModifiedShaderData*		pModData = pShaderWrapper->IsShaderModified() ? &pShaderWrapper->m_ModifiedShaderData : 0;

			if (pModData)
			{
				ShaderMatrixView.reset( new Commands::ChangeShaderMatrixView(SHADER_PIPELINE(i), pShaderWrapper->m_CRC32, 0, pModData, 0) );
				break;
			}
			else if (pProjData || pUnprojData)
			{
				ShaderMatrixView.reset( new Commands::ChangeShaderMatrixView(SHADER_PIPELINE(i), pShaderWrapper->m_CRC32, pProjData, 0, pUnprojData) );
				break;
			}
		}
	}

	// Instruct driver to setup left,right or mono matrices(resources)
	bool bStereoDraw = false;
	if ( ShaderMatrixView.get() != 0 ) // matrices for modification not found in shader
	{
		bStereoDraw = true;
		if (gInfo.DrawType == 0)
		{
			DEBUG_TRACE3(_T("\tMono draw (API)\n"));
			bStereoDraw = false;
		}
		else if (gInfo.DrawType == 2 && gInfo.DX10MonoNonIndexedDraw && drawType == 0)	// non-indexed draw calls instructed to be mono
			bStereoDraw = false;
	}

	if (bStereoDraw)
	{
		AddStereoCommand(ShaderMatrixView);

#ifndef FINAL_RELEASE
		if (drawType == 0)
			m_pWrapper->m_StereoDrawCount++;
		else if (drawType & drAuto)
			m_pWrapper->m_StereoDrawAutoCount++;
		else 
		{
			if (drawType & drIndexed)
				m_pWrapper->m_StereoDrawIndexedCount++;
			if (drawType & drInstanced)
				m_pWrapper->m_StereoDrawInstancedCount++;
			if (drawType & drIndirect)
				m_pWrapper->m_StereoDrawIndirectCount++;
		}
#endif
	}
	else {
		AddStereoCommand(DeviceViewMono);
	}

	// setup constant buffers and resources, will be chosen necessary right, left or mono handles
	bool bUsedStereoResources = false;
	for (int i = 0; i<SP_CS; ++i) // GS, DS, VS, HS, PS
	{
		ShaderPipelineStates* pl = m_pWrapper->m_DeviceState.GetShaderPipeline(SHADER_PIPELINE(i));
		if (!pl) {
			continue;
		}

		// we may need to reset all the constant buffers, because view changed or shader matrices changed
		AddStereoCommand( pl->m_ConstantBuffers->Clone() );

		// mark used but not updated (have no update commands in this CB) constant buffers to effectively cache resource commands
		// heavy and sophisticated logic, wants refactoring
		for (size_t j = 0; j < pl->m_ConstantBuffers->Values_.size(); ++j)
		{
			D3D10DDI_HRESOURCE resource = pl->m_ConstantBuffers->Values_[j];
			if (resource.pDrvPrivate != NULL && m_ResourcesUpdated.count(resource) == 0) {
				m_ResourcesUsedNotUpdated.insert(resource);
			}
		}

		// check whether stereo resources used, so we need to setup stereo type to the current RT, even if draw is mono
		if ( pl->m_ShaderResources->IsUsedStereoResources(m_pWrapper) ) {
			bUsedStereoResources = true;
		}
	}

	// setup modified shader CBs if needed
	{
		Commands::xxSetShader* pShader = (Commands::xxSetShader*)m_pWrapper->m_DeviceState.GetShaderPipeline(SP_VS)->m_Shader.get();
		ShaderWrapper* pShaderWrapper = 0;
		InitWrapper(pShader->hShader_, pShaderWrapper);	
		if ( pShaderWrapper && pShaderWrapper->IsShaderModified() ) {
			AddModifiedShaderCBs(pShaderWrapper);
		}
	}

	// add draw command at last
	AddStereoCommand(pCmd_, true);

	// restore left & right view
	if (!bStereoDraw) {
		AddStereoCommand(DeviceViewLeft, DeviceViewRight);
	}

	// make RT resources stereo if stereo draw performed. RT resources may become mono due to other SetDestResourceTypeCommand call, 
	// so call this each time stereo draw whether stereo draw occurs
	if (bUsedStereoResources || bStereoDraw) 
	{
		Commands::TCmdPtr pSetRTs = m_pWrapper->m_DeviceState.m_OMRenderTargets->Clone();
		AddSetDestResourceTypeCommand(pSetRTs.get(), TT_STEREO);
	}

	m_bHaveDrawCommand = true;
#ifndef FINAL_RELEASE
	++m_BufferDrawCount;
	++m_pWrapper->m_DrawCount;
	pCmd_->m_bStereoDraw = bStereoDraw;
#endif
	if (USE_UM_PRESENTER && m_pWrapper->CheckFlush())
	{
		m_pWrapper->m_bFlushCommands = true;
		m_WantProcess = true;
	}
}

void StereoCommandBuffer::AddDispatchCommand( Commands::DispatchBase11_0* pCmd_, int drawType )
{
	bool bStereoDispatch = true; // for now, always stereo
	if (bStereoDispatch)
	{
		//AddStereoCommand(ShaderMatrixView);
	}
	else {
		AddStereoCommand(DeviceViewMono);
	}

	// setup constant buffers and resources, will be chosen necessary right, left or mono handles
	bool bUsedStereoResources = false;
	ShaderPipelineStates* pl = m_pWrapper->m_DeviceState.GetShaderPipeline(SP_CS);
	if (pl)
	{
		// we may need to reset all the constant buffers, because view changed or shader matrices changed
		AddStereoCommand( pl->m_ConstantBuffers->Clone() );

		// mark used but not updated (have no update commands in this CB) constant buffers to effectively cache resource commands
		// heavy and sophisticated logic, wants refactoring
		for (size_t j = 0; j < pl->m_ConstantBuffers->Values_.size(); ++j)
		{
			D3D10DDI_HRESOURCE resource = pl->m_ConstantBuffers->Values_[j];
			if (resource.pDrvPrivate != NULL && m_ResourcesUpdated.count(resource) == 0) {
				m_ResourcesUsedNotUpdated.insert(resource);
			}
		}

		// check whether stereo resources used, so we need to setup stereo type to the current RT, even if draw is mono
		if ( pl->m_ShaderResources->IsUsedStereoResources(m_pWrapper) ) {
			bUsedStereoResources = true;
		}
	}

	// add dispatch command at last
	AddStereoCommand(pCmd_, true);

	// restore left & right view
	if (!bStereoDispatch) {
		AddStereoCommand(DeviceViewLeft, DeviceViewRight);
	}

	// make UAVs resources stereo if stereo draw performed. RT resources may become mono due to other SetDestResourceTypeCommand call, 
	// so call this each time stereo dispatch whether stereo draw occurs
	if (bUsedStereoResources || bStereoDispatch) 
	{
		//Commands::TCmdPtr pSetUAVs = m_pWrapper->m_DeviceState.m_CsUnorderedAccessViews;
		//AddSetDestResourceTypeCommand(pSetUAVs.get(), TT_STEREO);
	}
	
#ifndef FINAL_RELEASE
	pCmd_->m_bStereoDispatch = bStereoDispatch;
#endif
}

//////////////////////////////////////////////////////////////////////////

#ifndef FINAL_RELEASE
void StereoCommandBuffer::ClearStatistics()
{
	CommandBuffer::ClearStatistics();
	m_MapResourceSizeCount.clear();
	m_ViewChangeCount = 0;
	std::fill(m_FinalizeCommandCount, m_FinalizeCommandCount + idTotal, 0);
}

void StereoCommandBuffer::DumpStatistics()
{
	CommandBuffer::DumpStatistics();
	
	std::ostringstream ss;	
	ss << "Finalize command statistics:\n";
	{
		size_t finalizeCommandCount = std::accumulate(m_FinalizeCommandCount, m_FinalizeCommandCount + idTotal, 0);

		std::vector<std::pair<int, size_t>> values(idTotal);
		for (size_t i = 0; i<idTotal; ++i) 
		{
			values[i].first = i;
			values[i].second = m_FinalizeCommandCount[i];
		}
		std::sort(values.begin(), values.end(), [](std::pair<int, size_t> i, std::pair<int, size_t> j ){ return i.second > j.second; });
	
		for (size_t i = 0; i<idTotal; ++i) 
		{
			int ind = values[i].first;
			size_t val = values[i].second;
			if (val == 0)
				break;
			size_t percent = size_t(100.0f * val / finalizeCommandCount);
			ss << "\t" << CommandIDToString( CommandsId(ind) ) << ": " << val << "(" << percent << "%)\n";
		}
	}

	ss << "Unbuffered map commands:\n";
	{
		size_t mapCommandSizeCount = 0;
		std::for_each( m_MapResourceSizeCount.begin(),
					   m_MapResourceSizeCount.end(), 
					   [&mapCommandSizeCount](std::pair<size_t, size_t> elem) { mapCommandSizeCount += elem.second; } );
		for (auto it = m_MapResourceSizeCount.begin(); it != m_MapResourceSizeCount.end(); ++it)
		{
			size_t percent = size_t(100.0f * it->second / mapCommandSizeCount);
			ss << "\t" << it->first << ": " << it->second << "(" << percent << "%)\n";
		}

		ss << "Draw count: " << m_pWrapper->m_DrawCount << std::endl
		   << "View change count: " << m_ViewChangeCount << "(" << size_t(100.0f * m_ViewChangeCount / m_pWrapper->m_DrawCount) << "%)\n";
	}

	ZLOG_MESSAGE( zlog::SV_MESSAGE, _T("%S"), ss.str().c_str() );
}
#endif // FINAL_RELEASE

//////////////////////////////////////////////////////////////////////////

void StereoCommandBuffer::AddFinalizeCommand( Commands::Command* pCmd_, bool WantFinalize )
{
	_ASSERT(m_FinalizeCommand == NULL);
	m_FinalizeCommand = pCmd_;
	m_WantProcess = true;
	m_WantFinalize = WantFinalize;
#ifndef FINAL_RELEASE
	++m_FinalizeCommandCount[pCmd_->CommandId];
#endif
}

void StereoCommandBuffer::AddCommand( Commands::xxSetRenderTargets* pCmd_ )
{
	bool bUsedStereoResources = pCmd_->IsUsedStereoResources(m_pWrapper);
	if ( bUsedStereoResources && gInfo.RenderTargetBuffering ) 
	{
		// next render target is stereo, so we can cache it
		pCmd_->UpdateDeviceState(m_pWrapper, &m_pWrapper->m_DeviceState);
		AddStereoCommand(pCmd_, true);
	}
	else {
		AddFinalizeCommand(pCmd_, !bUsedStereoResources);
	}
}

void StereoCommandBuffer::AddCommand( Commands::CsSetUnorderedAccessViews11_0* pCmd_ )
{
	AddFinalizeCommand(pCmd_, false);
}

void StereoCommandBuffer::AddCommand( Commands::xxSetConstantBuffers* pCmd_ )
{
	// we gather shader resources from last draw command
	SHADER_PIPELINE	sp = pCmd_->ShaderPipeline_;

	if (m_StatesForRestoration.count(pCmd_->CommandId) == 0) {
		m_StatesForRestoration[pCmd_->CommandId] = m_pWrapper->m_DeviceState.GetLastCommand(pCmd_->CommandId, true);
	}
	pCmd_->UpdateDeviceState(m_pWrapper, &m_pWrapper->m_DeviceState);
}

//////////////////////////////////////////////////////////////////////////
#pragma region Resource

void StereoCommandBuffer::AddCommand( Commands::xxUpdateSubresourceUP* pCmd_ )
{
#if ENABLE_BUFFERING_USU
	m_ResourcesUpdated.insert(pCmd_->hDstResource_);

	bool needRestoration =  (m_ResourcesForRestoration.count(pCmd_->hDstResource_) == 0)   // not marked for restoration
							&& (m_ResourcesUsedNotUpdated.count(pCmd_->hDstResource_) != 0);  // used by some draw command earlier and not updated before use

	// If we find complete resource restoration command, we can cache update command and restore buffer
	// for right view after execution of resource update commands
	ResourceWrapper* pRes;
	InitWrapper(pCmd_->hDstResource_, pRes);
	if ( (!needRestoration || pRes->m_pRestoreCmd) && pRes->m_EnableCaching )
	{
		// user may dirty data used to update resource, cache it
		if ( !pCmd_->MakeInternalDataCopy(m_pWrapper) ) 
		{
			DEBUG_TRACE3(_T("USU: Can't allocate memory to cache command\n"));
			AddFinalizeCommand( pCmd_ );
			return;
		}

		AddCommandThatMayUseStereoResources(pCmd_);

		// add restoration command if we don't already added it to prologue and it is not dummy command
		if (needRestoration) {
			m_ResourcesForRestoration[pCmd_->hDstResource_] = pRes->m_pRestoreCmd;
		}
	}
	else
	{
		if (!pRes->m_pRestoreCmd) {
			DEBUG_TRACE3(_T("USU: No restore command\n"));
		} else if (needRestoration) {
			DEBUG_TRACE3(_T("USU: Resource needs restoration\n"));
		}
		AddFinalizeCommand( pCmd_ );
	}
#else
	AddFinalizeCommand( pCmd_ );
#endif
}

void StereoCommandBuffer::AddStereoCommand(const Commands::TCmdPtr& pCmd1_, const Commands::TCmdPtr& pCmd2_)
{
	FlushRestBuffer();
	m_LeftBuffer.push_back(pCmd1_);
	m_RightBuffer.push_back(pCmd2_);
}

void StereoCommandBuffer::AddStereoCommand(const Commands::TCmdPtr& pCmd_, bool require)
{
	m_RestBuffer.push_back(pCmd_);
	if (require) {
		FlushRestBuffer();
	}
}

void StereoCommandBuffer::FlushRestBuffer()
{
	if ( !m_RestBuffer.empty() )
	{
		// copy delayed commands to left and right buffer
		std::copy( m_RestBuffer.begin(), m_RestBuffer.end(), std::back_inserter(m_LeftBuffer) );
		std::copy( m_RestBuffer.begin(), m_RestBuffer.end(), std::back_inserter(m_RightBuffer) );
		m_RestBuffer.clear();
	}
}

void StereoCommandBuffer::AddSetDestResourceTypeCommand(Commands::Command* pCmd_, TextureType type)
{
	pCmd_->GetDestResources(m_ResourcesToRestoreType);
	AddStereoCommand( new Commands::SetDestResourceType(pCmd_, type) );
}

void StereoCommandBuffer::AddCommand( Commands::xxMap* pCmd_ )
{
	if ( pCmd_->CommandId == idResourceMap || pCmd_->CommandId == idDynamicResourceMapDiscard ) {
		AddSetDestResourceTypeCommand(pCmd_, TT_MONO);
	}

#if ENABLE_BUFFERING_MAP
	if (pCmd_->UpdateCommand_) {
		pCmd_->Execute(m_pWrapper); // execute faked map that will fill up update cmd data
	}
	else 
	{
#ifndef FINAL_RELEASE
		ResourceWrapper* pWrp;
		InitWrapper(pCmd_->hResource_, pWrp);

		size_t size = pWrp->Size(pCmd_->Subresource_);
		if (m_MapResourceSizeCount.count(size) > 0) {
			++m_MapResourceSizeCount[size];
		}
		else {
			m_MapResourceSizeCount[size] = 1;
		}
#endif
		AddFinalizeCommand(pCmd_);
	}
#else 
	AddFinalizeCommand(pCmd_);
#endif
}

void StereoCommandBuffer::AddCommand( Commands::xxUnmap* pCmd_ )
{
#if ENABLE_BUFFERING_MAP
	// find corresponding map
	ResourceWrapper* pWrp;
	InitWrapper(pCmd_->hResource_, pWrp);
	if (pWrp->m_pCurrentMapCmd && pWrp->m_pCurrentMapCmd->UpdateCommand_) 
	{
		// replace map/unmap command with single UpdateSubresourceUP for convenience and work with it
		AddCommand( pWrp->m_pCurrentMapCmd->UpdateCommand_.get() );
	}
	else {
		AddFinalizeCommand(pCmd_);
	}
#else // ENABLE_BUFFERING_MAP
	AddFinalizeCommand( pCmd_ );
#endif // ENABLE_BUFFERING_MAP
}

void StereoCommandBuffer::AddModifiedShaderCBs( ShaderWrapper* pWrp )
{
	D3D10DDI_HRESOURCE hCBView = m_pWrapper->m_pECBuffer->getHResource();
	m_pWrapper->m_pECBuffer->initMonoData(pWrp);
	AddStereoCommand( new Commands::DefaultConstantBufferUpdateSubresourceUP(hCBView, 0, 0, m_pWrapper->m_pECBuffer->m_Data, 0, 0) );
	AddStereoCommand( new Commands::VsSetConstantBuffers(pWrp->m_ModifiedShaderData.CBIndex, 1, &hCBView) );
}
#pragma endregion
