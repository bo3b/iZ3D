#pragma once

#include <vector>
#include "StringResourceManager.h"
//////////////////////////////////////////////////////////////////////////
template <class TWidgetWindow>
void InternationalizeControlWOResize( TWidgetWindow* pCtrl, const std::wstring& tagName )
{
	const wchar_t* ptr =
		iz3d::resources::TStringResourceSingleton::instance().getString( tagName.c_str() );
	pCtrl->SetLabel( ptr );
}

//////////////////////////////////////////////////////////////////////////
template <class TWidgetWindow>
void InternationalizeControl( TWidgetWindow* pCtrl, const std::wstring& tagName )
{
	InternationalizeControlWOResize( pCtrl, tagName );
	pCtrl->SetSize( wxRect( -1, -1, -1, -1 ) );
}

//////////////////////////////////////////////////////////////////////////
template <class TWidgetChoice>
void InternationalizeChoiceWOResize(TWidgetChoice* pChoice,
									const std::vector<std::wstring>& newChoices )
{
	int n = pChoice->GetSelection();
	pChoice->Clear();
	std::vector<wxString> choices(newChoices.size());
	for( size_t i = 0; i < choices.size(); ++ i )
	{
		choices[i] = iz3d::resources::TStringResourceSingleton::instance().getString(
			newChoices[i].c_str() );
	}
	pChoice->Append( wxArrayString( choices.size(), &choices[0]));
	pChoice->SetSelection( n );
}

//////////////////////////////////////////////////////////////////////////
template <class TWidgetChoice>
void InternationalizeChoice( TWidgetChoice* pChoice,
							const std::vector<std::wstring>& newChoices )
{
	InternationalizeChoiceWOResize( pChoice, newChoices);
	pChoice->SetSize( -1, -1, -1, -1 );
}