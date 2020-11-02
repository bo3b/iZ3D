#pragma once
#include "CommonDevice.h"
class CXButton : public CXControl
{
protected:
	HFONT m_hFont;
	CXLabel* m_pCaption;
	CXCanvas* m_pDefaultImage;
	CXCanvas* m_pPressedImage;
	bool m_Pressed;
public:
	CXButton( iz3d::uilib::CommonDevice* pDevice );
	virtual ~CXButton();

	bool OnRender();
	void OnMouseDown( int Button, int X, int Y );
	void OnMouseMove( int X, int Y );
	void OnMouseUp( int Button, int X, int Y );
	void OnSetFocus();
    void OnLostFocus();
	void OnKeyDown( WPARAM Key, LPARAM Extended );
	void OnKeyUp( WPARAM Key, LPARAM Extended );
	void OnSysKey (WPARAM Key, LPARAM Extended );

	void SetCaption( const TCHAR* pStrCaption ) { m_pCaption->SetCaption( pStrCaption ); }
	bool SetDefaultImage( ICommonTexture* pTex );
	bool SetDefaultImage( TCHAR* pStrFileName );
	bool SetDefaultImage( HMODULE hSrcModule, LPCTSTR pSrcResource );
	bool SetDefaultImage( size_t size, const void* pData );
	bool SetPressedImage( ICommonTexture* pTex );
	bool SetPressedImage( TCHAR* pStrFileName );
	bool SetPressedImage( HMODULE hSrcModule, LPCTSTR pSrcResource );
	bool SetPressedImage( size_t size, const void* pData );
	bool GetPressed() const { return m_Pressed; }
	void SetPressed( bool isPressed ){ m_Pressed = isPressed; }
	virtual void SetWidth( LONG Width );
	virtual void SetHeight( LONG Height );
};