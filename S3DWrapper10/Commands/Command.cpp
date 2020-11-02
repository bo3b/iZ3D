#include "stdafx.h"
#include "Command.h"
#include <math.h>
#include "CommandSet.h"
#include "..\streamer\codegenerator.h"


namespace Commands
{
	VIEWINDEX						Command::CurrentView_ = VIEWINDEX_MONO;
	DWORD							Command::CurrentShaderCRC32_ = 0;
	const ProjectionShaderData*		Command::CurrentShaderData_[SP_COUNT];
	const UnprojectionShaderData*	Command::CurrentPixelShaderData_[SP_COUNT];
	const ModifiedShaderData*		Command::CurrentModifiedShaderData_[SP_COUNT];

	//MemoryManager Command::m_MemoryPool;

#ifndef FINAL_RELEASE
	void *Command::Creates()
	{
		return (void *)-1;
	}

	std::vector<void *> Command::DependsOn()
	{
		return std::vector<void *>();
	}

	std::string Command::GetBuiltCommand(CDumpState *ds,std::string &sinit,std::string &sdecl,std::string &sglob,std::set<UINT> &skipped)
	{
		for(int i = 0;i < output_target_count;i++)
		{
			_output[i].clear();
		}

		BuildCommand(ds);

		if (!empty_output())
		{
			BOOL bEmpty = TRUE;

			for(int i = (int)draw;i <= (int)init;i++)
			{
				if (_output[i] != "")
				{
					char buf[128];
					if (i != draw || !ds->NeedSetEidCommands())
						sprintf(buf,"\n// [%d] %s\n",EventID_,CommandIDToString(CommandId));
					else
						sprintf(buf,"\nSetEID(%d); //%s\n",EventID_,CommandIDToString(CommandId));
					_output[i] = buf + _output[i];

					bEmpty = FALSE;
				}
			}

			if (bEmpty)	skipped.insert(CommandId);
		}

		sinit = _output[init];
		sdecl = _output[decl];
		sglob = _output[glob];
		if (sdecl.empty() && !sglob.empty())
			sdecl = "extern " + sglob;

		return _output[draw];
	}

#define OUTPUT_BODY(param) \
	const unsigned bufsize = 10000; \
	static char buffer[bufsize]; \
	va_list args; \
	va_start(args,format); \
	vsprintf_s(buffer,bufsize,format,args); \
	va_end(args); \
	_output[param] += buffer;

	void Command::output(output_target targ,const char *format,...)
	{
		OUTPUT_BODY(targ);
		_output[targ] += "\n";
	}

	void Command::output(const char *format,...)
	{
		OUTPUT_BODY(_output_target);
		_output[_output_target] += "\n";
	}

	void Command::output_(output_target targ,const char *format,...)
	{
		OUTPUT_BODY(targ);
	}

	void Command::output_(const char *format,...)
	{
		OUTPUT_BODY(_output_target);
	}
#endif // FINAL_RELEASE

	void Command::SendToCodeGenerator( CodeGenerator *cg )
	{ 
		cg->AppendCommand(this); 
	};

#ifndef FINAL_RELEASE
	CommandsId		Command::LastCommandId_ = idNone;
	static UINT g_CurrentEventID = 0;
	UINT Command::GetEventID()
	{
		return g_CurrentEventID++;
	}

	double Command::GetTimeMilliseconds()
	{
		static double reverse_freq = 0.0;
		LARGE_INTEGER time; 

		if( !reverse_freq )
		{
			LARGE_INTEGER freq;
			QueryPerformanceFrequency( &freq );
			reverse_freq = 1000.0 / (double)freq.QuadPart; 
		}
		QueryPerformanceCounter( &time );
		return (double)time.QuadPart * reverse_freq;
	}

	Command::pfCreateCMDFunc Command::m_CreateCMDFuncs[] = {};	


	Command* Command::CreateCommandFromCID( int _nCID )
	{		
		_ASSERT( _nCID >= 0 && _nCID < idTotal );
		pfCreateCMDFunc Create = m_CreateCMDFuncs[ _nCID ];
		if ( Create )
			return Create();
		return NULL;
	}

