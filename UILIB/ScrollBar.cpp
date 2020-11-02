#include <d3d9.h>
#include <d3dx9.h>
#include "uilib.h"

CXScrollBar::CXScrollBar(iz3d::uilib::CommonDevice* Device) : CXControl(Device)
{
	m_pThumb = new CXButton(Device);
	m_pRightTopArrow = new CXButton(Device);
	m_pLeftBottomArrow = new CXButton(Device);

	m_ThumbPosition = 0;
	m_ThumbHeight = 0;
	m_Change = 1;
	SetMinMax(0,1);
	SetValue(0);
	m_DragMode = false;

	ZeroMemory(&m_ThumbPos, sizeof(D3DXVECTOR2));
}

CXScrollBar::~CXScrollBar()
{
	if(m_pThumb)
		delete m_pThumb;

	if(m_pRightTopArrow)
		delete m_pRightTopArrow;

	if(m_pLeftBottomArrow)
		delete m_pLeftBottomArrow;
}

bool CXScrollBar::OnRender()
{
	POINT Abs;
	ZeroMemory(&Abs, sizeof(D3DXVECTOR2));
	GetAbsolutePosition(&Abs);

	POINTF Scaling;
	POINT Trans;

	Scaling.x = 1.0f;
	Scaling.y = AutoSizeBackground();
	Trans.x = Abs.x;
	Trans.y = Abs.y + m_pRightTopArrow->GetHeight();
	GetCanvas()->SetScaling(Scaling);

	for(int counter = m_Min; counter < m_Max; counter++)
	{
		GetCanvas()->SetTranslation(Trans);	
		GetPen()->DrawTexture(GetCanvas());
		Trans.y += (LONG)((FLOAT)m_BackgroundHeight * Scaling.y);
	}

	m_pRightTopArrow->SetXYPos(Abs.x, Abs.y);
	m_pRightTopArrow->OnRender();
	m_pLeftBottomArrow->SetXYPos(Abs.x, Abs.y + (GetHeight() - m_pLeftBottomArrow->GetHeight()));
	m_pLeftBottomArrow->OnRender();

	UpdateScaling();

	m_pThumb->SetXYPos(Abs.x, Abs.y + m_ThumbPos.y);
	m_pThumb->OnRender();

	return true;
}

void CXScrollBar::OnMouseDown(int , int , int Y)
{
	POINT Abs;
	ZeroMemory(&Abs, sizeof(D3DXVECTOR2));
	GetAbsolutePosition(&Abs);

	if ((Y >= Abs.y) && (Y <= Abs.y + m_pRightTopArrow->GetHeight()))
	{
		SetValue(m_ThumbPosition - m_Change);
		return;
	}

	if ((Y >= (Abs.y + GetHeight()) - m_pLeftBottomArrow->GetHeight()) && (Y <= Abs.y + GetHeight()))
	{
		SetValue(m_ThumbPosition + m_Change);
		return;
	}

	LONG Thumb = Abs.y + m_ThumbPos.y;

	if((Y >= Thumb) && ( Y <= Thumb + m_ThumbHeight))
	{
		m_DragMode = true;
		return;
	}


	LONG Val = m_pRightTopArrow->GetHeight() + (Y -(Abs.y + m_pRightTopArrow->GetHeight()));

	SetValue(GetPosValue(Val));
}

void CXScrollBar::OnMouseMove(int , int Y)
{
	POINT Abs;
	ZeroMemory(&Abs, sizeof(D3DXVECTOR2));
	GetAbsolutePosition(&Abs);


	if(m_DragMode)
	{
		LONG Val = m_pRightTopArrow->GetHeight() + (Y -(Abs.y + m_pRightTopArrow->GetHeight()));

		SetValue(GetPosValue(Val));
		return;
	}
}

void CXScrollBar::OnMouseUp(int , int , int )
{
	m_DragMode = false;
}

void CXScrollBar::OnSetFocus()
{
}

void CXScrollBar::OnLostFocus()
{
}

void CXScrollBar::OnKeyDown(WPARAM , LPARAM )
{
}

