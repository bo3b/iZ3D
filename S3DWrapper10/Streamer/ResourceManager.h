#ifndef __S3D_RESOURCEMANAGER_H__
#define __S3D_RESOURCEMANAGER_H__

#include <d3d10umddi.h>
#include "../ArchiveFile/archivefile.h"
#include <boost/unordered_map.hpp>
#include <vector>
#include <functional>

class D3DDeviceWrapper;
class XMLStreamer;

//////////////////////////////////////////////////////////////////////////

class CBaseResource
{
public:
	typedef std::vector<ArchiveFile::TArchiveChunk>				THistoryElement;
	typedef boost::unordered_map<DWORD, THistoryElement>		THistory;

	CBaseResource(DWORD _RID)
		:	RID			(_RID)		
	{}

	virtual ~CBaseResource()
	{}

	virtual void	SaveResourceData		( D3DDeviceWrapper* /*pWrapper*/, ArchiveFile*  /*pArchiveFile*/, DWORD /*EventID*/  )
	{}
	virtual void	SaveInitialResourceData	( D3DDeviceWrapper* /*pWrapper*/, ArchiveFile*  /*pArchiveFile*/, DWORD /*EventID*/  )
	{}
	virtual void	SaveResourceHistory		( XMLStreamer*	/*pXMLStreamer*/ )
	{}	
	virtual void	LoadResourceHistory		( XMLStreamer*	/*pXMLStreamer*/ )
	{}	
	
	DWORD	GetResourceID()	const
	{
		return RID;
	}	

protected:
	THistory		History;		// <EventID + vector of TArchiveChunk> pair map
	DWORD			RID;	
};

template <typename T>
class CEmptyResource: public CBaseResource
{
public:
	CEmptyResource(T Resource)
		:	CBaseResource(++Counter_)
		,	hResource_(Resource)
	{}

protected:
	static DWORD					Counter_;
	T								hResource_;			// the value (the resource UM handle)
};

class CResource: public CEmptyResource<D3D10DDI_HRESOURCE>
{
public:
	CResource(D3D10DDI_HRESOURCE hResource)
		:	CEmptyResource(hResource)
	{
		memset( &CreateResource_, 0, sizeof( D3D10DDIARG_CREATERESOURCE ) );
	}

	virtual void	SaveResourceData		( D3DDeviceWrapper* pWrapper, ArchiveFile*  pArchiveFile, DWORD EventID );
	virtual void	SaveInitialResourceData	( D3DDeviceWrapper* pWrapper, ArchiveFile*  pArchiveFile, DWORD EventID );
	virtual void	SaveResourceHistory		( XMLStreamer*	pXMlStreamer );

	void	SetArgCreateResource( const D3D10DDIARG_CREATERESOURCE *pRes )
	{
		CreateResource_ = *pRes;
		if( pRes->pPrimaryDesc )
		{
			PrimaryDesc_ = *pRes->pPrimaryDesc;
			CreateResource_.pPrimaryDesc = &PrimaryDesc_;
		}

		if( pRes->pMipInfoList )
		{			
			MipInfoList_.reserve( CreateResource_.MipLevels );
			for( UINT i=0; i<CreateResource_.MipLevels; ++i )
				MipInfoList_.push_back( CreateResource_.pMipInfoList[i] );
			CreateResource_.pMipInfoList = &MipInfoList_[0];
		}

		if( pRes->pInitialDataUP )
		{
			UINT	numsubresources = CreateResource_.MipLevels * CreateResource_.ArraySize;
			InitialDataUP_.reserve( numsubresources );
			for( UINT i=0; i<numsubresources; ++i ) 
				InitialDataUP_.push_back( CreateResource_.pInitialDataUP[i] );
		}
	}

	void SaveCreationParameters(XMLStreamer* _pXMLStreamer);

protected:
	union
	{
		D3D10DDIARG_CREATERESOURCE		CreateResource_;
		D3D11DDIARG_CREATERESOURCE		CreateResource11_;
	};
	std::vector<D3D10DDI_MIPINFO>				MipInfoList_;
	std::vector<D3D10_DDIARG_SUBRESOURCE_UP>	InitialDataUP_;
	DXGI_DDI_PRIMARY_DESC			PrimaryDesc_;

	void	SaveTextureToFile( DWORD EventID );
	void	SaveMappableResourceData( D3DDeviceWrapper* pWrapper, ArchiveFile*  pArchiveFile, DWORD EventID );
};

