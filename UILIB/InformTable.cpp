#include <xstring>
#include "InformTable.h"
#include "..\CommonUtils\StringResourceManager.h"
#include "..\S3DAPI\GlobalData.h"
#include "..\CommonUtils\KeyConv.h"
const wchar_t* LABLES_TO_SHOW[] = {
	L"lblTakeScreenshot",
	L"lblToggleStereo",
	L"lblIncConv",
	L"lblDecConv",
	L"lblIncSep",
	L"lblDecSep",
	L"lblSwapLR",
	L"lblPreset1",
	L"lblPreset2",
	L"lblPreset3",
	L"lblToggleAutofocus",
	L"lblToggleLasersight",
	L"lblToggleWizard",
	L"lblToggleFPS",
	L"lblToggleHotkeysOSD",
};

iz3d::UI::InformTable::InformTable()
{
}

iz3d::UI::InformTable::~InformTable()
{
	m_Table.clear();
}

size_t iz3d::UI::InformTable::GetLinesTable()
{
	return m_Table.size();
}

size_t iz3d::UI::InformTable::GetColumnsTable()
{
	if( m_Table.empty() )
		return 0;
	return m_Table[0].size();
}

const wchar_t* iz3d::UI::InformTable::GetItem( size_t nLineNum, size_t nColumnTable )
{
	if( nLineNum > m_Table.size() )
		return L"";
	if( nColumnTable > m_Table[nLineNum].size() )
		return L"";
	return m_Table[nLineNum][nColumnTable].c_str();
}

void iz3d::UI::InformTable::ReadTextMessages()
{
	m_Table.clear();
	std::wstring startNode = L"\\ControlCenter\\Profiles\\Input\\";
	// fill the table
	for( size_t i = 0; i < _countof( LABLES_TO_SHOW ); ++i ){
		const wchar_t* str = iz3d::resources::TStringResourceSingleton::instance().getString( startNode + LABLES_TO_SHOW[i] );
		iz3d::UI::InformTable::TRow WizHK;
		// first column
		WizHK.push_back( str ? str : L"" );
		// second column
		std::wstring keyString = app::guilib::KeyConverter::ToString(gInfo.HotKeyList[i].code);
		WizHK.push_back(keyString.c_str() );
		m_Table.push_back( WizHK );
	}
}
