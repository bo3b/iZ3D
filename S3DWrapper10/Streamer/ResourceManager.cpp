#include "stdafx.h"
#include <WinGDI.h>
#include "XMLStreamer.h"
#include "ResourceManager.h"
#include "../ArchiveFile/archivefile.h"
#include "D3DDeviceWrapper.h"
#include "ShaderWrapper.h"

using namespace LlamaXML;

#ifdef _DEBUG
_CrtMemState startMemState;
#endif

ResourceManager::ResourceManager( TResourceHistoryMode historymode )
:	pArchiveFile	( NULL )
,	HistoryMode		( historymode )
,	pXMLStreamer	( NULL )
{	
	_CrtMemCheckpoint( &startMemState );
#ifndef FINAL_RELEASE
	//if ( GINFO_DUMP_ON && gInfo.DumpInitialData )
	//{
	//	TCHAR	cbuf[MAX_PATH];
	//	pArchiveFile = DNew ArchiveFile();		
	//	_stprintf_s(cbuf, _T("%s\\ResourceHistoryStorage.pak"), gInfo.LogFileDirectory);
	//	pArchiveFile->Init(
	//		cbuf,
	//		(HistoryMode == TRMMode_SaveHistory) ? ArchiveFile::TArchive_OpenForDeflate : ArchiveFile::TArchive_OpenForInflate
	//	);
	//}


	pXMLStreamer = DNew XMLStreamer( 
		(HistoryMode == TRMMode_SaveHistory) ? XMLStreamer::TXML_OpenForWrite : XMLStreamer::TXML_OpenForRead );
#endif
}

void ResourceManager::OpenFile( D3D10DDI_HDEVICE hDevice )
{
	if ( HistoryMode == TRMMode_SaveHistory )
	{
#ifndef FINAL_RELEASE
		DWORD deviceId = GetID(hDevice);
		wchar_t	szBuf[MAX_PATH];
		swprintf_s( szBuf, MAX_PATH, L"%s\\ResourceHistory-%d.xml", gData.DumpDirectory, deviceId);
		DEBUG_MESSAGE(_T("Resource History: ResourceHistory-%d.xml\n"), deviceId);
		pXMLStreamer->OpenFile(szBuf, "iZ3DResourceHistory");
		if (GINFO_DUMP_ON && gInfo.DumpInitialData)
		{
			pArchiveFile = new ArchiveFile();
			swprintf_s( szBuf, MAX_PATH, L"%s\\ResourcesPack-%d.pak", gData.DumpDirectory, deviceId);
			pArchiveFile->Init(szBuf, ArchiveFile::TArchive_OpenForDeflate);
		}
#endif
	}
}

ResourceManager::~ResourceManager()
{	
#ifndef FINAL_RELEASE
	if( HistoryMode == TRMMode_SaveHistory )
		SaveResourceHistory();

	if ( pArchiveFile )
	{
		pArchiveFile->Release();
		delete pArchiveFile;
	}

	if ( pXMLStreamer )
		delete pXMLStreamer;
#endif	

#ifdef _DEBUG
	_CrtMemState endMemState;  
	_CrtMemCheckpoint( &endMemState );

	_CrtMemState diffMemState;
	_CrtMemDifference( &diffMemState, &startMemState, &endMemState );
	_CrtMemDumpStatistics( &diffMemState ); 
#endif
}

void ResourceManager::SaveResourceHistory()
{
#ifndef FINAL_RELEASE
	_ASSERT( pXMLStreamer );
	_ASSERT( HistoryMode == TRMMode_SaveHistory );
	if (!pXMLStreamer->IsWriterOK())
		return;
	Adapters_.SaveResourceHistory( pXMLStreamer );
	Devices_.SaveResourceHistory( pXMLStreamer );
	Resources_.SaveResourceHistory( pXMLStreamer );
	ShaderResourceViews_.SaveResourceHistory( pXMLStreamer );
	RenderTargetViews_.SaveResourceHistory( pXMLStreamer );
	DepthStencilView_.SaveResourceHistory( pXMLStreamer );
	Shaders_.SaveResourceHistory( pXMLStreamer );
	Layouts_.SaveResourceHistory( pXMLStreamer );
	BlendStates_.SaveResourceHistory( pXMLStreamer );
	DepthStencilSStates_.SaveResourceHistory( pXMLStreamer );
	RasterizerStates_.SaveResourceHistory( pXMLStreamer );
	Samplers_.SaveResourceHistory( pXMLStreamer );
	Queries_.SaveResourceHistory( pXMLStreamer );
#endif
}

