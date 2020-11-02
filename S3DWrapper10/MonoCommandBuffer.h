#ifndef __S3D_WRAPPER_DIRECT3D_10_MONO_COMMAND_BUFFER_H__
#define __S3D_WRAPPER_DIRECT3D_10_MONO_COMMAND_BUFFER_H__

#include "CommandBuffer.h"
#include "Commands\CommandSet.h"

class MonoCommandBuffer : public CommandBuffer
{
public:
								MonoCommandBuffer		();
								~MonoCommandBuffer		();

	void						FlushAll				();

	//////////////////////////////////////////////////////////////////////////
	void AddCommand( Commands::Command* pCmd_ )									{ AddMonoCommand( pCmd_ ); }	

	//////////////////////////////////////////////////////////////////////////
	// Change device state G1
	void AddCommand( Commands::IaSetIndexBuffer* pCmd_ )						{ AddChangeStateCommand( pCmd_ ); }
	void AddCommand( Commands::IaSetInputLayout* pCmd_ )						{ AddChangeStateCommand( pCmd_ ); }
	void AddCommand( Commands::IaSetTopology* pCmd_ )							{ AddChangeStateCommand( pCmd_ ); }
	void AddCommand( Commands::IaSetVertexBuffers* pCmd_ )						{ AddChangeStateCommand( pCmd_ ); }
	void AddCommand( Commands::SetBlendState* pCmd_ )							{ AddChangeStateCommand( pCmd_ ); }
	void AddCommand( Commands::SetDepthStencilState* pCmd_ )					{ AddChangeStateCommand( pCmd_ ); }
	void AddCommand( Commands::SetPredication* pCmd_ )							{ AddChangeStateCommand( pCmd_ ); }
	void AddCommand( Commands::SetRasterizerState* pCmd_ )						{ AddChangeStateCommand( pCmd_ ); }
	void AddCommand( Commands::SetScissorRects* pCmd_ )							{ AddChangeStateCommand( pCmd_ ); }
	void AddCommand( Commands::SetTextFilterSize* pCmd_ )						{ AddChangeStateCommand( pCmd_ ); }
	void AddCommand( Commands::SetViewports* pCmd_ )							{ AddChangeStateCommand( pCmd_ ); }
	void AddCommand( Commands::SoSetTargets* pCmd_ )							{ AddChangeStateCommand( pCmd_ ); }
	void AddCommand( Commands::xxSetShader* pCmd_ )								{ AddChangeStateCommand( pCmd_ ); }
	void AddCommand( Commands::xxSetConstantBuffers* pCmd_ )					{ AddChangeStateCommand( pCmd_ ); }
	void AddCommand( Commands::xxSetSamplers* pCmd_ )							{ AddChangeStateCommand( pCmd_ ); }
	void AddCommand( Commands::xxSetShaderResources* pCmd_ )					{ AddChangeStateCommand( pCmd_ ); }

	void AddCommand( Commands::xxSetShaderWithIfaces11_0* pCmd_ )				{ AddChangeStateCommand( pCmd_ ); }
	
	void AddCommand( Commands::xxSetRenderTargets* pCmd_ );
	void AddCommand( Commands::CsSetUnorderedAccessViews11_0* pCmd_ )			{ AddFinalizeCommand( pCmd_ ); }
	
	//////////////////////////////////////////////////////////////////////////		
	// G1	
	void AddCommand( Commands::ClearDepthStencilView* pCmd_ )					{ AddCommandThatMayUseStereoResources( pCmd_, TT_CLEARED ); }
	void AddCommand( Commands::ClearRenderTargetView* pCmd_ )					{ AddCommandThatMayUseStereoResources( pCmd_, TT_CLEARED ); }
	void AddCommand( Commands::GenMips* pCmd_ )									{ AddCommandThatMayUseStereoResources( pCmd_ ); }
	void AddCommand( Commands::ResourceConvert10_1* pCmd_ )						{ AddCommandThatMayUseStereoResources( pCmd_, TT_SETLIKESRC ); }
	void AddCommand( Commands::ResourceConvertRegion10_1* pCmd_ )				{ AddCommandThatMayUseStereoResources( pCmd_, TT_SETLIKESRC ); }
	void AddCommand( Commands::ResourceCopy* pCmd_ )							{ AddCommandThatMayUseStereoResources( pCmd_, TT_SETLIKESRC ); }
	void AddCommand( Commands::ResourceCopyRegion* pCmd_ )						{ AddCommandThatMayUseStereoResources( pCmd_, TT_SETLIKESRC ); }
	void AddCommand( Commands::ResourceReadAfterWriteHazard* pCmd_ )			{ AddCommandThatMayUseStereoResources( pCmd_ ); }
	void AddCommand( Commands::ResourceResolveSubresource* pCmd_ )				{ AddCommandThatMayUseStereoResources( pCmd_, TT_SETLIKESRC ); }
	void AddCommand( Commands::ShaderResourceViewReadAfterWriteHazard* pCmd_ )	{ AddCommandThatMayUseStereoResources( pCmd_ ); }

