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
/************************************************************************/
/* Project  -> Diagnostic Tool                                          */
/* Authors  -> Andrew Kolpakov aka x0ras                                */
/* E-Mail   -> x0ras@neurok.ru                                          */
/************************************************************************/
/* Copyright (c) 2007 iZ3D Inc.                                          */
/************************************************************************/

#include "Error.h"


const char* SystemException::message() const
{
  return message_.c_str();
}



const char* WrongVariantException::message() const
{
  return message_.c_str();
}

