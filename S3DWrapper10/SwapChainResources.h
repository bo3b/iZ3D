#pragma once

#include <boost/unordered_map.hpp>
#include "Presenter.h"

class ResourceWrapper;

class SwapChainResources
{
	friend class D3DSwapChain;
public:
	SwapChainResources();

	void	Initialize(D3DSwapChain* pSwapChain);

	void	Clear();

	void						InitializePresenterResources	();
	void						ClearPresenterResources			();

	bool						IsSwapEyes() const			{ _ASSERT( m_pDevice ); return m_pDevice->m_Input.SwapEyes != 0; };
	bool						IsStereoActive() const		{ _ASSERT( m_pDevice ); return m_pDevice->m_Input.StereoActive != 0; };

	ResourceWrapper*			GetBackBufferPrimaryWrap()	const { return m_pBackBufferPrimaryWrap; };

	// primary resources
	D3D10DDI_HRESOURCE			GetBackBufferPrimary()		const { return m_hBackBufferPrimary; };
	D3D10DDI_HRENDERTARGETVIEW	GetBackBufferRTPrimary()	const { return m_hBackBufferRTPrimary; };

	// secondary resources
	D3D10DDI_HRESOURCE			GetBackBufferSecondary()	const { return m_hBackBufferSecondary; };
	D3D10DDI_HRENDERTARGETVIEW	GetBackBufferRTSecondary()	const { return m_hBackBufferRTSecondary; };
	
	// left resources
	D3D10DDI_HRESOURCE			GetMethodResourceLeft()		const
	{ 
		if ( !IsStereoActive() || !IsSwapEyes() )
			return m_hMethodResourceLeft;
		else
			return m_hMethodResourceRight;
	};

	D3D10DDI_HSHADERRESOURCEVIEW	GetMethodTextureLeft()		const
	{
		if ( !IsStereoActive() || !IsSwapEyes() )
			return m_hMethodTextureLeft;
		else
			return m_hMethodTextureRight;
	};

	D3D10DDI_HRESOURCE			GetMethodResourceRight()	const
	{
		if ( IsStereoActive() && !IsSwapEyes() )
			return m_hMethodResourceRight;
		else
			return m_hMethodResourceLeft;
	};

	D3D10DDI_HSHADERRESOURCEVIEW	GetMethodTextureRight()		const
	{
		if ( IsStereoActive() && !IsSwapEyes() )
			return m_hMethodTextureRight;
		else
			return m_hMethodTextureLeft;
	};

	D3D10DDI_HRESOURCE	GetBackBufferLeft()					const
	{
		if ( !IsStereoActive() || !IsSwapEyes() )
			return m_hBackBufferLeft;
		else
			return m_hBackBufferRight;
	};

	D3D10DDI_HRESOURCE	GetBackBufferRight()				const
	{
		if ( IsStereoActive() && !IsSwapEyes() )
			return m_hBackBufferRight;
		else
			return m_hBackBufferLeft;
	};

	inline bool	IsAAOn()		const
	{
		return m_bAAOn;
	}

	inline bool	IsInitialized()		const
	{
		return m_hBackBufferRTLeft.pDrvPrivate && m_hBackBufferRTRight.pDrvPrivate;
	}

	void	SetStereoRenderTarget( int view );

	D3D10DDI_HRESOURCE	GetPresenterResource( bool bLeft )	const
	{
		if ( bLeft )
			return m_hPresenterResourceLeft;
		else
			return m_hPresenterResourceRight;
	};

	D3D10DDI_HRESOURCE	GetPresenterResourceWrap()	const
	{
		return m_hPresenterResourceWrap;
	};

	void				SwapKMPresenterResources()
	{
		if( m_hPresenterResourceWrap2.pDrvPrivate )			// KM shutter only
		{
			std::swap( m_hPresenterResourceWrap,  m_hPresenterResourceWrap2  );
			std::swap( m_hPresenterResourceLeft,  m_hPresenterResourceLeft2  );
			std::swap( m_hPresenterResourceRight, m_hPresenterResourceRight2 );
		}
	}

