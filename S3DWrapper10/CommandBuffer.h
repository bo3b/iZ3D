#ifndef __S3D_WRAPPER_DIRECT3D_10_COMMAND_BUFFER_H__
#define __S3D_WRAPPER_DIRECT3D_10_COMMAND_BUFFER_H__

#include "Commands\Command.h"
#include "ResourceWrapper.h"
#include <deque>

namespace Commands
{
	// CalcPrivateDataSize G2
	class CalcPrivateBlendStateSize;
	class CalcPrivateBlendStateSize10_1;
	class CalcPrivateDepthStencilStateSize;
	class CalcPrivateDepthStencilViewSize;
	class CalcPrivateElementLayoutSize;
	class CalcPrivateGeometryShaderWithStreamOutput;
	class CalcPrivateOpenedResourceSize;
	class CalcPrivateQuerySize;
	class CalcPrivateRasterizerStateSize;
	class CalcPrivateRenderTargetViewSize;
	class CalcPrivateResourceSize;
	class CalcPrivateSamplerSize;
	class CalcPrivateShaderResourceViewSize;
	class CalcPrivateShaderResourceViewSize10_1;
	class CalcPrivateShaderSize;
	class CalcPrivateCommandListSize11_0;
	class CalcPrivateDeferredContextSize11_0;
	class CalcPrivateDepthStencilViewSize11_0;
	class CalcPrivateGeometryShaderWithStreamOutput11_0;
	class CalcPrivateResourceSize11_0;
	class CalcPrivateShaderResourceViewSize11_0;
	class CalcPrivateTessellationShaderSize11_0;
	class CalcPrivateUnorderedAccessViewSize11_0;
	class CalcDeferredContextHandleSize11_0;
	// Change device state G1
	class xxSetConstantBuffers;
	class xxSetShaderResources;
	class GsSetConstantBuffers;
	class GsSetSamplers;
	class GsSetShader;
	class GsSetShaderResources;
	class IaSetIndexBuffer;
	class IaSetInputLayout;
	class IaSetTopology;
	class IaSetVertexBuffers;
	class PsSetConstantBuffers;
	class PsSetSamplers;
	class PsSetShader;
	class PsSetShaderResources;
	class SetBlendState;
	class SetDepthStencilState;
	class SetPredication;
	class SetRasterizerState;
	class SetRenderTargets;
	class SetScissorRects;
	class SetTextFilterSize;
	class SetViewports;
	class SoSetTargets;
	class VsSetConstantBuffers;
	class VsSetSamplers;
	class VsSetShader;
	class VsSetShaderResources;
	//
	class SetRenderTargets11_0;
	class HsSetShaderResources11_0;
	class HsSetShader11_0;
	class HsSetSamplers11_0;
	class HsSetConstantBuffers11_0;
	class DsSetShaderResources11_0;
	class DsSetShader11_0;
	class DsSetSamplers11_0;
	class DsSetConstantBuffers11_0;
	class CsSetShader11_0;
	class CsSetShaderResources11_0;
	class CsSetSamplers11_0;
	class CsSetConstantBuffers11_0;
	class CsSetUnorderedAccessViews11_0;
	class PsSetShaderWithIfaces11_0;
	class VsSetShaderWithIfaces11_0;
	class GsSetShaderWithIfaces11_0;
	class HsSetShaderWithIfaces11_0;
	class DsSetShaderWithIfaces11_0;
	class CsSetShaderWithIfaces11_0;
	class SetResourceMinLOD11_0;
	// Check G3
	class CheckCounter;
	class CheckCounterInfo;
	class CheckFormatSupport;
	class CheckMultisampleQualityLevels;
	class CheckDeferredContextHandleSizes11_0;
	// Create G2
	class CreateBlendState;
	class CreateBlendState10_1;
	class CreateDepthStencilState;
	class CreateDepthStencilView;
	class CreateElementLayout;
	class CreateGeometryShader;
	class CreateGeometryShaderWithStreamOutput;
	class CreatePixelShader;
	class CreateQuery;
	class CreateRasterizerState;
	class CreateRenderTargetView;
	class CreateResource;
	class CreateSampler;
	class CreateShaderResourceView;
	class CreateShaderResourceView10_1;
	class CreateVertexShader;
	class OpenResource;

	class CreateCommandList11_0;
	class CreateComputeShader11_0;
	class CreateDeferredContext11_0;
	class CreateDepthStencilView11_0;
	class CreateDomainShader11_0;
	class CreateGeometryShaderWithStreamOutput11_0;
	class CreateHullShader11_0;
	class CreateResource11_0;
	class CreateShaderResourceView11_0;
	class CreateUnorderedAccessView11_0;

	// Destroy G2
	class DestroyBlendState;
	class DestroyDepthStencilState;
	class DestroyDepthStencilView;
	class DestroyDevice;
	class DestroyElementLayout;
	class DestroyQuery;
	class DestroyRasterizerState;
	class DestroyRenderTargetView;
	class DestroyResource;
	class DestroySampler;
	class DestroyShader;
	class DestroyShaderResourceView;