void ResourceManager::LoadResourceHistory(const wchar_t* fileName)
{
#ifndef FINAL_RELEASE
	_ASSERT( pXMLStreamer );
	_ASSERT( HistoryMode == TRMMode_LoadHistory );
	pXMLStreamer->OpenFile(fileName, "iZ3DResourceHistory");
	Adapters_.LoadResourceHistory( pXMLStreamer );
	Devices_.LoadResourceHistory( pXMLStreamer );
	Resources_.LoadResourceHistory( pXMLStreamer );
	ShaderResourceViews_.LoadResourceHistory( pXMLStreamer );
	RenderTargetViews_.LoadResourceHistory( pXMLStreamer );
	DepthStencilView_.LoadResourceHistory( pXMLStreamer );
	Shaders_.LoadResourceHistory( pXMLStreamer );
	Layouts_.LoadResourceHistory( pXMLStreamer );
	BlendStates_.LoadResourceHistory( pXMLStreamer );
	DepthStencilSStates_.LoadResourceHistory( pXMLStreamer );
	RasterizerStates_.LoadResourceHistory( pXMLStreamer );
	Samplers_.LoadResourceHistory( pXMLStreamer );
	Queries_.LoadResourceHistory( pXMLStreamer );
#endif
}

//////////////////////////////////////////////////////////////////////////

void CResource::SaveTextureToFile( DWORD EventID )
{
#ifndef FINAL_RELEASE
	// FIXME! only 2D textures R8G8B8A8 are supported ATM
	_ASSERT(CreateResource_.pInitialDataUP);
	_ASSERT(CreateResource_.ResourceDimension == D3D10DDIRESOURCE_TEXTURE2D);

	TCHAR path[MAX_PATH];
	_stprintf_s( path, MAX_PATH, _T("%s\\Resources"), gData.DumpDirectory );
	CreateDirectory( path, NULL );
	TCHAR filename[MAX_PATH];
	_stprintf_s( filename, MAX_PATH, _T("%s\\img%08X.jpg"), path, EventID );

	// FIXME! only mip-map level zero is supported ATM
	// FIXME! texture arrays are not supported ATM
	DWORD	width		= CreateResource_.pMipInfoList[0].TexelWidth;
	DWORD	height		= CreateResource_.pMipInfoList[0].TexelHeight;
	char *	buffer		= (char *)CreateResource_.pInitialDataUP[0].pSysMem;
	UINT	pitch		= CreateResource_.pInitialDataUP[0].SysMemPitch;

	SaveImageToFile(filename, CreateResource_.Format, width, height, buffer, pitch);
#endif
}


