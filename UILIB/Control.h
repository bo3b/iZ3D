#pragma once
#include "CommonDevice.h"
class CXControl
{
	//Status
	POINT m_Position; //Control's relative position
	LONG m_Width;
	LONG m_Height;
	bool m_Visible;
	bool m_bFocus;

	CXCanvas* m_pCanvas;
	CXCanvas* m_pDefaultCanvas;
	CXPen* m_pPen;
	iz3d::uilib::CommonDevice* m_pDevice;

    CXControl* m_pChildControls;
	CXControl* m_pNextSibling;
	CXControl* m_pPreviousSibling;
	CXControl* m_pParent;
	CXControl* m_pFocusControl;

	CXControl* PostToAll( UINT msg, WPARAM wParam, LPARAM lParam, void* Data );

	void ProcessStereo( bool isLeft );
public:

	CXControl( iz3d::uilib::CommonDevice* pDevice );
	virtual ~CXControl();

	CXCanvas* GetCanvas() { return m_pCanvas; }
	void SetCanvas( CXCanvas* pTexture )		{ m_pCanvas = pTexture; }

    iz3d::uilib::CommonDevice* GetDevice()		{ return m_pDevice; }
	void SetDevice( iz3d::uilib::CommonDevice* pDevice ) { m_pDevice = pDevice; }

    bool GetVisible() const { return m_Visible; }
	void SetVisible( bool Visible ) { m_Visible = Visible; }

	CXPen* GetPen() { return m_pPen; }
	void SetPen( CXPen* pPen )					{ m_pPen = pPen; }
	
	POINT* GetPosition()						{ return &m_Position; }
	LONG GetXPos() const						{ return m_Position.x; }
	LONG GetYPos() const						{ return m_Position.y; }
	void SetXPos( LONG X )						{ m_Position.x = X; }
	void SetYPos( LONG Y )						{ m_Position.y = Y; }
	void SetXYPos( LONG X, LONG Y );
	void GetAbsolutePosition( POINT* pPosition );

	LONG GetWidth() const							{ return m_Width; }
	LONG GetHeight() const							{ return m_Height; }
	virtual void SetWidth( LONG Width )				{ m_Width = Width; }
	virtual void SetHeight( LONG Height )			{ m_Height = Height; }
	void SetWidthHeight( LONG Width, LONG Height );

    CXControl* GetParentControl()					{ return m_pParent; }
	void SetParentControl( CXControl* pControl )	{ m_pParent = pControl; }
	CXControl* GetNextSibling()						{ return m_pNextSibling; }
	void SetNextSibling( CXControl* pControl )		{ m_pNextSibling = pControl; }
	CXControl* GetPreviousSibling()					{ return m_pPreviousSibling; }
	void SetPreviousSibling( CXControl* pControl )	{ m_pPreviousSibling = pControl; }
	CXControl* GetFirstChild()						{return m_pChildControls;}
	void SetFirstChild( CXControl* pControl )		{ m_pChildControls = pControl; }
    CXControl* GetNextChild( CXControl* pControl )	{ return pControl->GetNextSibling(); }
	CXControl* GetFocusControl()					{ return m_pFocusControl; }
	void SetFocusControl( CXControl* pControl );

	CXControl* AddChildControl( CXControl* pControl );
	CXControl* RemoveChildControl( CXControl* pControl );
	void RemoveAllChildren();
	int GetChildCount();

	bool LoadCanvas( ICommonTexture* pTex );
	bool LoadCanvasFromFile( TCHAR* pStrFileName );
	bool LoadCanvasFromResource( HMODULE hSrcModule, LPCTSTR pSrcResource );
	bool LoadCanvasFromMemory( size_t size, const void* pData );

    //Posts a notification message to the control
	bool PostMessage( UINT msg, WPARAM wParam, LPARAM lParam, void* Data );
    CXControl* PostToAllReverse( CXControl* pControl, UINT msg, WPARAM wParam, LPARAM lParam, void* pData );

	void MoveToFront( CXControl* pControl );

	bool CursorIntersect( int X, int Y );

	bool GetFocus() const							{ return m_bFocus; }
	void SetFocus( bool State );

	void SetRight();
	void SetLeft();
	HRESULT RenderMe();

	virtual bool OnRender() = NULL;
    virtual void OnMouseDown( int Button, int X, int Y ) = NULL;
	virtual void OnMouseMove( int X, int Y ) = NULL;
	virtual void OnMouseUp( int Button, int X, int Y ) = NULL;
	virtual void OnSetFocus() = NULL;
	virtual void OnLostFocus() = NULL;
	virtual void OnKeyDown( WPARAM Key, LPARAM Extended ) = NULL;
    virtual void OnKeyUp( WPARAM Key, LPARAM Extended ) = NULL;
	virtual void OnSysKey( WPARAM Key, LPARAM Extended ) = NULL;
};