	void Command::InitCommandsCreateFunc()
	{	
		REGISTER_CREATEFUNC( Blt );
		REGISTER_CREATEFUNC( Present );
		REGISTER_CREATEFUNC( GetGammaCaps );
		REGISTER_CREATEFUNC( SetDisplayMode );
		REGISTER_CREATEFUNC( SetResourcePriority );
		REGISTER_CREATEFUNC( QueryResourceResidency );
		REGISTER_CREATEFUNC( RotateResourceIdentities );
		REGISTER_CREATEFUNC( ResolveSharedResource );

		//D3D10Functions
		REGISTER_CREATEFUNC( DefaultConstantBufferUpdateSubresourceUP );
		REGISTER_CREATEFUNC( VsSetConstantBuffers );
		REGISTER_CREATEFUNC( PsSetShaderResources );
		REGISTER_CREATEFUNC( PsSetShader );
		REGISTER_CREATEFUNC( PsSetSamplers );
		REGISTER_CREATEFUNC( VsSetShader );
		REGISTER_CREATEFUNC( DrawIndexed );
		REGISTER_CREATEFUNC( Draw );
		REGISTER_CREATEFUNC( DynamicIABufferMapNoOverwrite );
		REGISTER_CREATEFUNC( DynamicIABufferUnmap );
		REGISTER_CREATEFUNC( DynamicConstantBufferMapDiscard );
		REGISTER_CREATEFUNC( DynamicIABufferMapDiscard );
		REGISTER_CREATEFUNC( DynamicConstantBufferUnmap );
		REGISTER_CREATEFUNC( PsSetConstantBuffers );
		REGISTER_CREATEFUNC( IaSetInputLayout );
		REGISTER_CREATEFUNC( IaSetVertexBuffers );
		REGISTER_CREATEFUNC( IaSetIndexBuffer );
		REGISTER_CREATEFUNC( DrawIndexedInstanced );
		REGISTER_CREATEFUNC( DrawInstanced );
		REGISTER_CREATEFUNC( DynamicResourceMapDiscard );
		REGISTER_CREATEFUNC( DynamicResourceUnmap );
		REGISTER_CREATEFUNC( GsSetConstantBuffers );
		REGISTER_CREATEFUNC( GsSetShader );
		REGISTER_CREATEFUNC( IaSetTopology );
		REGISTER_CREATEFUNC( StagingResourceMap );
		REGISTER_CREATEFUNC( StagingResourceUnmap );
		REGISTER_CREATEFUNC( VsSetShaderResources );
		REGISTER_CREATEFUNC( VsSetSamplers );
		REGISTER_CREATEFUNC( GsSetShaderResources );
		REGISTER_CREATEFUNC( GsSetSamplers );
		REGISTER_CREATEFUNC( SetRenderTargets );
		REGISTER_CREATEFUNC( ShaderResourceViewReadAfterWriteHazard );
		REGISTER_CREATEFUNC( ResourceReadAfterWriteHazard );
		REGISTER_CREATEFUNC( SetBlendState );
		REGISTER_CREATEFUNC( SetDepthStencilState );
		REGISTER_CREATEFUNC( SetRasterizerState );
		REGISTER_CREATEFUNC( QueryEnd );
		REGISTER_CREATEFUNC( QueryBegin );
		REGISTER_CREATEFUNC( ResourceCopyRegion );
		REGISTER_CREATEFUNC( ResourceUpdateSubresourceUP );
		REGISTER_CREATEFUNC( SoSetTargets );
		REGISTER_CREATEFUNC( DrawAuto );
		REGISTER_CREATEFUNC( SetViewports );
		REGISTER_CREATEFUNC( SetScissorRects );
		REGISTER_CREATEFUNC( ClearRenderTargetView );
		REGISTER_CREATEFUNC( ClearDepthStencilView );
		REGISTER_CREATEFUNC( SetPredication );
		REGISTER_CREATEFUNC( QueryGetData );
		REGISTER_CREATEFUNC( Flush );
		REGISTER_CREATEFUNC( GenMips );
		REGISTER_CREATEFUNC( ResourceCopy );
		REGISTER_CREATEFUNC( ResourceResolveSubresource );
		REGISTER_CREATEFUNC( ResourceMap );
		REGISTER_CREATEFUNC( ResourceUnmap );
		REGISTER_CREATEFUNC( ResourceIsStagingBusy );
		REGISTER_CREATEFUNC( RelocateDeviceFuncs );
		REGISTER_CREATEFUNC( CalcPrivateResourceSize );
		REGISTER_CREATEFUNC( CalcPrivateOpenedResourceSize );
		REGISTER_CREATEFUNC( CreateResource );
		REGISTER_CREATEFUNC( OpenResource );
		REGISTER_CREATEFUNC( DestroyResource );
		REGISTER_CREATEFUNC( CalcPrivateShaderResourceViewSize );
		REGISTER_CREATEFUNC( CreateShaderResourceView );
		REGISTER_CREATEFUNC( DestroyShaderResourceView );
		REGISTER_CREATEFUNC( CalcPrivateRenderTargetViewSize );
		REGISTER_CREATEFUNC( CreateRenderTargetView );
		REGISTER_CREATEFUNC( DestroyRenderTargetView );
		REGISTER_CREATEFUNC( CalcPrivateDepthStencilViewSize );
		REGISTER_CREATEFUNC( CreateDepthStencilView );
		REGISTER_CREATEFUNC( DestroyDepthStencilView );
		REGISTER_CREATEFUNC( CalcPrivateElementLayoutSize );
		REGISTER_CREATEFUNC( CreateElementLayout );
		REGISTER_CREATEFUNC( DestroyElementLayout );
		REGISTER_CREATEFUNC( CalcPrivateBlendStateSize );
		REGISTER_CREATEFUNC( CreateBlendState );
		REGISTER_CREATEFUNC( DestroyBlendState );
		REGISTER_CREATEFUNC( CalcPrivateDepthStencilStateSize );
		REGISTER_CREATEFUNC( CreateDepthStencilState );
		REGISTER_CREATEFUNC( DestroyDepthStencilState );
		REGISTER_CREATEFUNC( CalcPrivateRasterizerStateSize );
		REGISTER_CREATEFUNC( CreateRasterizerState );
		REGISTER_CREATEFUNC( DestroyRasterizerState );
		REGISTER_CREATEFUNC( CalcPrivateShaderSize );
		REGISTER_CREATEFUNC( CreateVertexShader );
		REGISTER_CREATEFUNC( CreateGeometryShader );
		REGISTER_CREATEFUNC( CreatePixelShader );
		REGISTER_CREATEFUNC( CalcPrivateGeometryShaderWithStreamOutput );
		REGISTER_CREATEFUNC( CreateGeometryShaderWithStreamOutput );
		REGISTER_CREATEFUNC( DestroyShader );
		REGISTER_CREATEFUNC( CalcPrivateSamplerSize );
		REGISTER_CREATEFUNC( CreateSampler );
		REGISTER_CREATEFUNC( DestroySampler );
		REGISTER_CREATEFUNC( CalcPrivateQuerySize );
		REGISTER_CREATEFUNC( CreateQuery );
		REGISTER_CREATEFUNC( DestroyQuery );
		REGISTER_CREATEFUNC( CheckFormatSupport );
		REGISTER_CREATEFUNC( CheckMultisampleQualityLevels );
		REGISTER_CREATEFUNC( CheckCounterInfo );
		REGISTER_CREATEFUNC( CheckCounter );
		REGISTER_CREATEFUNC( DestroyDevice );
		REGISTER_CREATEFUNC( SetTextFilterSize );

		//D3D10_1Functions
		REGISTER_CREATEFUNC( RelocateDeviceFuncs10_1 );
		REGISTER_CREATEFUNC( CalcPrivateShaderResourceViewSize10_1 );
		REGISTER_CREATEFUNC( CreateShaderResourceView10_1 );
		REGISTER_CREATEFUNC( CalcPrivateBlendStateSize10_1 );
		REGISTER_CREATEFUNC( CreateBlendState10_1 );
		REGISTER_CREATEFUNC( ResourceConvert10_1 );
		REGISTER_CREATEFUNC( ResourceConvertRegion10_1 );

		//D3D11_0Functions
		REGISTER_CREATEFUNC( AbandonCommandList11_0 );
		REGISTER_CREATEFUNC( CalcDeferredContextHandleSize11_0 );
		REGISTER_CREATEFUNC( CalcPrivateCommandListSize11_0 );
		REGISTER_CREATEFUNC( CalcPrivateDeferredContextSize11_0 );
		REGISTER_CREATEFUNC( CalcPrivateDepthStencilViewSize11_0 );
		REGISTER_CREATEFUNC( CalcPrivateGeometryShaderWithStreamOutput11_0 );
		REGISTER_CREATEFUNC( CalcPrivateResourceSize11_0 );
		REGISTER_CREATEFUNC( CalcPrivateShaderResourceViewSize11_0 );
		REGISTER_CREATEFUNC( CalcPrivateTessellationShaderSize11_0 );
		REGISTER_CREATEFUNC( CalcPrivateUnorderedAccessViewSize11_0 );
		REGISTER_CREATEFUNC( CheckDeferredContextHandleSizes11_0 );
		REGISTER_CREATEFUNC( ClearUnorderedAccessViewFloat11_0 );
		REGISTER_CREATEFUNC( ClearUnorderedAccessViewUint11_0 );
		REGISTER_CREATEFUNC( CommandListExecute11_0 );
		REGISTER_CREATEFUNC( CopyStructureCount11_0 );
		REGISTER_CREATEFUNC( CreateCommandList11_0 );
		REGISTER_CREATEFUNC( CreateComputeShader11_0 );
		REGISTER_CREATEFUNC( CreateDeferredContext11_0 );
		REGISTER_CREATEFUNC( CreateDepthStencilView11_0 );
		REGISTER_CREATEFUNC( CreateDomainShader11_0 );
		REGISTER_CREATEFUNC( CreateGeometryShaderWithStreamOutput11_0 );
		REGISTER_CREATEFUNC( CreateHullShader11_0 );
		REGISTER_CREATEFUNC( CreateResource11_0 );
		REGISTER_CREATEFUNC( CreateShaderResourceView11_0 );
		REGISTER_CREATEFUNC( CreateUnorderedAccessView11_0 );
		REGISTER_CREATEFUNC( CsSetConstantBuffers11_0 );
		REGISTER_CREATEFUNC( CsSetSamplers11_0 );
		REGISTER_CREATEFUNC( CsSetShader11_0 );
		REGISTER_CREATEFUNC( CsSetShaderResources11_0 );
		REGISTER_CREATEFUNC( CsSetShaderWithIfaces11_0 );
		REGISTER_CREATEFUNC( CsSetUnorderedAccessViews11_0 );
		REGISTER_CREATEFUNC( DestroyCommandList11_0 );
		REGISTER_CREATEFUNC( DestroyUnorderedAccessView11_0 );
		REGISTER_CREATEFUNC( Dispatch11_0 );
		REGISTER_CREATEFUNC( DispatchIndirect11_0 );
		REGISTER_CREATEFUNC( DrawIndexedInstancedIndirect11_0 );
		REGISTER_CREATEFUNC( DrawInstancedIndirect11_0 );
		REGISTER_CREATEFUNC( DsSetConstantBuffers11_0 );
		REGISTER_CREATEFUNC( DsSetSamplers11_0 );
		REGISTER_CREATEFUNC( DsSetShader11_0 );
		REGISTER_CREATEFUNC( DsSetShaderResources11_0 );
		REGISTER_CREATEFUNC( DsSetShaderWithIfaces11_0 );
		REGISTER_CREATEFUNC( GsSetShaderWithIfaces11_0 );
		REGISTER_CREATEFUNC( HsSetConstantBuffers11_0 );
		REGISTER_CREATEFUNC( HsSetSamplers11_0 );
		REGISTER_CREATEFUNC( HsSetShader11_0 );
		REGISTER_CREATEFUNC( HsSetShaderResources11_0 );
		REGISTER_CREATEFUNC( HsSetShaderWithIfaces11_0 );
		REGISTER_CREATEFUNC( PsSetShaderWithIfaces11_0 );
		REGISTER_CREATEFUNC( RelocateDeviceFuncs11_0 );
		REGISTER_CREATEFUNC( SetRenderTargets11_0 );
		REGISTER_CREATEFUNC( SetResourceMinLOD11_0 );
		REGISTER_CREATEFUNC( VsSetShaderWithIfaces11_0 );

		REGISTER_CREATEFUNC( ChangeDeviceView );
		REGISTER_CREATEFUNC( ChangeShaderMatrixView );
	}

#endif

	void Command::Dump(std::ostream& os) const 
	{ 
		os << CommandIDToString(CommandId); 
	}

}
