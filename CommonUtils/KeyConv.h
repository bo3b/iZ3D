#pragma once

/* 
* Project : iZ3D Stereo Driver
* Copyright (C) iZ3D Inc. 2002 - 2010
*/

#ifndef __CC_GUILIB_KEY_CONVERTER_H__
#define __CC_GUILIB_KEY_CONVERTER_H__

#include <string>

namespace app {
	namespace guilib {

		class KeyConverter
		{
		public:

			static std::wstring ToString(int key);
			static int ToKey(const std::wstring& text);

		private:

			class Key
			{
			public:
				int modifiers;
				int key;
			};

			enum Modifiers
			{
				None = 0,
				Shift = 1,
				Control = 2,
				Alt = 4,
				Windows = 8
			};

			// Only unpacked values are acceptable
			static void ToStringModifiers(std::wstringstream &stream, int modifiers);
			static void ToStringIfNumpadKey(std::wstringstream &stream, int key);
			static void ToStringIfFKey(std::wstringstream &stream, int key);
			static void ToStringIfAsciiKey(std::wstringstream &stream, int key);
			static void ToStringIfOther(std::wstringstream &stream, int key);

			static int ToKeyIfNumpadKey(const std::wstring& text);
			static int ToKeyIfFKey(const std::wstring& text);
			static int ToKeyIfAsciiKey(const std::wstring& text);
			static int ToKeyModifiers(const std::wstring& text);
			static int ToKeyIfOther(const std::wstring& text);

			static Key UnpackKey(int key);
			static int PackKey(Key key);


			//-------------------------------------------------------------------------
			// Windows virtual key codes constants
			//-------------------------------------------------------------------------

			// Means our own ascii table wich starts from 45 and ends at 96 in standard ascii table
			static const int FIRST_ASCII_CHAR = 48;
			static const int LAST_ASCII_CHAR = 95;
		};

	}
}

#endif
