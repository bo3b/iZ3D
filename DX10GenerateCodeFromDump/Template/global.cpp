#include "pcmp.h"
#include "global.h"

HINSTANCE	g_hInstance				= NULL;
HWND		g_hWnd					= NULL;
//int			g_nWndWidth				= 1024;
//int			g_nWndHeight			= 768;
bool		g_bFullscreen			= false;
TCHAR		g_sBinDir[ MAX_PATH ]	= {};
struct ITaskbarList3*	g_pTaskbarList = NULL;
CComQIPtr<IStereoAPIInternal2>  g_pAPI2;

/************************************************************************/
/* GetDirFromFileName                                                   */
/************************************************************************/
TCHAR*	GetDirFromFileName( TCHAR* _szFileName )
{
	static TCHAR szBuff[ MAX_PATH ];
	int nLen = ( int )_tcslen( _szFileName ) + 1;
	memcpy( szBuff, _szFileName, nLen * sizeof( TCHAR ) );
	int nLastSlashPos = -1;

	for ( int i = 0; i < nLen; i++ )
	{
		if ( _szFileName[ i ] == '\\' || _szFileName[ i ] == '/' )
			nLastSlashPos = i;
	}

	if ( nLastSlashPos == -1 )
		return szBuff;

	szBuff[ nLastSlashPos ] = '\0';
	return szBuff;
}

/************************************************************************/
/* GetShaderPath		                                                */
/************************************************************************/
TCHAR* GetShaderPath( TCHAR* _szSahderName )
{
	static TCHAR szBuff[ MAX_PATH ];
	_stprintf_s( szBuff, L"%s\\shaders\\simple.fx", g_sBinDir );
	return szBuff;
}

/************************************************************************/
/* SetEID		                                                        */
/************************************************************************/
void  SetEID(UINT64 eid)
{
	if (g_pAPI2) g_pAPI2->SetEID(eid);
}
