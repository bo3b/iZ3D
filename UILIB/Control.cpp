#include <d3d9.h>
#include <d3dx9.h>
#include "UILIB.h"

CXControl::CXControl( iz3d::uilib::CommonDevice* pDevice )
	: m_pDevice( pDevice ),
	m_pChildControls( 0 ),
	m_pNextSibling( 0 ),
	m_pPreviousSibling( 0 ),
	m_pParent( 0 ),
	m_Visible( true ),
	m_bFocus( false ),
	m_pFocusControl( 0 ),
	m_Width( 0 ),
	m_Height( 0 ),
	m_pCanvas( 0 )
{
	m_pDefaultCanvas = new CXCanvas( pDevice );
	m_pCanvas = m_pDefaultCanvas;
	m_pPen = new CXPen( m_pDevice );
	m_Position.x = m_Position.y = 0;
}

CXControl::~CXControl()
{
	delete m_pDefaultCanvas;
	if( m_pCanvas != m_pDefaultCanvas )
		SAFE_DELETE( m_pCanvas );
	if( !m_pParent ) 
		SAFE_DELETE( m_pPen );

	RemoveAllChildren();
}

void CXControl::SetXYPos( LONG X, LONG Y )
{
	m_Position.x = X;
	m_Position.y = Y;
}

CXControl* CXControl::AddChildControl( CXControl* pControl )
{
	pControl->SetParentControl( this );

	CXPen* Pen = pControl->m_pPen;

	SAFE_DELETE( Pen );

	pControl->SetPen( m_pPen );

	if(!m_pChildControls)
		m_pChildControls = pControl;
	else 
	{
		CXControl* pTemp = m_pChildControls;

		while( pTemp->GetNextSibling() )
			pTemp = pTemp->GetNextSibling();

		pTemp->SetNextSibling( pControl );
		pControl->SetPreviousSibling( pTemp );
	}

	return pControl;
}

CXControl* CXControl::RemoveChildControl(CXControl* pControl)
{
	CXControl* pNext = pControl->m_pNextSibling;
	CXControl* pPrevious = pControl->m_pPreviousSibling;

	SAFE_DELETE( pControl );

	pNext->m_pPreviousSibling = pPrevious;
	pControl = pNext;
	return pControl;
}

void CXControl::RemoveAllChildren()
{
	CXControl* pTemp = m_pChildControls;

	while( pTemp )
	{
		CXControl* Next = pTemp->m_pNextSibling;

		if( pTemp )
			delete pTemp;

		pTemp = Next;
	}

	m_pChildControls = NULL;
	m_pFocusControl = NULL;
}

int CXControl::GetChildCount()
{
	int Count = 0;
	CXControl* pTemp =  GetFirstChild();

	while( pTemp )
	{
		CXControl* Next = pTemp->GetNextSibling();
		Count++;
		pTemp = Next;
	}
	return Count;
}

bool CXControl::LoadCanvasFromFile( TCHAR* pStrFileName )
{
	D3DXIMAGE_INFO Info;
	auto hr = D3DXGetImageInfoFromFile( pStrFileName, &Info );
	if( FAILED( hr ) )
		return false;

	SetWidthHeight( Info.Width, Info.Height );
	hr = m_pCanvas->LoadFromFile( pStrFileName );
	if( FAILED( hr ) )
		return false;

	return true;
}

void CXControl::SetWidthHeight( LONG Width, LONG Height )
{
	// it calls virtual functions!!!
	SetWidth( Width );
	SetHeight( Height );
}

bool CXControl::PostMessage( UINT msg, WPARAM wParam, LPARAM lParam, void* pData )
{
	switch( msg )
	{
	case WM_PAINT://Render
		{
			if( GetVisible())
			{
				OnRender();

				if( m_pChildControls ) 
					PostToAllReverse( m_pChildControls, msg, wParam, lParam, pData );
			}
			break;
		}

	case WM_MOUSEMOVE:
		{
			OnMouseMove( LOWORD( lParam ), HIWORD( lParam ) );

			PostToAll( msg, wParam, lParam, pData );

			break;
		}


	case WM_LBUTTONDBLCLK:
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_MBUTTONDBLCLK:
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_RBUTTONDBLCLK: 
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
		{
			if( !GetVisible() )
				break;
			if( !CursorIntersect( LOWORD( lParam ), HIWORD( lParam ) ) )
				return false;
			CXControl* Control = PostToAll(msg, wParam, lParam, pData);
			if( Control )
				return true;
			if( ( msg == WM_LBUTTONDBLCLK ) ||
				( msg == WM_LBUTTONDOWN ) ||
				( msg == WM_MBUTTONDBLCLK ) ||
				( msg == WM_MBUTTONDOWN ) ||
				( msg == WM_RBUTTONDBLCLK ) ||
				( msg == WM_RBUTTONDOWN ) )
			{
				OnMouseDown( msg, LOWORD( lParam ), HIWORD( lParam ) );

				if( GetParentControl() )
					GetParentControl()->MoveToFront( this );

				SetFocusControl( this );
			}
			else
				OnMouseUp( msg, LOWORD( lParam ), HIWORD( lParam ) );
			return true;
		}

	case WM_KEYUP:
	case WM_KEYDOWN:
	case WM_CHAR:
		{
			if( !GetFocusControl() )
				break;
			switch( msg )
			{
			case WM_KEYUP:
				GetFocusControl()->OnKeyUp( wParam, lParam );
				break;
			case WM_KEYDOWN:
				GetFocusControl()->OnSysKey( wParam, lParam );
				break;
			case WM_CHAR:
				GetFocusControl()->OnKeyDown( wParam, lParam );
				break;
			default:
				break;
			}
		}
	}

	return false;
}

