#pragma once
#include "uilib.h"
#include <string>
#include <map>
#include <d3d9.h>
#include "Framework.h"
#include "..\S3DAPI\GlobalData.h"
#include <boost/serialization/access.hpp>
#include "ICommonDrawer.h"
#include "CommonDevice.h"

class CBaseSwapChain;
class CXDesktopWindow;
class CXWindow;
class CXExtWindow;
class CXLabel;
class CXCanvas;
class D3DDeviceWrapper;

struct D3D10DDI_DEVICEFUNCS;
struct D3D10DDI_HDEVICE;
struct D3D10DDI_HRENDERTARGETVIEW;
namespace uilib
{
	
	typedef std::map< std::basic_string< TCHAR >,  ICommonTexture*> TextureMap_t;
	class TexturesManager
	{
		iz3d::uilib::CommonDevice* m_pDevice;
		TextureMap_t m_Data;
	public:
		TexturesManager() : m_pDevice( 0 ) { }
		void SetDevice( iz3d::uilib::CommonDevice* pDevice ) { m_pDevice = pDevice; }
		void Clear() { 
			if( !m_Data.empty() )
				m_Data.clear();
		}
		ICommonTexture* GetImage( const wchar_t* pResName );
	};

	class WizardSCData;

	class Wizard
	{
		// constants
		static const TCHAR* STR_THISDLLNAME;
		static const HMODULE s_hModule;


		// fields
		TexturesManager m_TexManager;
		iz3d::uilib::CommonDevice* m_pDevice;
		// static fields
		static bool s_bEnabled;

		HRESULT CommonInitialize();
	public:
		static const LOGFONT s_lf;
		static const LOGFONT s_Notlf;
		HRESULT Initialize( IDirect3DDevice9* pD3DDevice );
		HRESULT Initialize( ICommonDrawer* pDrawer );
		iz3d::uilib::CommonDevice* GetDevice() { return m_pDevice; }
		void Clear();
		Wizard();
		~Wizard();
		friend WizardSCData;
	};
}
