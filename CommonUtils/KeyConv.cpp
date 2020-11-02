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
#include <sstream>
#include <shlwapi.h>
#include <Windows.h>
#include "KeyConv.h"

namespace {
	static const wchar_t* DISABLED_KEY			= L"<disabled>";

	static const wchar_t* CONTROL_MODIFIER		= L"Ctrl + ";
	static const wchar_t* ALT_MODIFIER			= L"Alt + ";
	static const wchar_t* SHIFT_MODIFIER		= L"Shift + ";
	static const wchar_t* WINDOWS_MODIFIER		= L"Windows + ";
}

namespace app {
	namespace guilib {

		//-------------------------------------------------------------------------------------
		// KeyConverter class realization
		//-------------------------------------------------------------------------------------

		std::wstring KeyConverter::ToString(int key)
		{
			std::wstringstream stream;
			Key unpackedKey = UnpackKey(key);
			ToStringModifiers(stream, unpackedKey.modifiers);
			ToStringIfNumpadKey(stream, unpackedKey.key);
			ToStringIfFKey(stream, unpackedKey.key);
			ToStringIfAsciiKey(stream, unpackedKey.key);
			ToStringIfOther(stream, unpackedKey.key);

			if (stream.str().size() == 0)
				stream << DISABLED_KEY;

			return stream.str();
		}

		int KeyConverter::ToKey(const std::wstring& text)
		{
			Key key;

			key.modifiers = ToKeyModifiers( text );

			key.key = ToKeyIfNumpadKey( text );
			if ( !key.key )
			{
				key.key = ToKeyIfFKey( text );
				if ( !key.key )
				{
					key.key = ToKeyIfOther( text );
					if ( !key.key )
					{
						if (text.find(DISABLED_KEY) == std::wstring::npos)
						{
							key.key = ToKeyIfAsciiKey( text );
							if( !key.key )
								throw std::runtime_error("error parsing key");
						}
					}
				}
			}
			return PackKey( key );
		}

		void KeyConverter::ToStringIfNumpadKey(std::wstringstream &stream, int key)
		{
			if (key == VK_CLEAR)
			{
				stream << L"Num Middle";
				return;
			}
			if (key < VK_NUMPAD0 || key > VK_DIVIDE)
				return;

			stream << L"Num ";

			if (key >= VK_NUMPAD0 && key <= VK_NUMPAD9)
				stream << (key - VK_NUMPAD0);
			else
			{
				switch (key)
				{
				case VK_MULTIPLY:
					{
						stream << L"*";
						break;
					}
				case VK_SEPARATOR:
					{
						stream << L"enter";
						break;
					}
				case VK_ADD:
					{
						stream << L"+";
						break;
					}
				case VK_SUBTRACT:
					{
						stream << L"-";
						break;
					}
				case VK_DECIMAL:
					{
						stream << L".";
						break;
					}
				case VK_DIVIDE:
					{
						stream << L"/";
						break;
					}
				default:
					throw std::runtime_error("incorrect key value");
				}
			}
		}

		void KeyConverter::ToStringIfFKey(std::wstringstream &stream, int key)
		{
			if (key < VK_F1 || key > VK_F24)
				return;

			stream << L"F" << ((key - VK_F1) + 1); // We want to display F1, F2, F3 and etc., that's why we use VK_F1 + 1
		}

		void KeyConverter::ToStringIfAsciiKey(std::wstringstream &stream, int key)
		{
			if (key < FIRST_ASCII_CHAR || key > LAST_ASCII_CHAR)
				return;

			stream << static_cast<wchar_t>(key);
		}