class CShader: public CEmptyResource<D3D10DDI_HSHADER>
{
public:
	CShader(D3D10DDI_HSHADER hShader)
		:	CEmptyResource(hShader)
	{}

	virtual ~CShader()
	{
	}

	virtual void	SaveInitialResourceData	( D3DDeviceWrapper* pWrapper, ArchiveFile*  pArchiveFile, DWORD EventID );

	virtual void	SaveResourceHistory	( XMLStreamer*	pXMlStreamer );

	void	SetArgSignatures( CONST D3D10DDIARG_STAGE_IO_SIGNATURES*  pSignatures )
	{
		// pSignatures != NULL
		InputSignatures_.reserve (pSignatures->NumInputSignatureEntries );
		OutputSignatures_.reserve(pSignatures->NumOutputSignatureEntries);
		for( UINT i = 0; i<pSignatures->NumInputSignatureEntries;  i++ ) InputSignatures_.push_back (pSignatures->pInputSignature [i]);
		for( UINT i = 0; i<pSignatures->NumOutputSignatureEntries; i++ ) OutputSignatures_.push_back(pSignatures->pOutputSignature[i]);
	}

	void	SetArgCode( CONST UINT*	pCode )
	{
		if (pCode == NULL)
			return;
		Code_.reserve(pCode[1]);
		for( UINT i = 0; i<pCode[1];  i++ ) Code_.push_back(pCode[i]);
	}

	void SaveShaderData( XMLStreamer* _pXMLStreamer );

protected:
	std::vector<UINT>	Code_;
	std::vector<D3D10DDIARG_SIGNATURE_ENTRY>	InputSignatures_;
	std::vector<D3D10DDIARG_SIGNATURE_ENTRY>	OutputSignatures_;
};



//////////////////////////////////////////////////////////////////////////

//TODO: Add management texture surfaces and parent devices
template<typename T, typename P>
class ResourceType
{
public:

	typedef boost::unordered_map< T, CBaseResource* > ResourceMap;

	ResourceType() {}

	~ResourceType()
	{
		ResourceMap::const_iterator it = Data.begin();
		for( ; it != Data.end(); ++it )
			delete it->second;
	}

	void SaveResourceHistory( XMLStreamer* _pXMLStreamer )
	{
		ResourceMap::const_iterator it = Data.begin();
		for( ; it != Data.end(); ++it )
			it->second->SaveResourceHistory( _pXMLStreamer );
	}

	void LoadResourceHistory( XMLStreamer* _pXMLStreamer )
	{
		//ResourceMap::const_iterator it = Data.begin();
		//for( ; it != Data.end(); ++it )
		//	it->second->SaveResourceHistory( _pXMLStreamer );
	}

	CBaseResource* FindResource(T val)
	{
		_ASSERT(val.pDrvPrivate != NULL);
		ResourceMap::const_iterator it = Data.find(val);
		if (it != Data.end())
			return it->second;
		else
			return NULL;
	}

	CBaseResource* FindOrCreateResource(T val)
	{
		CBaseResource* p = FindResource(val);
		if( p ) 
			return p;
		
		return CreateResource(val);
	}

	CBaseResource* CreateResource(T val)
	{
		P* r = DNew P(val);
		return Data[val] = r;
	}

	bool FindResourceByID( UINT /*rid*/, T& /*pVal*/ )
	{
		return true;
	}

	void DestroyResource(T val, XMLStreamer* _pXMLStreamer)
	{
		CBaseResource* p = FindResource(val);
		_ASSERT(p);
		p->SaveResourceHistory( _pXMLStreamer );
		delete p;
		Data.erase(val);
	}

	ResourceMap	Data;				// void* to RID
};

#define	DECLARE_FINDORCREATERESOURCE_SPECIALIZATION(TT,name)			\
	template <> void CreateResource<TT>( TT pVal )						\
	{	name.CreateResource( pVal );		}							\
																		\
	template <> void DestroyResource<TT>( TT pVal )						\
	{	name.DestroyResource( pVal, pXMLStreamer );		}				\
																		\
	template <> CBaseResource* FindOrCreateResource<TT>( TT pVal )		\
	{	return name.FindOrCreateResource( pVal );		}				\
																		\
	template <> CBaseResource* FindResource<TT>( TT pVal )				\
	{	return name.FindResource( pVal );		}						\
																		\
	template <> bool FindResourceByID<TT>( UINT rid, TT& pVal )			\
	{	return name.FindResourceByID( rid, pVal );		}				\


class ResourceManager
{
public:
	typedef enum
	{
		TRMMode_SaveHistory,
		TRMMode_LoadHistory
	}
	TResourceHistoryMode;