void CResource::SaveInitialResourceData( D3DDeviceWrapper* pWrapper, ArchiveFile*  pArchiveFile, DWORD EventID )
{
#ifndef FINAL_RELEASE
	_ASSERT(pWrapper);
	_ASSERT(pArchiveFile);
	if (!CreateResource_.pInitialDataUP)
		return;

	THistoryElement	historyelement;
	ArchiveFile::TArchiveChunk ch;
	ch.nOffset           = 0;
	ch.nSizeAfterArchive = 0;
	ch.nSizeBeforArchive = 0;

	UINT numsubresources = CreateResource_.MipLevels * CreateResource_.ArraySize;
	historyelement.reserve( numsubresources );
	
	DEBUG_TRACE3( _T("Saving resource:\n") );
	DEBUG_TRACE3( _T("\tResourceDimention = %s\n"), EnumToString(CreateResource_.ResourceDimension) );
	DEBUG_TRACE3( _T("\tArraySize = %d\n"), CreateResource_.ArraySize );
	for( UINT i=0; i<numsubresources; ++i )
	{
		DEBUG_TRACE3( _T("\tSaving subresource:\n") );
		if ( i < MipInfoList_.size() )
		{
			DEBUG_TRACE3( _T("\t\tPhysicalWidth = %d\n"), MipInfoList_[i].PhysicalWidth );
			DEBUG_TRACE3( _T("\t\tPhysicalHeight = %d\n"), MipInfoList_[i].PhysicalHeight );
		}
		DEBUG_TRACE3( _T("\t\tSysMemPitch = %d\n"), InitialDataUP_[i].SysMemPitch );
		DEBUG_TRACE3( _T("\t\tSysMemSlicePitch = %d\n"), InitialDataUP_[i].SysMemSlicePitch );

		DWORD subresourcesize;
		switch( CreateResource_.ResourceDimension )
		{
			// TODO! check various buffers with various SDK samples
			case D3D10DDIRESOURCE_BUFFER:			subresourcesize = MipInfoList_[i].PhysicalWidth;		break;

			case D3D10DDIRESOURCE_TEXTURE1D:		subresourcesize = ( CreateResource_.ArraySize == 1 ) ? 
														InitialDataUP_[i].SysMemPitch : 
														InitialDataUP_[i].SysMemSlicePitch;
													break;

			case D3D10DDIRESOURCE_TEXTURE2D:		subresourcesize = ( CreateResource_.ArraySize == 1 ) ?
														InitialDataUP_[i].SysMemPitch * MipInfoList_[i].PhysicalHeight :
														InitialDataUP_[i].SysMemSlicePitch;
													break;
			case D3D10DDIRESOURCE_TEXTURE3D:
			case D3D10DDIRESOURCE_TEXTURECUBE:		subresourcesize = InitialDataUP_[i].SysMemSlicePitch;	break;
		}

		// sometimes subresourcesize is zero, more precisely SysMemSlicePitch == 0
		if (subresourcesize > 0)
		{
			pArchiveFile->Deflate(
				(const char *) InitialDataUP_[i].pSysMem,
				subresourcesize,
				ch
			);
			historyelement.push_back( ch );
		}
		else {
			DEBUG_TRACE1( _T("ERROR: Failed storing subresource. Subresource size is zero:\n") );
		}
	}

	// DEBUG only! Texture2D - save the initial data to BMP image
	if( D3D10DDIRESOURCE_TEXTURE2D == CreateResource_.ResourceDimension )
		SaveTextureToFile( EventID );

	History[ EventID ] = historyelement;			// we should use EventID (EID) here!	
#endif
}

void CResource::SaveMappableResourceData( D3DDeviceWrapper* pWrapper, ArchiveFile*  pArchiveFile, DWORD EventID )
{
#ifndef FINAL_RELEASE
	_ASSERT( D3D10_DDI_MAP_READ == CreateResource_.MapFlags || D3D10_DDI_MAP_READWRITE == CreateResource_.MapFlags );

	D3D10DDI_HRESOURCE hResource = GET_RESOURCE_HANDLE(hResource_);

	THistoryElement	historyelement;
	ArchiveFile::TArchiveChunk ch;
	ch.nOffset           = 0;
	ch.nSizeAfterArchive = 0;
	ch.nSizeBeforArchive = 0;

	UINT numsubresources = CreateResource_.MipLevels * CreateResource_.ArraySize;
	historyelement.reserve( numsubresources );

	//iterate all sub-resources to save the whole resource
	for( UINT i = 0; i<numsubresources; ++i )
	{

		D3D10DDI_MAPPED_SUBRESOURCE	subresource;
		pWrapper->OriginalDeviceFuncs.pfnResourceMap(
			pWrapper->hDevice,
			hResource,
			i,			// subresource
			D3D10_DDI_MAP_READ,
			0,			// flags (D3D10_DDI_MAP_FLAG_DONOTWAIT, etc.)
			&subresource
			);

		DWORD subresourcesize;
		switch( CreateResource_.ResourceDimension )
		{
			case D3D10DDIRESOURCE_BUFFER:
			case D3D10DDIRESOURCE_TEXTURE1D:		subresourcesize = subresource.RowPitch;		break;

			case D3D10DDIRESOURCE_TEXTURE2D:
			case D3D10DDIRESOURCE_TEXTURE3D:
			case D3D10DDIRESOURCE_TEXTURECUBE:		subresourcesize = subresource.DepthPitch;	break;
		}

		pArchiveFile->Deflate(
			(const char *) subresource.pData,
			subresourcesize,
			ch
			);
		historyelement.push_back( ch );

		pWrapper->OriginalDeviceFuncs.pfnResourceUnmap(
			pWrapper->hDevice,
			hResource,
			i			// subresource
			);
	}
	History[ EventID ] = historyelement;			// we should use EventID (EID) here!	
#endif
}

