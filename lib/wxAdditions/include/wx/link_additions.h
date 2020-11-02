/////////////////////////////////////////////////////////////////////////////
// Name:        wx/link_additions.h
// Purpose:     Link wxAdditions libraries
// Author:      RPM Enterprises
// Modified by:
// Created:     22.08.06
// Copyright:   (c) RPM Enterprises
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef LINK_ADDITIONS_H
#define LINK_ADDITIONS_H

#include <wx/defs.h>

// MSVC is the only linker supported, until we know of a way to link a library from a source file for other linkers.
#ifdef __VISUALC__

// Version
#define STRINGIFIER( x ) #x
#define BUILD_TWO_PART_STRING( x, y ) STRINGIFIER( x ) STRINGIFIER( y )
#define WX_LIB_VERSION BUILD_TWO_PART_STRING( wxMAJOR_VERSION, wxMINOR_VERSION )

// Unicode
#ifdef _UNICODE
	#define WX_UNICODE_FLAG "u"
#else
	#define WX_UNICODE_FLAG
#endif

// Only support linking monolithic dlls in this way
#ifdef WXUSINGDLL
	#define WX_MONO_FLAG "m"
#else
	#define WX_MONO_FLAG
#endif

// Debug
#ifdef _DEBUG
	#define WX_DEBUG_FLAG "d"
#else
	#define WX_DEBUG_FLAG
#endif

// Full library name
#define GET_WX_LIB_NAME( LIB ) \
	"wxmsw" WX_LIB_VERSION WX_UNICODE_FLAG WX_MONO_FLAG WX_DEBUG_FLAG "_" #LIB ".lib"

// Link libraries

// AWX has no exported functions, so it cannot be used as a dll from VC
#ifndef WXUSINGDLL
	#pragma comment( lib, GET_WX_LIB_NAME( awx ) )
#endif

#pragma comment( lib, GET_WX_LIB_NAME( flatnotebook ) )
#pragma comment( lib, GET_WX_LIB_NAME( plot ) )
#pragma comment( lib, GET_WX_LIB_NAME( plotctrl ) )
#pragma comment( lib, GET_WX_LIB_NAME( propgrid ) )
#pragma comment( lib, GET_WX_LIB_NAME( scintilla ) )
#pragma comment( lib, GET_WX_LIB_NAME( things ) )
#pragma comment( lib, GET_WX_LIB_NAME( treelistctrl ) )
#pragma comment( lib, GET_WX_LIB_NAME( ledbargraph ) )

#endif //_VISUALC_

#endif //LINK_ADDITIONS_H