		void KeyConverter::ToStringIfOther(std::wstringstream &stream, int key)
		{
			// Big switch! :(

			switch (key)
			{
			case VK_OEM_COMMA:
				{
					stream << L",";
					break;
				}
			case VK_OEM_PERIOD:
				{
					stream << L".";
					break;
				}
			case VK_OEM_PLUS:
				{
					stream << L"=";
					break;
				}
			case VK_OEM_MINUS:
				{
					stream << L"-";
					break;
				}
			case VK_OEM_1:
				{
					stream << L";";
					break;
				}
			case VK_OEM_2:
				{
					stream << L"/";
					break;
				}
			case VK_OEM_3:
				{
					stream << L"~";
					break;
				}
			case VK_OEM_4:
				{
					stream << L"[";
					break;
				}
			case VK_OEM_5:
				{
					stream << L"\\";
					break;
				}
			case VK_OEM_6:
				{
					stream << L"]";
					break;
				}
			case VK_OEM_7:
				{
					stream << L"'";
					break;
				}
			case VK_BACK:
				{
					stream << L"Backspace";
					break;
				}
			case VK_TAB:
				{
					stream << L"Tab";
					break;
				}
				//case VK_ESCAPE:
				//  {
				//    stream << L"Esc";
				//    break;
				//  }
			case VK_SPACE:
				{
					stream << L"Space";
					break;
				}
			case VK_PRIOR:
				{
					stream << L"PgUp";
					break;
				}
			case VK_NEXT:
				{
					stream << L"PgDown";
					break;
				}
			case VK_END:
				{
					stream << L"End";
					break;
				}
			case VK_HOME:
				{
					stream << L"Home";
					break;
				}
			case VK_INSERT:
				{
					stream << L"Ins";
					break;
				}
			case VK_DELETE:
				{
					stream << L"Del";
					break;
				}
			case VK_LEFT:
				{
					stream << L"Left";
					break;
				}
			case VK_RIGHT:
				{
					stream << L"Right";
					break;
				}
			case VK_UP:
				{
					stream << L"Up";
					break;
				}
			case VK_DOWN:
				{
					stream << L"Down";
					break;
				}
			case VK_SNAPSHOT:
				{
					stream << L"PrtScrn";
					break;
				}
			case VK_PAUSE:
				{
					stream << L"Pause";
					break;
				}
			case VK_CAPITAL:
				{
					stream << L"Caps Lock";
					break;
				}
			case VK_NUMLOCK:
				{
					stream << L"Num Lock";
					break;
				}
			case VK_SCROLL:
				{
					stream << L"Scroll Lock";
					break;
				}
			}
		}


		void KeyConverter::ToStringModifiers(std::wstringstream &stream, int modifiers)
		{
			if ((modifiers & Control) > 0)
				stream << CONTROL_MODIFIER;
			if ((modifiers & Alt) > 0)
				stream << ALT_MODIFIER;
			if ((modifiers & Shift) > 0)
			{
				stream << SHIFT_MODIFIER;
			}
		}
		int KeyConverter::ToKeyIfNumpadKey(const std::wstring& text)
		{
			if (text.empty())
				return 0;

			std::wstring::size_type pos = text.rfind(L"Num ");

			if (std::wstring::npos == pos)
				return 0;

			int key = 0;

			if (text.find(L"Middle") != std::wstring::npos)
			{
				key = VK_CLEAR;
				return key;
			}

			if (text.find(L"enter") != std::wstring::npos)
			{
				key = VK_SEPARATOR;
				return key;
			}

			wchar_t lastChar = (*(--text.end()));

			if (lastChar >= L'0' && lastChar <= L'9')
				key = VK_NUMPAD0 + (static_cast<int>(lastChar) - 48);

			switch (lastChar)
			{
			case L'*':
				{
					key = VK_MULTIPLY;
					break;
				}
			case L'/':
				{
					key = VK_DIVIDE;
					break;
				}
			case L'-':
				{
					key = VK_SUBTRACT;
					break;
				}
			case L'+':
				{
					key = VK_ADD;
					break;
				}
			case L'.':
				{
					key = VK_DECIMAL;
					break;
				}
			}

			return key;
		}

		int KeyConverter::ToKeyIfFKey(const std::wstring& text)
		{
			if (text.empty() || (text.length() < 2))
				return 0;

			std::wstring::size_type pos = text.rfind(L"F");

			if (pos == std::wstring::npos)
				return 0;

			int key = VK_F1;

			//std::wstring::iterator itEnd = text.end();
			wchar_t lastChar = *(--text.end());
			wchar_t preLastChar = *(--(--text.end()));

			if (preLastChar == L'F')
				key += ((int)lastChar - 49); // 48 is the code of '1' in ascii table
			else
			{
				if (preLastChar == L'1')
					key += (((int)lastChar - 49) + 10); // We are in the F10-F19 range, so add 10
				else if (preLastChar == L'2')
					key += (((int)lastChar - 49) + 20); // We are in the F20-F24 range, so add 20
				else
					key = 0;
			}

			return key;
		}

