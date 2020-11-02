#include "StdAfx.h"
#include "D3DDeviceWrapper.h"
#include "ShaderWrapper.h"
#include "ExternalConstantBuffer.h"
#include "Commands/DefaultConstantBufferUpdateSubresourceUP.h"

iz3d::ExternalConstantBuffer::ExternalConstantBuffer( D3DDeviceWrapper* pDevice, UINT numConstants )
	: m_numConst( numConstants ), m_pDevice ( pDevice )
{
	D3D10DDI_HDEVICE hDevice = m_pDevice->GetHandle();

	D3D11DDIARG_CREATERESOURCE CreateCBRes;
	memset( &CreateCBRes, 0, sizeof( D3D11DDIARG_CREATERESOURCE ) );

	UINT totalConstantSize = ( numConstants / 4 + ( ( numConstants % 4 )?1:0 ) ) * 4 * sizeof(float);
	m_Data = malloc(totalConstantSize);

	D3D10DDI_MIPINFO CBMipInfo;
	CBMipInfo.TexelWidth				= totalConstantSize;
	CBMipInfo.TexelHeight				= 1;
	CBMipInfo.TexelDepth				= 1;
	CBMipInfo.PhysicalWidth				= totalConstantSize;
	CBMipInfo.PhysicalHeight			= 1;
	CBMipInfo.PhysicalDepth				= 1;

	CreateCBRes.pMipInfoList			= &CBMipInfo;
	CreateCBRes.ResourceDimension		= D3D10DDIRESOURCE_BUFFER;
	CreateCBRes.Usage					= D3D10_DDI_USAGE_DEFAULT;
	CreateCBRes.BindFlags				= D3D10_DDI_BIND_CONSTANT_BUFFER;
	CreateCBRes.Format					= DXGI_FORMAT_UNKNOWN;
	CreateCBRes.SampleDesc.Count		= 1;
	CreateCBRes.SampleDesc.Quality		= 0;
	CreateCBRes.MipLevels				= 1;
	CreateCBRes.ArraySize				= 1;

	m_hCBView.pDrvPrivate				= NULL;

	static char RTData[16] = {0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xA, 0xB, 0xC, 0xD, 0xE, 0xF};
	D3D10DDI_HRTRESOURCE hRTCBView		= { RTData };

	ResourceWrapper* pWrp;	
	
	//////////////////////////////////////////////////////////////////////////
	// Creation
	if ( m_pDevice->GetD3DVersion() != TD3DVersion_11_0 )
	{
		SIZE_T nSize = m_pDevice->OriginalDeviceFuncs.pfnCalcPrivateResourceSize( hDevice, (D3D10DDIARG_CREATERESOURCE*)&CreateCBRes );
		nSize += ADDITIONAL_RESOURCE_SIZE;
		NEW_HANDLE( m_hCBView, nSize );
		InitWrapper(m_hCBView, pWrp);
		const D3D10DDIARG_CREATERESOURCE* pCreateResource = (const D3D10DDIARG_CREATERESOURCE*)&CreateCBRes;
		new(pWrp) ResourceWrapper( m_pDevice, pCreateResource, hRTCBView );
		m_pDevice->OriginalDeviceFuncs.pfnCreateResource( hDevice, (D3D10DDIARG_CREATERESOURCE*)&CreateCBRes, GET_RESOURCE_HANDLE(m_hCBView), hRTCBView );
	}
	else
	{
		SIZE_T nSize = m_pDevice->OriginalDeviceFuncs11.pfnCalcPrivateResourceSize( hDevice, &CreateCBRes );
		nSize += ADDITIONAL_RESOURCE_SIZE;
		NEW_HANDLE( m_hCBView, nSize );
		InitWrapper(m_hCBView, pWrp);
		const D3D11DDIARG_CREATERESOURCE* pCreateResource = (const D3D11DDIARG_CREATERESOURCE*)&CreateCBRes;
		new(pWrp) ResourceWrapper( m_pDevice, pCreateResource, hRTCBView );
		m_pDevice->OriginalDeviceFuncs11.pfnCreateResource( hDevice, &CreateCBRes, GET_RESOURCE_HANDLE(m_hCBView), hRTCBView );
	}
	//////////////////////////////////////////////////////////////////////////
	m_pResourceWrapper = pWrp;
}

iz3d::ExternalConstantBuffer::~ExternalConstantBuffer()
{
	D3D10DDI_HDEVICE hDevice = m_pDevice->GetHandle();
	ResourceWrapper* pWrp;
	InitWrapper(m_hCBView, pWrp);
	pWrp->Destroy(D3D10_GET_WRAPPER());
	pWrp->~ResourceWrapper();
	m_pDevice->OriginalDeviceFuncs.pfnDestroyResource( hDevice, GET_RESOURCE_HANDLE(m_hCBView) );
	DELETE_HANDLE(m_hCBView);
	if (m_Data) {
		free(m_Data);
	}
}
		
void iz3d::ExternalConstantBuffer::initMonoData( ShaderWrapper* pWrp )
{
	float* pData = (float*)m_Data;
	if (gInfo.VertexShaderModificationMethod < 2)
	{
		//--- dp4Vector ---
		pData[0] = 1; pData[1] = 0; pData[2] = 0; pData[3] = 0;
		//--- ZNearVector ---
		pData[4] = 0; pData[5] = 0; pData[6] = 0; pData[7] = pWrp->m_ModifiedShaderData.StereoZNear;
	}
	else
	{
		//--- dp4Vector ---
		pData[0] = 0; pData[1] = 0; pData[2] = 0; pData[3] = pWrp->m_ModifiedShaderData.StereoZNear;
	}
}

void iz3d::ExternalConstantBuffer::setConstantsF( TFloatData& data )
{

	D3D10DDI_HDEVICE hDevice = m_pDevice->GetHandle();
	UINT size = (UINT)(data.size() * sizeof( TFloatData::value_type ));
	m_pDevice->OriginalDeviceFuncs.pfnDefaultConstantBufferUpdateSubresourceUP( hDevice, GET_RESOURCE_HANDLE(m_hCBView), 0, NULL, ( const void* )&data[0], size, size );

}

void iz3d::ExternalConstantBuffer::setConstantsI( TIntData& data )
{
	D3D10DDI_HDEVICE hDevice = m_pDevice->GetHandle();
	UINT size = (UINT)(data.size() * sizeof( TIntData::value_type ));
	m_pDevice->OriginalDeviceFuncs.pfnDefaultConstantBufferUpdateSubresourceUP( hDevice, GET_RESOURCE_HANDLE(m_hCBView), 0, NULL, ( const void* )&data[0], size, size );
}

D3D10DDI_HRESOURCE iz3d::ExternalConstantBuffer::getHResource() const
{
	return m_hCBView;
}

Commands::TCmdPtr iz3d::ExternalConstantBuffer::setConstantsAsyncF( const void* data, UINT size )
{
	D3D10DDI_HDEVICE hDevice = m_pDevice->GetHandle();
	return new Commands::DefaultConstantBufferUpdateSubresourceUP( m_hCBView, 0, NULL, data, size, size );
}

Commands::TCmdPtr iz3d::ExternalConstantBuffer::setConstantsAsyncI( const void* data, UINT size )
{
	D3D10DDI_HDEVICE hDevice = m_pDevice->GetHandle();
	return new Commands::DefaultConstantBufferUpdateSubresourceUP( m_hCBView, 0, NULL, data, size, size );
}