	ResourceManager		( TResourceHistoryMode historymode = TRMMode_SaveHistory );
	~ResourceManager	();

	void					OpenFile				( D3D10DDI_HDEVICE hDevice );

	template <typename T>
	void					CreateResource			( T	pVal );

	template <typename T>
	void					DestroyResource			( T	pVal );

	template <typename T>
	CBaseResource*			FindOrCreateResource	( T	pVal );

	template <typename T>
	CBaseResource*			FindResource			( T	pVal );

	void	AddResource		(D3D10DDIARG_CREATERESOURCE& Resource, D3D10DDI_HRESOURCE hResource, D3DDeviceWrapper *pWrapper);
	void	AddShader		(CONST UINT*  pCode, D3D10DDI_HSHADER  hShader, D3DDeviceWrapper *pWrapper);

	template <typename T>
	int						GetID					( T	val ) 
	{ 
		if (val.pDrvPrivate != NULL)
			return FindOrCreateResource(val)->GetResourceID();
		return 0;
	}

	template <typename T>
	CBaseResource*			GetResource				( T	val ) 
	{ 
		return FindResource(val);
	}

	template <typename T>
	bool					FindResourceByID		( UINT nRef, T& pVal ); 

	template <typename T>
	void					SaveInitialResourceData		( T val, D3DDeviceWrapper* pWrapper, DWORD EventID )
	{
		CBaseResource* p = FindResource(val);
		_ASSERT(p);
		p->SaveInitialResourceData(pWrapper, pArchiveFile, EventID);	
	}

	template <typename T>
	void					SaveResourceData		( T val, D3DDeviceWrapper* pWrapper, DWORD EventID )
	{
		CBaseResource* p = FindResource(val);
		_ASSERT(p);
		p->SaveResourceData(pWrapper, pArchiveFile, EventID);	
	}
	
	void	SaveResourceHistory		();
	void	LoadResourceHistory		(const wchar_t* fileName);

	DECLARE_FINDORCREATERESOURCE_SPECIALIZATION( D3D10DDI_HADAPTER,		Adapters_	)
	DECLARE_FINDORCREATERESOURCE_SPECIALIZATION( D3D10DDI_HDEVICE, 		Devices_	)
	DECLARE_FINDORCREATERESOURCE_SPECIALIZATION( D3D10DDI_HRESOURCE,  	Resources_	)

	DECLARE_FINDORCREATERESOURCE_SPECIALIZATION( D3D10DDI_HSHADERRESOURCEVIEW,	ShaderResourceViews_	)
	DECLARE_FINDORCREATERESOURCE_SPECIALIZATION( D3D10DDI_HRENDERTARGETVIEW,	RenderTargetViews_		)
	DECLARE_FINDORCREATERESOURCE_SPECIALIZATION( D3D10DDI_HDEPTHSTENCILVIEW,	DepthStencilView_		)
	DECLARE_FINDORCREATERESOURCE_SPECIALIZATION( D3D10DDI_HSHADER,				Shaders_				)
	DECLARE_FINDORCREATERESOURCE_SPECIALIZATION( D3D10DDI_HELEMENTLAYOUT,		Layouts_				)
	DECLARE_FINDORCREATERESOURCE_SPECIALIZATION( D3D10DDI_HBLENDSTATE,			BlendStates_			)
	DECLARE_FINDORCREATERESOURCE_SPECIALIZATION( D3D10DDI_HDEPTHSTENCILSTATE,	DepthStencilSStates_	)
	DECLARE_FINDORCREATERESOURCE_SPECIALIZATION( D3D10DDI_HRASTERIZERSTATE,		RasterizerStates_		)
	DECLARE_FINDORCREATERESOURCE_SPECIALIZATION( D3D10DDI_HSAMPLER,				Samplers_				)
	DECLARE_FINDORCREATERESOURCE_SPECIALIZATION( D3D10DDI_HQUERY,				Queries_				)

	DECLARE_FINDORCREATERESOURCE_SPECIALIZATION( D3D11DDI_HUNORDEREDACCESSVIEW,	UnorderedAccessViews_	)
	DECLARE_FINDORCREATERESOURCE_SPECIALIZATION( D3D11DDI_HCOMMANDLIST,			CommandLists_			)

	ResourceType< D3D10DDI_HADAPTER, CEmptyResource<D3D10DDI_HADAPTER> >		Adapters_;
	ResourceType< D3D10DDI_HDEVICE, CEmptyResource<D3D10DDI_HDEVICE> >			Devices_;
	ResourceType< D3D10DDI_HRESOURCE, CResource >								Resources_;