void CResource::SaveResourceData( D3DDeviceWrapper* pWrapper, ArchiveFile*  pArchiveFile, DWORD EventID )
{
#ifndef FINAL_RELEASE

	D3D10DDI_HRESOURCE hResource = GET_RESOURCE_HANDLE(hResource_);

	if( D3D10_DDI_MAP_READ == CreateResource_.MapFlags || D3D10_DDI_MAP_READWRITE == CreateResource_.MapFlags )
	{
		SaveMappableResourceData( pWrapper, pArchiveFile, EventID);
		return;
	}

	_ASSERT(pWrapper);
	_ASSERT(pArchiveFile);
	D3D11DDIARG_CREATERESOURCE cres = CreateResource11_;
	cres.Usage     = D3D10_DDI_USAGE_STAGING;
	cres.MapFlags  = D3D10_DDI_MAP_READ;
	cres.BindFlags = 0;				// Don't change this - we can't bind our copy to the graphics pipeline!
	cres.pPrimaryDesc = NULL;		// Don't change this - we can't present our copy!
	cres.SampleDesc.Count	= 1;	// Don't change this - we can't map copy-and-map multi-sampled resources!
	cres.SampleDesc.Quality = 0;

	THistoryElement	historyelement;
	ArchiveFile::TArchiveChunk ch;
	ch.nOffset           = 0;
	ch.nSizeAfterArchive = 0;
	ch.nSizeBeforArchive = 0;

	UINT numsubresources = CreateResource_.MipLevels * CreateResource_.ArraySize;
	historyelement.reserve( numsubresources );
	{
		std::vector<BYTE>		bbuffer;
		D3D10DDI_HRESOURCE		hStagingResource;
		D3D10DDI_HRTRESOURCE	hrtbuffer = { NULL };
		if ( pWrapper->GetD3DVersion() != TD3DVersion_11_0 )
		{
			SIZE_T	rsize = pWrapper->OriginalDeviceFuncs.pfnCalcPrivateResourceSize(
				pWrapper->hDevice, 
				(D3D10DDIARG_CREATERESOURCE*)&cres
				);

			bbuffer.resize( rsize, BYTE(0) );
			hStagingResource.pDrvPrivate		= (VOID *) &bbuffer[ 0 ];

			pWrapper->OriginalDeviceFuncs.pfnCreateResource(
				pWrapper->hDevice,
				(D3D10DDIARG_CREATERESOURCE*)&cres,
				hStagingResource,
				hrtbuffer
				);
		}
		else
		{
			SIZE_T	rsize = pWrapper->OriginalDeviceFuncs11.pfnCalcPrivateResourceSize(
				pWrapper->hDevice, 
				&cres
				);

			bbuffer.resize( rsize, BYTE(0) );
			hStagingResource.pDrvPrivate		= (VOID *) &bbuffer[ 0 ];

			pWrapper->OriginalDeviceFuncs11.pfnCreateResource(
				pWrapper->hDevice,
				&cres,
				hStagingResource,
				hrtbuffer
				);
		}

		if( 1 == CreateResource_.SampleDesc.Count )
			pWrapper->OriginalDeviceFuncs.pfnResourceCopy(
				pWrapper->hDevice,
				hStagingResource,					// hDstResource
				hResource					// hSrcResource
			);
		else
		{
			// multi-sampled resource must be resolved prior to it being copied!
			// resolve the multi-sampled resource
			D3D11DDIARG_CREATERESOURCE cres2 = CreateResource11_;
			cres2.Usage     = D3D10_DDI_USAGE_DEFAULT;
			cres2.MapFlags  = 0;
			cres2.BindFlags = 0;			// Don't change this - we can't bind our copy to the graphics pipeline!
			cres2.pPrimaryDesc = NULL;		// Don't change this - we can't present our copy!
			cres2.SampleDesc.Count	= 1;	// Don't change this - we can't copy-and-map multi-sampled resources!
			cres2.SampleDesc.Quality = 0;

			std::vector<BYTE>		bbuffer2;
			D3D10DDI_HRESOURCE		hDefaultResource;
			D3D10DDI_HRTRESOURCE	hrtbuffer2 = { NULL };
			if ( pWrapper->GetD3DVersion() != TD3DVersion_11_0 )
			{
				SIZE_T	rsize2 = pWrapper->OriginalDeviceFuncs.pfnCalcPrivateResourceSize(
					pWrapper->hDevice, 
					(D3D10DDIARG_CREATERESOURCE*)&cres2
					);

				bbuffer2.resize( rsize2, BYTE(0) );
				hDefaultResource.pDrvPrivate	= (VOID *) &bbuffer2[ 0 ];

				pWrapper->OriginalDeviceFuncs.pfnCreateResource(
					pWrapper->hDevice,
					(D3D10DDIARG_CREATERESOURCE*)&cres2,
					hDefaultResource,
					hrtbuffer2
					);
			}
			else
			{
				SIZE_T	rsize2 = pWrapper->OriginalDeviceFuncs11.pfnCalcPrivateResourceSize(
					pWrapper->hDevice, 
					&cres2
					);

				bbuffer2.resize( rsize2, BYTE(0) );
				hDefaultResource.pDrvPrivate	= (VOID *) &bbuffer2[ 0 ];

				pWrapper->OriginalDeviceFuncs11.pfnCreateResource(
					pWrapper->hDevice,
					&cres2,
					hDefaultResource,
					hrtbuffer2
					);
			}

			DXGI_FORMAT format = GetTypedDXGIFormat( cres2.Format );
			for( UINT i=0; i<numsubresources; ++i )
				pWrapper->OriginalDeviceFuncs.pfnResourceResolveSubresource(
					pWrapper->hDevice,
					hDefaultResource,				// hDstResource
					i,						// DstSubresource
					hResource,				// hSrcResource
					i,						// SrcSubresource
					format					// ResolveFormat
				);

			pWrapper->OriginalDeviceFuncs.pfnResourceCopy(
				pWrapper->hDevice,
				hStagingResource,					// hDstResource
				hDefaultResource					// hSrcResource
			);

			pWrapper->OriginalDeviceFuncs.pfnDestroyResource(
				pWrapper->hDevice, 
				hDefaultResource
			);

		}

		//iterate all sub-resources to save the whole resource
		for( UINT i = 0; i<numsubresources; ++i )
		{

			D3D10DDI_MAPPED_SUBRESOURCE	subresource;
			pWrapper->OriginalDeviceFuncs.pfnStagingResourceMap(
				pWrapper->hDevice,
				hStagingResource,
				i,			// subresource
				D3D10_DDI_MAP_READ,
				0,			// flags (D3D10_DDI_MAP_FLAG_DONOTWAIT, etc.)
				&subresource
			);

			DWORD subresourcesize;
			switch( cres.ResourceDimension )
			{
				case D3D10DDIRESOURCE_BUFFER:
				case D3D10DDIRESOURCE_TEXTURE1D:		subresourcesize = subresource.RowPitch;		break;

				case D3D10DDIRESOURCE_TEXTURE2D:
				case D3D10DDIRESOURCE_TEXTURE3D:
				case D3D10DDIRESOURCE_TEXTURECUBE:		subresourcesize = subresource.DepthPitch;	break;
			}

			pArchiveFile->Deflate(
				(const char *) subresource.pData,
				subresourcesize,
				ch
			);
			historyelement.push_back( ch );

			pWrapper->OriginalDeviceFuncs.pfnStagingResourceUnmap(
				pWrapper->hDevice,
				hStagingResource,
				i			// subresource
			);
		}

		pWrapper->OriginalDeviceFuncs.pfnDestroyResource(
			pWrapper->hDevice, 
			hStagingResource
		);

	}

	History[ EventID ] = historyelement;			// we should use EventID (EID) here!	
#endif
}

