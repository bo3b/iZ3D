#pragma once
#include <Unknwn.h>
#include <boost/function.hpp>
#include "../CommonUtils/Function.h"
struct ICommonTexture
{
	virtual SIZE GetWidthHeight() const = 0;
	typedef iz3d::utils::IFunction2<void, RECT*, RECT*> TDrawFunction;
};