void CXScrollBar::OnKeyUp(WPARAM , LPARAM )
{
}

void CXScrollBar::OnSysKey(WPARAM Key, LPARAM )
{
	switch(Key)
	{
	case VK_UP:
		SetValue(m_ThumbPosition - m_Change);
		return;
	case VK_DOWN:
		SetValue(m_ThumbPosition + m_Change);
		return;
	}
}

void CXScrollBar::LoadThumbImage(TCHAR* File)
{
	m_pThumb->SetDefaultImage(File);
	m_pThumb->SetPressedImage(File);
	m_ThumbHeight = m_pThumb->GetHeight();
}

void CXScrollBar::LoadRightTopArrow(TCHAR* File)
{
	m_pRightTopArrow->SetDefaultImage(File);
	m_pRightTopArrow->SetPressedImage(File);
}

void CXScrollBar::LoadLeftBottomArrow(TCHAR* File)
{
	m_pLeftBottomArrow->SetDefaultImage(File);
	m_pLeftBottomArrow->SetPressedImage(File);
}

void CXScrollBar::SetValue(int Value)
{
	int NewValue = Value;

	if(Value < m_Min)
		NewValue = m_Min;
	else if (Value >= m_Max)
		NewValue = m_Max - 1;

	LONG Range = (GetHeight() - m_pLeftBottomArrow->GetHeight()) - m_pRightTopArrow->GetHeight(); 
	FLOAT Increment = (FLOAT)Range / m_Max;

	m_ThumbPos.y = m_pRightTopArrow->GetHeight() + (LONG)(Increment * (FLOAT)NewValue);
	m_ThumbPosition = NewValue;
}

int CXScrollBar::GetPosValue(LONG Pos)
{
	if((Pos >= m_pRightTopArrow->GetHeight()) && (Pos < (GetHeight() - m_pLeftBottomArrow->GetHeight())))
	{

		LONG Range = (GetHeight() - m_pLeftBottomArrow->GetHeight()) - m_pRightTopArrow->GetHeight(); 
		FLOAT Increment = (FLOAT)Range / m_Max;


		for(int counter = m_Min; counter < m_Max; counter++)
		{

			FLOAT Item = m_pRightTopArrow->GetHeight() + (Increment * counter);

			if(((Pos >= Item)) && (Pos <= (Item + Increment)))
				return counter;
		}

	}

	return 0;
}

FLOAT CXScrollBar::AutoSizeThumb()
{
	LONG Range = (GetHeight() - m_pLeftBottomArrow->GetHeight()) - m_pRightTopArrow->GetHeight(); 
	FLOAT Increment = (FLOAT)Range / m_Max;

	return (Increment / m_pThumb->GetHeight());
}

void CXScrollBar::SetMinMax(int Min, int Max)
{
	m_Min = Min;
	m_Max = Max;

	if(m_ThumbPosition > m_Max)
		m_ThumbPosition = m_Max;

	if(m_ThumbPosition < Min)
		m_ThumbPosition = Min;
}

void CXScrollBar::UpdateScaling()
{
	m_ThumbScale.x = 1.0f;
	m_ThumbScale.y = AutoSizeThumb();
	m_pThumb->GetCanvas()->SetScaling(m_ThumbScale);
	m_ThumbHeight = (LONG)(m_pThumb->GetHeight() * m_ThumbScale.y);
}

void CXScrollBar::LoadBackground(TCHAR* File)
{
	GetCanvas()->LoadFromFile(File);
	D3DXIMAGE_INFO Info;
	ZeroMemory(&Info, sizeof(D3DXIMAGE_INFO));
	D3DXGetImageInfoFromFile(File, &Info);
	m_BackgroundHeight = Info.Height;
}

FLOAT CXScrollBar::AutoSizeBackground()
{
	LONG Range = (GetHeight() - m_pLeftBottomArrow->GetHeight()) - m_pRightTopArrow->GetHeight(); 
	FLOAT Increment = (FLOAT)Range / m_Max;

	return (Increment / m_BackgroundHeight);
}