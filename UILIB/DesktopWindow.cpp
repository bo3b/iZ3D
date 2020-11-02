#include <d3d9.h>
#include <d3dx9.h>
#include "uilib.h"

CXDesktopWindow::CXDesktopWindow(iz3d::uilib::CommonDevice* Device) : CXControl(Device)
{
	SIZE viewPort = Device->GetViewportWidthHeight();
	this->SetWidthHeight( viewPort.cx, viewPort.cy );
	this->SetFocusControl(this);
}

bool CXDesktopWindow::OnRender()
{
    this->RenderMe();
	return true;
}

void CXDesktopWindow::OnLostDevice()
{
	GetPen()->InvalidateDeviceObjects();
	GetCanvas()->InvalidateDeviceObjects();
	RemoveAllChildren();
}

void CXDesktopWindow::OnMouseDown(int , int , int )
{
}

void CXDesktopWindow::OnMouseMove(int , int )
{
}

void CXDesktopWindow::OnMouseUp(int , int , int )
{
}

void CXDesktopWindow::OnSetFocus()
{
}

void CXDesktopWindow::OnLostFocus()
{
}

void CXDesktopWindow::OnKeyDown(WPARAM , LPARAM )
{
}

void CXDesktopWindow::OnKeyUp(WPARAM , LPARAM )
{
}

void CXDesktopWindow::OnSysKey(WPARAM , LPARAM )
{
}