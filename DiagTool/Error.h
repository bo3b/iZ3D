/* 
* Project : iZ3D Stereo Driver
* Copyright (C) iZ3D Inc. 2002 - 2010
*/

/************************************************************************/
/* Project  -> Diagnostic Tool                                          */
/* Authors  -> Andrew Kolpakov aka x0ras                                */
/* E-Mail   -> x0ras@neurok.ru                                          */
/************************************************************************/
/* Copyright (c) 2007 iZ3D Inc.                                          */
/************************************************************************/

#ifndef __DIAGTOOL_ERROR_H__
#define __DIAGTOOL_ERROR_H__

#include <exception>
#include <string>


class SystemException : public std::exception
{
public:
  
  inline SystemException(const std::string& message);

  virtual const char* message() const;

private:

  std::string message_;
};


//---------------------------------------------------
// Definition
//---------------------------------------------------


SystemException::SystemException(const std::string& message) :
std::exception(),
message_(message)
{

}




//---------------------------------------------------
// Macro
//---------------------------------------------------

#define ERROR_OUTPUT(a,b) { if (FAILED(a)) throw SystemException((b)); }



//---------------------------------------------------
// Exception type for wrong VARIANT types (Microsoft bicycle)
//---------------------------------------------------


class WrongVariantException : public std::exception
{
public:

  inline WrongVariantException(const std::string& message);

  virtual const char* message() const;

private:

  std::string message_;
};




WrongVariantException::WrongVariantException(const std::string& message) :
std::exception(),
message_(message)
{

}



#endif