CXControl* CXControl::PostToAll( UINT msg, WPARAM wParam, LPARAM lParam, void* pData )
{
	CXControl* pTemp =  GetFirstChild();

	while( pTemp )
	{

		CXControl* Next = pTemp->GetNextSibling();	   
		if( pTemp->PostMessage( msg, wParam, lParam, pData ) )
			return pTemp;

		pTemp = Next;
	}

	return NULL;
}

//Post a message to all controls in reverse order- right to left
CXControl* CXControl::PostToAllReverse( CXControl* pControl, UINT msg, WPARAM wParam, LPARAM lParam, void* pData )
{
	CXControl* pNext = pControl->GetNextSibling();	   

	if(pNext)
		pNext->PostToAllReverse( pNext, msg, wParam, lParam, pData );

	pControl->PostMessage( msg, wParam, lParam, pData );

	return NULL;
}

//Move a control to the front of the queue and repair the chain

void CXControl::MoveToFront( CXControl* pControl )
{
	if( !pControl )
		return;

	CXControl* pNext = pControl->GetNextSibling(); //Next
	CXControl* pPrevious = pControl->GetPreviousSibling(); //Previous

	if( !pPrevious )
		return;
	
	pPrevious->SetNextSibling( pNext );
	if(pNext)
		pNext->SetPreviousSibling(  pPrevious);

	pControl->SetNextSibling( m_pChildControls );
	m_pChildControls->SetPreviousSibling( pControl );
	pControl->SetPreviousSibling( 0 );
	m_pChildControls = pControl;
}

void CXControl::GetAbsolutePosition( POINT* pPosition )
{
	pPosition->x +=  GetXPos();
	pPosition->y +=  GetYPos();

	if( m_pParent )
		 m_pParent->GetAbsolutePosition( pPosition );
}

HRESULT CXControl::RenderMe()
{
	static POINT ControlAbsolutePos;

	HRESULT Result = E_FAIL;

	ControlAbsolutePos.x = 0;
	ControlAbsolutePos.y = 0;

	 GetAbsolutePosition( &ControlAbsolutePos );


	if ( m_pCanvas->GetTexture() )
	{
		POINTF Scale;
		auto WidthHeight = m_pCanvas->GetTexture()->GetWidthHeight();
		Scale.x = ( float )m_Width / WidthHeight.cx;
		Scale.y = ( float )m_Height/ WidthHeight.cy;
		m_pCanvas->SetScaling( Scale );
	}

	m_pCanvas->SetTranslation( ControlAbsolutePos );
	if( m_pPen )
		Result =  m_pPen->DrawTexture( m_pCanvas );

	m_pCanvas->SetTranslation( POINT() );

	return Result;
}


//Returns true or false to indicate whether a 
//specified coordinate (I.E Mouse) exists within
//the frame of a control

bool CXControl::CursorIntersect( int X, int Y )
{
	POINT ControlAbsolutePos;
	ControlAbsolutePos.x = 0;
	ControlAbsolutePos.y = 0;

	GetAbsolutePosition( &ControlAbsolutePos );

	if( ( X >= ControlAbsolutePos.x ) &&
		( X <= ControlAbsolutePos.x + m_Width ) &&
		( Y >= ControlAbsolutePos.y ) &&
		( Y <= ControlAbsolutePos.y + m_Height ) )
		return true;

	return false;
}

void CXControl::SetFocusControl( CXControl* pControl )
{
	if( pControl->GetFocus() )
		return;

	if( m_pParent )
		m_pParent->SetFocusControl(pControl);
	else
	{
		if( m_pFocusControl )
			m_pFocusControl->SetFocus( false );
		m_pFocusControl = pControl;
		pControl->SetFocus(true);
	}
}

void CXControl::SetFocus( bool State )
{
	if( State )
	{
		OnSetFocus();
		m_bFocus = true;
	}
	else
	{
		OnLostFocus();
		m_bFocus = false;
	}
}

bool CXControl::LoadCanvasFromResource( HMODULE hSrcModule, LPCTSTR pSrcResource )
{
	D3DXIMAGE_INFO Info;

	HRESULT hr = D3DXGetImageInfoFromResource( hSrcModule, pSrcResource, &Info );
	if( FAILED( hr ))
		return false;
	
	SetWidthHeight( Info.Width, Info.Height );
	hr = m_pCanvas->LoadFromResource( hSrcModule, pSrcResource );
	if( FAILED( hr ) )
		return false;
	return true;
}

bool CXControl::LoadCanvasFromMemory( size_t size, const void* pData )
{
	D3DXIMAGE_INFO Info;

	HRESULT hr = D3DXGetImageInfoFromFileInMemory( pData, (UINT)size, &Info );
	if( FAILED( hr ) )
		return false;

	SetWidthHeight( Info.Width, Info.Height );
	hr = m_pCanvas->LoadFromMemory( size, pData );
	if( FAILED( hr ) )
		return false;
	return true;
}

bool CXControl::LoadCanvas( ICommonTexture* pTex )
{
	if (pTex == NULL)
		return false;
	m_pCanvas->SetTexture( pTex );
	return true;
}

void CXControl::SetRight()
{
	ProcessStereo( false );
}

void CXControl::ProcessStereo( bool isLeft )
{
	if( m_pCanvas->IsStereo() )
		isLeft ? m_pCanvas->SetLeft() : m_pCanvas->SetRight();
	CXControl* pContr = GetFirstChild();
	while( pContr )
	{
		pContr->ProcessStereo( isLeft );
		pContr = pContr->GetNextSibling();
	}
}

void CXControl::SetLeft()
{
	ProcessStereo( true );
}