		int KeyConverter::ToKeyIfAsciiKey(const std::wstring& text)
		{
			if (text.empty())
				return 0;

			//std::wstring::iterator itEnd = text.end();
			wchar_t lastChar = *(--text.end());

			int key = 0;

			if ((int)lastChar >= FIRST_ASCII_CHAR && (int)lastChar <= LAST_ASCII_CHAR)
				key = (int)lastChar;

			return key;
		} 

		int KeyConverter::ToKeyIfOther(const std::wstring& text)
		{
			if (text.empty())
				return 0;

			int key = 0;

			if (text.find(L",") != std::wstring::npos)
				key = VK_OEM_COMMA;
			else if (text.find(L".") != std::wstring::npos)
				key = VK_OEM_PERIOD;
			else if (text.find(L"=") != std::wstring::npos)
				key = VK_OEM_PLUS;
			else if (text.find(L"-") != std::wstring::npos)
				key = VK_OEM_MINUS;
			else if (text.find(L";") != std::wstring::npos)
				key = VK_OEM_1;
			else if (text.find(L"/") != std::wstring::npos)
				key = VK_OEM_2;
			else if (text.find(L"~") != std::wstring::npos)
				key = VK_OEM_3;
			else if (text.find(L"[") != std::wstring::npos)
				key = VK_OEM_4;
			else if (text.find(L"\\") != std::wstring::npos)
				key = VK_OEM_5;
			else if (text.find(L"]") != std::wstring::npos)
				key = VK_OEM_6;
			else if (text.find(L"'") != std::wstring::npos)
				key = VK_OEM_7;
			else if (text.find(L"Backspace") != std::wstring::npos)
				key = VK_BACK;
			else if (text.find(L"Tab") != std::wstring::npos)
				key = VK_TAB;
			//else if (text.find(L"Esc") != std::wstring::npos)
			//  key = VK_ESCAPE;
			else if (text.find(L"Space") != std::wstring::npos)
				key = VK_SPACE;
			else if (text.find(L"PgUp") != std::wstring::npos)
				key = VK_PRIOR;
			else if (text.find(L"PgDown") != std::wstring::npos)
				key = VK_NEXT;
			else if (text.find(L"End") != std::wstring::npos)
				key = VK_END;
			else if (text.find(L"Home") != std::wstring::npos)
				key = VK_TAB;
			else if (text.find(L"Ins") != std::wstring::npos)
				key = VK_INSERT;
			else if (text.find(L"Del") != std::wstring::npos)
				key = VK_DELETE;
			else if (text.find(L"Left") != std::wstring::npos)
				key = VK_LEFT;
			else if (text.find(L"Right") != std::wstring::npos)
				key = VK_RIGHT;
			else if (text.find(L"Up") != std::wstring::npos)
				key = VK_UP;
			else if (text.find(L"Down") != std::wstring::npos)
				key = VK_DOWN;
			else if (text.find(L"PrtScrn") != std::wstring::npos)
				key = VK_SNAPSHOT;
			else if (text.find(L"Pause") != std::wstring::npos)
				key = VK_PAUSE;
			else if (text.find(L"Caps Lock") != std::wstring::npos)
				key = VK_CAPITAL;
			else if (text.find(L"Num Lock") != std::wstring::npos)
				key = VK_NUMLOCK;
			else if (text.find(L"Scroll Lock") != std::wstring::npos)
				key = VK_SCROLL;
			else 
				return 0;

			return key;
		}

		int KeyConverter::ToKeyModifiers(const std::wstring& text)
		{
			int modifiers = 0;

			if (text.find(CONTROL_MODIFIER, 0) != std::wstring::npos)
				modifiers |= Control;
			if (text.find(ALT_MODIFIER, 0) != std::wstring::npos)
				modifiers |= Alt;
			if (text.find(SHIFT_MODIFIER, 0) != std::wstring::npos)
				modifiers |= Shift;

			return modifiers;
		}

		KeyConverter::Key KeyConverter::UnpackKey(int key)
		{
			Key unpackedKey;

			unpackedKey.key       = key & 0xff;
			unpackedKey.modifiers = ((key & 0x0f00) >> 8);

			return unpackedKey;
		}

		int KeyConverter::PackKey(Key key)
		{
			return (key.key | (key.modifiers << 8));
		}

	}
}
