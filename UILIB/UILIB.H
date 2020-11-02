#pragma once
#include <windows.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <iostream>
#include <string.h>
#include "Framework.h"
#include "Control.h"
#include "DesktopWindow.h"
#include "Window.h"
#include "Label.h"
#include "Button.h"
#include "TextBox.h"
#include "CheckBox.h"
#include "ScrollBar.h"
#include "ListItem.h"
#include "ListBox.h"
#include "DropDownList.h"
#include "CommonTexture.h"
#include "CommonSprite.h"
#include "CommonFont.h"
#include "CommonLine.h"

#ifndef SAFE_DELETE
template <typename T>
inline void SAFE_DELETE (T*& p)
{
	if (!p)
		return;
	delete p;
	p = 0;
};
#endif