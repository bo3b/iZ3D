#include <d3d9.h>
#include <d3dx9.h>

#include "uilib.h"
#include "ExtWindow.h"
#include "..\S3DAPI\GlobalData.h"
using namespace uilib;
std::wstring CXExtWindow::m_uncheckedText = _T("");
std::wstring CXExtWindow::m_checkedText = _T("");

CXExtWindow::CXExtWindow( iz3d::uilib::CommonDevice* pDev ):
CXWindow( pDev )
{
	m_prevSt = !gInfo.Input.ShowWizardAtStartup;
}

CXExtWindow::~CXExtWindow(void)
{
}

void CXExtWindow::OnSysKey( WPARAM Key, LPARAM )
{
	TReacts::iterator i = m_Reacts.find(Key);
	if ( i == m_Reacts.end() )
	{
		return;
	}
	if (!i->second)
	{
	//	m_pWizard->Hide();
		//m_pParent->SetVisible(false);
		return;
	}
	this->SetVisible(false);
	i->second->SetVisible(true);
	GetParentControl()->SetFocusControl( i-> second );
}

void CXExtWindow::AddReact( WPARAM Key, CXWindow* pWindow )
{
	m_Reacts[Key] = pWindow;
}

bool CXExtWindow::OnRender()
{
	if (m_prevSt != gInfo.Input.ShowWizardAtStartup)
	{
		CXCheckBox* pBox = (CXCheckBox*)GetFirstChild();
		pBox->SetCheckedState(gInfo.Input.ShowWizardAtStartup != 0);
		m_prevSt = gInfo.Input.ShowWizardAtStartup;
		if (m_prevSt)
			pBox->SetCaption(m_checkedText.c_str());
		else
			pBox->SetCaption(m_uncheckedText.c_str());
	}
	return CXWindow::OnRender();
}

void CXExtWindow::SetCheckedText( const TCHAR* cT )
{
	m_checkedText = cT;
}

void CXExtWindow::SetUncheckedText( const TCHAR* uT )
{
	m_uncheckedText = uT;
}