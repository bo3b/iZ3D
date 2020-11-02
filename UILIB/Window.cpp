#include <d3d9.h>
#include <d3dx9.h>
#include "uilib.h"

CXWindow::CXWindow(iz3d::uilib::CommonDevice* Device) : CXControl(Device)
{
	m_X = 0;
	m_Y = 0;
	m_Depressed = true;
	this->SetXYPos(0,0);
}

bool CXWindow::OnRender()
{
	if(!GetVisible())
		return true;
	this->RenderMe();
	return true;
}

void CXWindow::OnMouseDown(int , int X, int Y)
{
	POINT Abs;

	Abs.x = 0;
	Abs.y = 0;
	this->GetAbsolutePosition(&Abs);

	m_X = (long) (X - Abs.x);
	m_Y = (long) (Y - Abs.y);
	m_Depressed = false;
}

void CXWindow::OnMouseMove(int X, int Y)
{
	if(!m_Depressed)
	{
		POINT Abs;

		Abs.x = 0;
		Abs.y = 0;
		this->GetAbsolutePosition(&Abs);

		this->SetXPos(this->GetXPos() + ((X - Abs.x) - m_X));
		this->SetYPos(this->GetYPos() + ((Y - Abs.y) - m_Y));
	}
}

void CXWindow::OnMouseUp(int , int , int )
{
	m_Depressed = true;
}


void CXWindow::OnSetFocus()
{
}

void CXWindow::OnLostFocus()
{
}

void CXWindow::OnKeyDown(WPARAM , LPARAM )
{
}

void CXWindow::OnKeyUp(WPARAM , LPARAM )
{
}

void CXWindow::OnSysKey(WPARAM , LPARAM )
{
}