	void				RotateKMShutterMarkerIdentities	()
	{
		m_uKMShutterMarkerResourceIndex = (m_uKMShutterMarkerResourceIndex + 1) % KM_SHUTTER_SERVICE_BUFFER_COUNT;
	}

	D3D10DDI_HRESOURCE	GetKMShutterMarkerResource		()	const
	{
		_ASSERT( m_uKMShutterMarkerResourceIndex < KM_SHUTTER_SERVICE_BUFFER_COUNT );
		return	m_hKMShutterMarkerResource[ m_uKMShutterMarkerResourceIndex ];
	}

	unsigned			GetKMShutterMarkerResourceIndex	()	const
	{
		return	m_uKMShutterMarkerResourceIndex;
	}

	D3D10DDI_HRESOURCE	GetKMShutterPrimaryResource	( UINT indx ) const
	{
		_ASSERT( indx < KM_SHUTTER_SERVICE_BUFFER_COUNT );
		return	m_hKMShutterPrimaryResource[ indx ];
	}

protected:
	class D3DDeviceWrapper*				m_pDevice;

	bool								m_bAAOn;

	ResourceWrapper*					m_pBackBufferPrimaryWrap;
	ResourceWrapper*					m_pBackBufferPrimaryBeforeScalingWrap;
	D3D10DDI_HRESOURCE					m_hBackBufferPrimary;
	D3D10DDI_HRENDERTARGETVIEW			m_hBackBufferRTPrimary;

	D3D10DDI_HRESOURCE					m_hBackBufferSecondary;
	D3D10DDI_HRENDERTARGETVIEW			m_hBackBufferRTSecondary;

	D3D10DDI_HRESOURCE					m_hScaledMethodResource;
	D3D10DDI_HRESOURCE					m_hMethodResourceLeft;
	D3D10DDI_HSHADERRESOURCEVIEW		m_hMethodTextureLeft;
	D3D10DDI_HRESOURCE					m_hMethodResourceRight;
	D3D10DDI_HSHADERRESOURCEVIEW		m_hMethodTextureRight;

	D3D10DDI_HRESOURCE					m_hBackBufferLeftBeforeScaling; // don't destroy
	D3D10DDI_HRESOURCE					m_hBackBufferLeft; 
	D3D10DDI_HRENDERTARGETVIEW			m_hBackBufferRTLeft;
	D3D10DDI_HRESOURCE					m_hBackBufferRightBeforeScaling; // don't destroy
	D3D10DDI_HRESOURCE					m_hBackBufferRight; 
	D3D10DDI_HRENDERTARGETVIEW			m_hBackBufferRTRight;
	
	D3D10DDI_HRESOURCE					m_hPresenterResourceWrap;
	D3D10DDI_HRESOURCE					m_hPresenterResourceLeft;
	D3D10DDI_HRESOURCE					m_hPresenterResourceRight;

	D3D10DDI_HRESOURCE					m_hPresenterResourceWrap2;		// KM shutter only
	D3D10DDI_HRESOURCE					m_hPresenterResourceLeft2;
	D3D10DDI_HRESOURCE					m_hPresenterResourceRight2;

	D3D10DDI_HRESOURCE					m_hKMShutterMarkerResource	[ KM_SHUTTER_SERVICE_BUFFER_COUNT ];
	unsigned							m_uKMShutterMarkerResourceIndex;

	D3D10DDI_HRESOURCE					m_hKMShutterPrimaryResource	[ KM_SHUTTER_SERVICE_BUFFER_COUNT ];
	unsigned							m_uKMShutterPrimaryResourceIndex;
};

//////////////////////////////////////////////////////////////////////////

#ifndef _DEBUG
typedef boost::unordered_map<D3D10DDI_HRESOURCE, SwapChainResources> PrimaryResourcesMap;
#else
typedef std::map<D3D10DDI_HRESOURCE, SwapChainResources> PrimaryResourcesMap;
#endif