	class DestroyCommandList11_0;
	class DestroyUnorderedAccessView11_0;
	// DXGI G2
	class Blt;
	class GetGammaCaps;
	class Present;
	class QueryResourceResidency;
	class RotateResourceIdentities;
	class SetDisplayMode;
	class SetResourcePriority;
	// G1
	class ClearDepthStencilView;
	class ClearRenderTargetView;
	class Draw;
	class DrawAuto;
	class DrawIndexed;
	class DrawIndexedInstanced;
	class DrawInstanced;
	class GenMips;
	class ResolveSharedResource;
	class ResourceConvert10_1;
	class ResourceConvertRegion10_1;
	class ResourceCopy;
	class ResourceCopyRegion;
	class ResourceReadAfterWriteHazard;
	class ResourceResolveSubresource;
	class ShaderResourceViewReadAfterWriteHazard;	
	// G2
	class Flush;
	class QueryBegin;
	class QueryEnd;
	class QueryGetData;
	class ResourceIsStagingBusy;
	// Map/Unmap G2
	class xxMap;
	class xxUnmap;
	class xxUpdateSubresourceUP;
	class DefaultConstantBufferUpdateSubresourceUP;
	class DynamicConstantBufferMapDiscard;
	class DynamicConstantBufferUnmap;
	class DynamicIABufferMapDiscard;
	class DynamicIABufferMapNoOverwrite;
	class DynamicIABufferUnmap;
	class DynamicResourceMapDiscard;
	class DynamicResourceUnmap;
	class ResourceMap;
	class ResourceUnmap;
	class ResourceUpdateSubresourceUP;
	class StagingResourceMap;
	class StagingResourceUnmap;
	//
	class ClearUnorderedAccessViewUint11_0;
	class ClearUnorderedAccessViewFloat11_0;
	class DrawIndexedInstancedIndirect11_0;
	class DrawInstancedIndirect11_0;
	class CopyStructureCount11_0;
	class Dispatch11_0;
	class DispatchIndirect11_0;
	class AbandonCommandList11_0;
	class CommandListExecute11_0;
	//
	class RelocateDeviceFuncs;
	class RelocateDeviceFuncs10_1;
	class RelocateDeviceFuncs11_0;
	// Advanced
	class ChangeDeviceView;
	class xxSetConstantBuffers;
	class DrawBase;
}

class CommandBuffer
{
protected:
	typedef std::deque<Commands::TCmdPtr> TCommandsVector;

public:
								CommandBuffer		();
								~CommandBuffer		();

	void						Clear();
	
	bool						HaveFinaleCommand		() const { return m_FinalizeCommand != NULL; }
	bool						WantProcess				() const { return m_WantProcess; }
	bool						WantFinalize			() const { return m_WantFinalize; }
	bool						Empty					() const { return m_LeftBuffer.empty(); }

	void						Init					( class D3DDeviceWrapper* pWrapper_ );
	bool						IsSetStereoRT			() const;
	void						DumpDestParam			( const Commands::TCmdPtr& cmd_, bool b2RT, DumpView view = dvBoth );

	void						WriteCommand			( const Commands::TCmdPtr& cmd_ );

#ifndef FINAL_RELEASE
	virtual void				ClearStatistics();
	virtual void				DumpStatistics();
#endif

	/** Start using command buffer. */
	virtual void Begin() { m_bProcessing = true; }

	/** Check whether command buffer ready to process commands (Begin was called and FlushAll not) */
	bool Processing() { return m_bProcessing; } 
	
	/** End using command buffer. */
	virtual void Finalize();

	/** Flush commands */
	virtual void FlushAll() { m_bProcessing = false; }

protected:
	// CalcPrivate...Size, Create..., Destroy..., ...Map..., ...Umap...
	void						AddFinalizeCommand		( Commands::Command* pCmd_, bool WantFinalize = false );
	void						AddMonoCommand			( Commands::Command* pCmd_, bool WantProcess = false );

	bool						GetCommandResources		( const Commands::Command* pCmd_, class ResourceWrapper* &pDstResource, class ResourceWrapper* &pSrcResource );
	void						SetRTTextureType( TextureType tt );
	class D3DDeviceWrapper*		m_pWrapper;
	bool						m_bHaveDrawCommand;
	bool						m_WantProcess;
	bool						m_WantFinalize;
	bool						m_bProcessing;

	TCommandsVector				m_LeftBuffer;
	Commands::TCmdPtr			m_FinalizeCommand;
};

// Trace message and send to WriteStreamer if necessary
void DumpMessage(D3DDeviceWrapper *pWrapper, const char* format, ...);

#endif//__S3D_WRAPPER_DIRECT3D_10_COMMAND_BUFFER_H__