void CResource::SaveCreationParameters( XMLStreamer* _pXMLStreamer )
{
	_ASSERT( _pXMLStreamer );
	_pXMLStreamer->StartNewElement( "D3D10DDI_HRESOURCE" );
		_pXMLStreamer->m_pXMLWriter->WriteAttribute("ResourceID", RID );
		_pXMLStreamer->StartNewElement( "CREATION_PARAM" );
			if( !MipInfoList_.empty()   ) CreateResource_.pMipInfoList   = &MipInfoList_[0];
			if( !InitialDataUP_.empty() ) CreateResource_.pInitialDataUP = &InitialDataUP_[0];
			_pXMLStreamer->WriteValue("D3D10DDIARG_CREATERESOURCE", CreateResource_ );
		_pXMLStreamer->EndElement();
	_pXMLStreamer->EndElement();
}

void CResource::SaveResourceHistory( XMLStreamer* _pXMLStreamer )
{
	_ASSERT( _pXMLStreamer );
	_pXMLStreamer->StartNewElement( "D3D10DDI_HRESOURCE" );
		_pXMLStreamer->m_pXMLWriter->WriteAttribute("ResourceID", RID );
		_pXMLStreamer->WriteValue( "REVISIONS", History );
	_pXMLStreamer->EndElement();
}

