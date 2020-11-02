#pragma  once

#include <atlbase.h>
#include "CommonDevice.h"

class CXScaler;
struct IDirect3DDevice9;
struct IDirect3DTexture9;

class CXCanvas
{
	iz3d::uilib::CommonDevice* m_pDevice;
	ICommonTexture* m_pTexture;
	POINT m_RotationCenter;
	POINT m_Translation;
	POINTF m_Scaling;
	FLOAT m_Rotation;
	RECT m_SrcRect;
	std::basic_string<TCHAR> m_Path;

	bool isStereo;
	
	bool isLeft;
	
	void init( iz3d::uilib::CommonDevice* pDevice );
public: 
	CXCanvas( iz3d::uilib::CommonDevice* pDevice);
	CXCanvas( iz3d::uilib::CommonDevice* pDevice, const TCHAR* Path );
	~CXCanvas();

	ICommonTexture* GetTexture() { return m_pTexture; }   
	void SetTexture( ICommonTexture* pTexture );

	iz3d::uilib::CommonDevice* GetDevice() { return m_pDevice; }
	void SetDevice(iz3d::uilib::CommonDevice* pDevice) { m_pDevice = pDevice; }

	void EnableStereo() { isStereo = true; }
	void DisableStereo(){ isStereo = false; }
	bool IsStereo() const {	return isStereo; }

	void SetLeft(){ isLeft = true; }
	void SetRight(){ isLeft = false; }
	bool IsLeft() const{ return isLeft; }
	bool IsRight() const{ return !isLeft; }

	const POINT GetRotationCenter () const{	return m_RotationCenter; }
	void SetRotationCenter( const POINT& RotationCenter ) {
		m_RotationCenter = RotationCenter;
	}
	
	POINT GetTranslation() const;
	void SetTranslation( const POINT& Translation ){ m_Translation = Translation; } 
	
	POINTF GetScaling() const;
	void SetScaling( const POINTF& Scaling ){ m_Scaling = Scaling; }
	
	FLOAT GetRotation() const { return m_Rotation; }
	void SetRotation( FLOAT Rotation ){ m_Rotation = Rotation; } 
	
	RECT GetRect() const;
	void SetRect( const RECT& SrcRect) { m_SrcRect = SrcRect; }
	
	LONG GetWidth() const{ return m_SrcRect.right - m_SrcRect.left;	}
	LONG GetHeight() const{	return m_SrcRect.bottom - m_SrcRect.top; }

	const TCHAR* GetPath() const { return m_Path.c_str(); }
	void SetPath( const TCHAR* Path ) { m_Path = Path; }

	HRESULT InvalidateDeviceObjects();
	HRESULT RestoreDeviceObjects();
	HRESULT LoadFromFile( const TCHAR* Path );
	HRESULT LoadFromResource( HMODULE hSrcModule, LPCTSTR pSrcResource);
	HRESULT LoadFromMemory( size_t size, const void* pData );

	HRESULT LoadTextureFomMemory( const void* pData, size_t size );
};

class CXPen
{
	iz3d::uilib::CommonDevice*	 m_pDevice;
	ICommonSprite* m_pSprite;
public:
	CXPen( iz3d::uilib::CommonDevice* pDevice );
	~CXPen();
	ICommonSprite* GetSprite() { return m_pSprite; }   
	void SetSprite( ICommonSprite* Sprite ) { m_pDevice->DeleteSprite( &m_pSprite ); m_pSprite = Sprite; }
	iz3d::uilib::CommonDevice* GetDevice() { return m_pDevice; }
	void SetDevice( iz3d::uilib::CommonDevice* pDevice ) { m_pDevice = pDevice; }
	HRESULT DrawTexture( CXCanvas* Texture );
	HRESULT InvalidateDeviceObjects();
	HRESULT RestoreDeviceObjects();
};

HRESULT LoadTextureFromMemory( IDirect3DDevice9* pDevice, const void* pSrcData,
	size_t SrcDataSize, IDirect3DTexture9** ppTexture );