	void AddCommand( Commands::ClearUnorderedAccessViewUint11_0* pCmd_ )		{ AddCommandThatMayUseStereoResources( pCmd_ ); }
	void AddCommand( Commands::ClearUnorderedAccessViewFloat11_0* pCmd_ )		{ AddCommandThatMayUseStereoResources( pCmd_ ); }
	void AddCommand( Commands::CopyStructureCount11_0* pCmd_ )					{ AddCommandThatMayUseStereoResources( pCmd_ ); }
	void AddCommand( Commands::SetResourceMinLOD11_0* pCmd_ )					{ AddCommandThatMayUseStereoResources( pCmd_ ); }

	//////////////////////////////////////////////////////////////////////////		
	// G1	
	void AddCommand( Commands::Draw* pCmd_ )									{ AddDrawCommand( pCmd_ ); }
	void AddCommand( Commands::DrawAuto* pCmd_ )								{ AddDrawCommand( pCmd_ ); }
	void AddCommand( Commands::DrawIndexed* pCmd_ )								{ AddDrawCommand( pCmd_ ); }
	void AddCommand( Commands::DrawIndexedInstanced* pCmd_ )					{ AddDrawCommand( pCmd_ ); }
	void AddCommand( Commands::DrawInstanced* pCmd_ )							{ AddDrawCommand( pCmd_ ); }

	void AddCommand( Commands::DrawIndexedInstancedIndirect11_0* pCmd_ )		{ AddDrawCommand( pCmd_ ); }
	void AddCommand( Commands::DrawInstancedIndirect11_0* pCmd_ )				{ AddDrawCommand( pCmd_ ); }

	void AddCommand( Commands::Dispatch11_0* pCmd_ )							{ AddDispatchCommand( pCmd_ ); }
	void AddCommand( Commands::DispatchIndirect11_0* pCmd_ )					{ AddDispatchCommand( pCmd_ ); }

	//////////////////////////////////////////////////////////////////////////
	
	void AddCommand( Commands::CreateResource* pCmd_ )							{ AddMonoCommand(pCmd_, true); }
	void AddCommand( Commands::CreateResource11_0* pCmd_ )						{ AddMonoCommand(pCmd_, true); }
	void AddCommand( Commands::xxUpdateSubresourceUP* pCmd_ )					{ AddMonoCommand(pCmd_, true); }
	void AddCommand( Commands::xxMap* pCmd_ );
	void AddCommand( Commands::xxUnmap* pCmd_ );

	//////////////////////////////////////////////////////////////////////////
	// DXGI G2
	void AddCommand( Commands::Blt* pCmd_ )										{ AddFinalizeCommand(pCmd_, true); }
	void AddCommand( Commands::Present* pCmd_ )									{ AddFinalizeCommand(pCmd_, true); }

protected:

	void						Execute					();
	void						ExecuteFinalizeCommand	();

	void						AddChangeStateCommand	( Commands::Command* pCmd_ );
	void						AddCommandThatMayUseStereoResources( Commands::Command* pCmd_, int type = -1 );
	void						AddDrawCommand			( Commands::DrawBase* pCmd_ );
	void						AddDispatchCommand		( Commands::DispatchBase11_0* pCmd_ );
};

#endif//__S3D_WRAPPER_DIRECT3D_10_MONO_COMMAND_BUFFER_H__
