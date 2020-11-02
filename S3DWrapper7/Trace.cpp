/* IZ3D_FILE: $Id$ 
*
* Project : iZ3D Stereo Driver
* Copyright (C) iZ3D Inc. 2002 - 2010
*
* $Author$
* $Revision$
* $Date$
* $LastChangedBy$
* $URL$
*/
#include "stdafx.h"

#ifndef FINAL_RELEASE

#define NSBUFFERLENGTH 4096

void PrintMatrix( CONST D3DMATRIX * mat ) {
	DEBUG_MESSAGE( "%12.7f %12.7f %12.7f %12.7f\n"
		"%12.7f %12.7f %12.7f %12.7f\n"
		"%12.7f %12.7f %12.7f %12.7f\n"
		"%12.7f %12.7f %12.7f %12.7f\n\n",
		mat->_11, mat->_12, mat->_13, mat->_14,
		mat->_21, mat->_22, mat->_23, mat->_24,
		mat->_31, mat->_32, mat->_33, mat->_34,
		mat->_41, mat->_42, mat->_43, mat->_44 );
}

#endif

const char * GetResultString( HRESULT hr )
{
#ifndef FINAL_RELEASE
	return DXGetErrorStringA(hr);
#else
	return NULL;
#endif
}