//////////////////////////////////////////////////////////////////////////

void CShader::SaveInitialResourceData( D3DDeviceWrapper* pWrapper, ArchiveFile*  pArchiveFile, DWORD EventID )
{
#ifndef FINAL_RELEASE
	_ASSERT(pWrapper);
	_ASSERT(pArchiveFile);

	ArchiveFile::TArchiveChunk ch;
	pArchiveFile->Deflate(
		(const char *) &Code_[0],
		Code_.size() * sizeof(Code_[0]),
		ch
	);									

	History[ EventID ] = THistoryElement(1, ch);	// we should use EventID (EID) here!	
	
	THistoryElement& vec = History.find(EventID)->second;
	vec.reserve(3);									// code chunk + input signatures + output signatures

	size_t i_size = InputSignatures_.size();
	if( i_size )
	{
		pArchiveFile->Deflate(
			(const char *) &InputSignatures_[0],
			InputSignatures_.size() * sizeof(InputSignatures_[0]),
			ch
		);									
	}
	else
		ch.nOffset = ch.nSizeAfterArchive = ch.nSizeBeforArchive = 0;
	vec.push_back(ch);

	size_t o_size = OutputSignatures_.size();
	if( o_size )
	{
		pArchiveFile->Deflate(
			(const char *) &OutputSignatures_[0],
			OutputSignatures_.size() * sizeof(OutputSignatures_[0]),
			ch
		);
	}
	else
		ch.nOffset = ch.nSizeAfterArchive = ch.nSizeBeforArchive = 0;
	vec.push_back(ch);

	//--- temporary shader writer ---
	if(i_size && o_size)
	{
		TCHAR lName[MAX_PATH], sName[64], sDecl[64];
		_tcscpy_s(lName, gData.DumpDirectory);
		ShaderWrapper* pWrp;
		InitWrapper(hResource_, pWrp);
		switch(pWrp->m_ShaderType)
		{
			case D3D10DDISHADERUNITTYPE_GEOMETRY:
				PathAppend(lName, L"Bin_GS");
				break;
			case D3D10DDISHADERUNITTYPE_VERTEX:
				PathAppend(lName, L"Bin_VS");
				break;
			case D3D10DDISHADERUNITTYPE_PIXEL:
				PathAppend(lName, L"Bin_PS");
				break;
			case D3D11DDISHADERUNITTYPE_HULL:
				PathAppend(lName, L"Bin_HL");
				break;
			case D3D11DDISHADERUNITTYPE_DOMAIN:
				PathAppend(lName, L"Bin_DS");
				break;
			case D3D11DDISHADERUNITTYPE_COMPUTE:
				PathAppend(lName, L"Bin_CS");
				break;
			default:		
				PathAppend(lName, L"Bin");
		}
		_stprintf_s(sName, _countof(sName), L"shader%04i_0x%X.csh", pWrp->m_ShaderIndex, pWrp->m_CRC32);
		_stprintf_s(sDecl, _countof(sDecl), L"shader%04i_0x%X.dcl", pWrp->m_ShaderIndex, pWrp->m_CRC32);
		D3D10DDIARG_STAGE_IO_SIGNATURES signatures = { &InputSignatures_[0],  (UINT)i_size, &OutputSignatures_[0],  (UINT)o_size };
		WriteShaderToFile(sName, sDecl, lName, &Code_[0], &signatures);
		if ( pWrp->IsShaderModified() )
		{
			_stprintf_s(sName, _countof(sName), L"shader%04i_0x%X_modified.csh", pWrp->m_ShaderIndex, pWrp->m_CRC32);
			_stprintf_s(sDecl, _countof(sDecl), L"shader%04i_0x%X_modified.dcl", pWrp->m_ShaderIndex, pWrp->m_CRC32);
			WriteShaderToFile(sName, sDecl, lName, &pWrp->m_pCode[0], &signatures);
		}
	}
#endif	
}