	//	DX10 resources are:
	//	Resource	== Buffer || Texture
	//		Buffer	== Vertex buffer || Index buffer || Constant buffer
	//		Texture	== Texture 1D    || Texture 2D   || Texture 3D
	//
	//	UM D3D10 driver resources:
	//	typedef enum D3D10DDIRESOURCE_TYPE {
	//		D3D10DDIRESOURCE_BUFFER  = 1,
	//		D3D10DDIRESOURCE_TEXTURE1D  = 2,
	//		D3D10DDIRESOURCE_TEXTURE2D  = 3,
	//		D3D10DDIRESOURCE_TEXTURE3D  = 4,
	//		D3D10DDIRESOURCE_TEXTURECUBE  = 5,
	//	} D3D10DDIRESOURCE_TYPE;

	ResourceType< D3D10DDI_HSHADERRESOURCEVIEW, CEmptyResource<D3D10DDI_HSHADERRESOURCEVIEW> >	ShaderResourceViews_;
	ResourceType< D3D10DDI_HRENDERTARGETVIEW, CEmptyResource<D3D10DDI_HRENDERTARGETVIEW> >		RenderTargetViews_;
	ResourceType< D3D10DDI_HDEPTHSTENCILVIEW, CEmptyResource<D3D10DDI_HDEPTHSTENCILVIEW> >		DepthStencilView_;
	ResourceType< D3D10DDI_HSHADER, CShader >													Shaders_;
	ResourceType< D3D10DDI_HELEMENTLAYOUT, CEmptyResource<D3D10DDI_HELEMENTLAYOUT> >			Layouts_;
	ResourceType< D3D10DDI_HBLENDSTATE, CEmptyResource<D3D10DDI_HBLENDSTATE> >					BlendStates_;
	ResourceType< D3D10DDI_HDEPTHSTENCILSTATE, CEmptyResource<D3D10DDI_HDEPTHSTENCILSTATE> >	DepthStencilSStates_;
	ResourceType< D3D10DDI_HRASTERIZERSTATE, CEmptyResource<D3D10DDI_HRASTERIZERSTATE> >		RasterizerStates_;
	ResourceType< D3D10DDI_HSAMPLER, CEmptyResource<D3D10DDI_HSAMPLER> >						Samplers_;
	ResourceType< D3D10DDI_HQUERY, CEmptyResource<D3D10DDI_HQUERY> >							Queries_;

	ResourceType< D3D11DDI_HUNORDEREDACCESSVIEW, CEmptyResource<D3D11DDI_HUNORDEREDACCESSVIEW> > UnorderedAccessViews_;
	ResourceType< D3D11DDI_HCOMMANDLIST, CEmptyResource<D3D11DDI_HCOMMANDLIST> >				CommandLists_;

	XMLStreamer *			pXMLStreamer;

protected:
private:	
	ArchiveFile *			pArchiveFile;
	TResourceHistoryMode	HistoryMode;
};

//	TODO:
//	We need more specific link for ResourceManager because each Device needs its own manager,
//	so global g_ResourceManager will not work; Let's add D3DDeviceWrapper.pResourceManager_ and call
//	g_XMLStreamer->SetDevice(pWrapper->pResourceManager_) in WRITE_COMMAND_BEGIN macro;
//  Thus the Streamer will remember 


#ifndef FINAL_RELEASE
	#define GET_RESOURCE(hResource)								\
	(															\
		pWrapper->m_ResourceManager.FindResource(hResource)		\
	)
	#define DESTROY_RESOURCE(hResource)							\
	(															\
		pWrapper->m_ResourceManager.DestroyResource(hResource)	\
	)
#else
	#define GET_RESOURCE(hResource)
	#define DESTROY_RESOURCE(hResource)
#endif

template <typename T>
inline std::size_t hash_value(T const& d3dHandle)
{
	boost::hash<void*> hasher;
	return hasher((void*)d3dHandle.pDrvPrivate);
}

namespace std
{
	template<>
	class hash<D3D10DDI_HRESOURCE>
		: public unary_function<void*, size_t>
	{	// hash functor
	public:
		typedef D3D10DDI_HRESOURCE _Kty;
		typedef _Uint32t _Inttype;	// use first (1 or 2)*32 bits

		size_t operator()(_Kty _Keyval) const
		{
			return (std::hash<void*>()(_Keyval.pDrvPrivate));
		}
	};

}


#endif//__S3D_RESOURCEMANAGER_H__