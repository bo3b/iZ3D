#pragma once
#include "Control.h"

class CXButton;
class CXLabel;

class CXCheckBox : public CXControl
{
protected:
	bool m_Checked;
	CXButton* m_pTickBox;
	CXLabel* m_pLabel;
public:
	CXCheckBox( const LOGFONT& lf, iz3d::uilib::CommonDevice* pDevice );
	virtual ~CXCheckBox();
	bool GetCheckedState() const { return m_Checked; }
	void SetCheckedState( bool State ) { m_Checked = State; m_pTickBox->SetPressed( State ); }

	bool LoadCheckedImage( ICommonTexture* pTex );
	bool LoadUncheckedImage( ICommonTexture* pTex );
	bool LoadCheckedImage( TCHAR* pStrFileName );
	bool LoadUncheckedImage( TCHAR* pStrFileName );
	bool LoadCheckedImage( HMODULE hSrcModule, LPCTSTR pSrcResource );
	bool LoadUncheckedImage( HMODULE hSrcModule, LPCTSTR pSrcResource );
	bool LoadCheckedImage( size_t size, const void* pData );
	bool LoadUncheckedImage( size_t size, const void* pData );
	void SetCaption( const TCHAR* pStrText );
	const TCHAR* GetCaption() const { return m_pLabel->GetCaption(); }
	bool OnRender();
	void OnMouseDown( int Button, int X, int Y );
	void OnMouseMove( int X, int Y );
	void OnMouseUp( int Button, int X, int Y );
	void OnSetFocus();
    void OnLostFocus();
	void OnKeyDown( WPARAM Key, LPARAM Extended );
	void OnKeyUp( WPARAM Key, LPARAM Extended );
	void OnSysKey( WPARAM Key, LPARAM Extended );
	void SetCaptionColour( DWORD colour );
};