void CShader::SaveShaderData( XMLStreamer* _pXMLStreamer )
{
#ifndef FINAL_RELEASE
	_ASSERT( _pXMLStreamer );
	_pXMLStreamer->StartNewElement( "D3D10DDI_HSHADER" );
		_pXMLStreamer->m_pXMLWriter->WriteAttribute("ResourceID", RID);
		ShaderWrapper* pWrp;
		InitWrapper(hResource_, pWrp);
		_pXMLStreamer->m_pXMLWriter->WriteAttribute("Type", EnumToString(pWrp->m_ShaderType));
		//TiXmlElement * pCreationParamElem	= _pXMLStreamer->AddNewElement( "CREATION_PARAM", pResourceElem );
		//_pXMLStreamer->WriteStreamer::Value("D3D10DDIARG_CREATERESOURCE", CreateResource_, pCreationParamElem );
	_pXMLStreamer->EndElement();
#endif
}

void CShader::SaveResourceHistory( XMLStreamer* _pXMLStreamer )
{
#ifndef FINAL_RELEASE
	_ASSERT( _pXMLStreamer );
	_pXMLStreamer->StartNewElement( "D3D10DDI_HSHADER" );
		_pXMLStreamer->m_pXMLWriter->WriteAttribute("ResourceID", RID);
		//ShaderWrapper* pWrp;
		//InitWrapper(hResource_, pWrp);
		//_pXMLStreamer->m_pXMLWriter->WriteAttribute("Type", EnumToString(pWrp->m_ShaderType));
		//TiXmlElement * pCreationParamElem	= _pXMLStreamer->AddNewElement( "CREATION_PARAM", pResourceElem );
		//_pXMLStreamer->WriteStreamer::Value("D3D10DDIARG_CREATERESOURCE", CreateResource_, pCreationParamElem );
		_pXMLStreamer->WriteValue( "REVISIONS", History );
	_pXMLStreamer->EndElement();
#endif
}

//////////////////////////////////////////////////////////////////////////

template <>	DWORD CEmptyResource<D3D10DDI_HADAPTER>::Counter_				= 0;
template <>	DWORD CEmptyResource<D3D10DDI_HDEVICE>::Counter_				= 0;
template <>	DWORD CEmptyResource<D3D10DDI_HRESOURCE>::Counter_				= 0;

template <>	DWORD CEmptyResource<D3D10DDI_HSHADERRESOURCEVIEW>::Counter_	= 0;
template <>	DWORD CEmptyResource<D3D10DDI_HRENDERTARGETVIEW>::Counter_		= 0;
template <>	DWORD CEmptyResource<D3D10DDI_HDEPTHSTENCILVIEW>::Counter_		= 0;
template <>	DWORD CEmptyResource<D3D10DDI_HSHADER>::Counter_				= 0;
template <>	DWORD CEmptyResource<D3D10DDI_HELEMENTLAYOUT>::Counter_			= 0;
template <>	DWORD CEmptyResource<D3D10DDI_HBLENDSTATE>::Counter_			= 0;
template <>	DWORD CEmptyResource<D3D10DDI_HDEPTHSTENCILSTATE>::Counter_		= 0;
template <>	DWORD CEmptyResource<D3D10DDI_HRASTERIZERSTATE>::Counter_		= 0;
template <>	DWORD CEmptyResource<D3D10DDI_HSAMPLER>::Counter_				= 0;
template <>	DWORD CEmptyResource<D3D10DDI_HQUERY>::Counter_					= 0;

template <>	DWORD CEmptyResource<D3D11DDI_HUNORDEREDACCESSVIEW>::Counter_	= 0;
template <>	DWORD CEmptyResource<D3D11DDI_HCOMMANDLIST>::Counter_			= 0;


//////////////////////////////////////////////////////////////////////////
