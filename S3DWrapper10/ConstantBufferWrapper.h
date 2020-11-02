#pragma once

#include "../ShaderAnalysis/AnalyzeData.h"
#include <D3DX10Math.h>
#include <boost/scoped_array.hpp>
#include <boost/variant.hpp>

class ResourceWrapper;
class D3DDeviceWrapper;

typedef boost::variant<ProjectionCBMatrices,
					   UnprojectionCBMatrices,
					   ModifiedShaderData>		CBUpdateData;

class ShaderConstantModifier
{
public:
	/** Compare constant modifier data with provided. */
	virtual bool CompareData(const CBUpdateData& updateData) = 0;
	
	/** Check if application setup projection or inverse projection matrix. 
	 * @return true if matrices need to be updated.
	 */
	virtual bool CheckMatrices(D3DDeviceWrapper* pWrapper, const FLOAT* pData) = 0;

	/** Update application projection matrix to left perspective or right perspective */
	virtual void UpdateMatrices(D3DDeviceWrapper* pWrapper, VIEWINDEX vi, const FLOAT* pMonoData, FLOAT* pData) = 0;

	virtual ~ShaderConstantModifier() {}
};

typedef boost::shared_ptr<ShaderConstantModifier> ShaderConstantModifierPtrT;

class ConstantBufferWrapper
{
public:
	ConstantBufferWrapper(D3DDeviceWrapper* pWrapper, ResourceWrapper* pResurceWrapper);
	virtual ~ConstantBufferWrapper();

	bool Initialize(D3DDeviceWrapper* pWrapper, const CBUpdateData& data, DWORD ShaderCRC);
	
	bool				IsMonoDataInitialized	( ) { return m_pMonoData != 0; };
	D3D10DDI_HRESOURCE	GetHandle(D3DDeviceWrapper* pWrapper, VIEWINDEX vi);

	void Map(void* pData);
	void Unmap(D3DDeviceWrapper* pWrapper);
	void UpdateSubresourceUP(D3DDeviceWrapper* pWrapper, const void* pData);

protected:
	/** Override in derived class to use 'Map' or 'UpdateSubresourceUp' */
	virtual void UpdateCurrentPool( D3DDeviceWrapper* pWrapper, VIEWINDEX index, bool mono ) = 0;

	void UpdateMatrices(D3DDeviceWrapper* pWrapper, VIEWINDEX vi, bool mono, FLOAT* pData);
	
protected:
	ResourceWrapper*			m_pResourceWrapper;
	ShaderConstantModifierPtrT	m_ShaderConstantModifier;
	DWORD						m_ShaderCRC;
	bool						m_bNeedToCheckMatrices;
	bool						m_bHaveStereoMatrices;
	bool						m_bBufferUpdated[2];
	UINT						m_ElementCount;
	void*						m_pData; // Map
	FLOAT*						m_pMonoData;
};

extern D3D10DDI_HRESOURCE UnwrapCurrentViewCBHandle( D3DDeviceWrapper* pWrapper, UINT index, D3D10DDI_HRESOURCE handle, SHADER_PIPELINE shaderType );
extern D3D10DDI_HRESOURCE UnwrapCBHandle( D3DDeviceWrapper* pWrapper, D3D10DDI_HRESOURCE handle );

#define UNWRAP_HS_CB_HANDLE(pWrapper, ind, hResource)		UnwrapCBHandle(pWrapper, hResource);
#define UNWRAP_DS_CB_HANDLE(pWrapper, ind, hResource)		UnwrapCurrentViewCBHandle(pWrapper, ind, hResource, SP_DS);
#define UNWRAP_GS_CB_HANDLE(pWrapper, ind, hResource)		UnwrapCurrentViewCBHandle(pWrapper, ind, hResource, SP_GS);
#define UNWRAP_VS_CB_HANDLE(pWrapper, ind, hResource)		UnwrapCurrentViewCBHandle(pWrapper, ind, hResource, SP_VS);
#define UNWRAP_PS_CB_HANDLE(pWrapper, ind, hResource)		UnwrapCurrentViewCBHandle(pWrapper, ind, hResource, SP_PS);
#define UNWRAP_CS_CB_HANDLE(pWrapper, ind, hResource)		UnwrapCBHandle(pWrapper, hResource);
