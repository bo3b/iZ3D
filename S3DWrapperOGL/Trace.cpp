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
#include "StdAfx.h"
#include "S3DWrapperOGL.h"
#include "Trace.h"

#ifndef FINAL_RELEASE

#define NSBUFFERLENGTH 8192

#define UNPACK_DEFINE(Value, VALUE) \
	if ((Value & VALUE) == VALUE) \
{ \
	if (bAdd) string += "|"; \
	string += #VALUE; \
	bAdd = TRUE; \
}

#define UNPACK_VALUE(VALUE) case VALUE: return #VALUE;

const char *GetPixelFormatDescriptorFlagsString(DWORD Flags)
{
	static std::string string;
	string = "";
	BOOL bAdd = FALSE;
	UNPACK_DEFINE(Flags, PFD_DOUBLEBUFFER);
	UNPACK_DEFINE(Flags, PFD_STEREO);
	UNPACK_DEFINE(Flags, PFD_DRAW_TO_WINDOW);
	UNPACK_DEFINE(Flags, PFD_DRAW_TO_BITMAP);
	UNPACK_DEFINE(Flags, PFD_SUPPORT_GDI);
	UNPACK_DEFINE(Flags, PFD_SUPPORT_OPENGL);
	UNPACK_DEFINE(Flags, PFD_GENERIC_FORMAT);
	UNPACK_DEFINE(Flags, PFD_NEED_PALETTE);
	UNPACK_DEFINE(Flags, PFD_NEED_SYSTEM_PALETTE);
	UNPACK_DEFINE(Flags, PFD_SWAP_EXCHANGE);
	UNPACK_DEFINE(Flags, PFD_SWAP_COPY);
	UNPACK_DEFINE(Flags, PFD_SWAP_LAYER_BUFFERS);
	UNPACK_DEFINE(Flags, PFD_GENERIC_ACCELERATED);
	UNPACK_DEFINE(Flags, PFD_SUPPORT_DIRECTDRAW);
	UNPACK_DEFINE(Flags, PFD_DEPTH_DONTCARE);
	UNPACK_DEFINE(Flags, PFD_DOUBLEBUFFER_DONTCARE);
	UNPACK_DEFINE(Flags, PFD_STEREO_DONTCARE);
	return string.c_str();
}

const char *GetPixelFormatDescriptorString(const PIXELFORMATDESCRIPTOR *ppfd)
{
	static char szBuffer[NSBUFFERLENGTH];
	szBuffer[0] = 0;
	if (ppfd == 0)
	{
		return "";
	}
	_snprintf_s(szBuffer, _TRUNCATE, 
		"\n"
		"nSize = %d\n"
		"nVersion = %d\n"
		"dwFlags = %s\n"
		"iPixelType = %d\n"
		"cColorBits = %d\n"
		"cRedBits = %d\n"
		"cRedShift = %d\n"
		"cGreenBits = %d\n"
		"cGreenShift = %d\n"
		"cBlueBits = %d\n"
		"cBlueShift = %d\n"
		"cAlphaBits = %d\n"
		"cAlphaShift = %d\n"
		"cAccumBits = %d\n"
		"cAccumRedBits = %d\n"
		"cAccumGreenBits = %d\n"
		"cAccumBlueBits = %d\n"
		"cAccumAlphaBits = %d\n"
		"cDepthBits = %d\n"
		"cStencilBits = %d\n"
		"cAuxBuffers = %d\n"
		"iLayerType = %d\n"
		"bReserved = %d\n"
		"dwLayerMask = %d\n"
		"dwVisibleMask = %d\n"
		"dwDamageMask = %d\n",
		ppfd->nSize,
		ppfd->nVersion,
		GetPixelFormatDescriptorFlagsString(ppfd->dwFlags),
		ppfd->iPixelType,
		ppfd->cColorBits,
		ppfd->cRedBits,
		ppfd->cRedShift,
		ppfd->cGreenBits,
		ppfd->cGreenShift,
		ppfd->cBlueBits,
		ppfd->cBlueShift,
		ppfd->cAlphaBits,
		ppfd->cAlphaShift,
		ppfd->cAccumBits,
		ppfd->cAccumRedBits,
		ppfd->cAccumGreenBits,
		ppfd->cAccumBlueBits,
		ppfd->cAccumAlphaBits,
		ppfd->cDepthBits,
		ppfd->cStencilBits,
		ppfd->cAuxBuffers,
		ppfd->iLayerType,
		ppfd->bReserved,
		ppfd->dwLayerMask,
		ppfd->dwVisibleMask,
		ppfd->dwDamageMask);
	return szBuffer;
}

const char *GetDrawBufferFlagsString(DWORD Flags)
{
	switch (Flags)
	{
	UNPACK_VALUE(GL_FRONT_LEFT);
	UNPACK_VALUE(GL_FRONT_RIGHT);
	UNPACK_VALUE(GL_BACK_LEFT);
	UNPACK_VALUE(GL_BACK_RIGHT);
	UNPACK_VALUE(GL_FRONT);
	UNPACK_VALUE(GL_BACK);
	UNPACK_VALUE(GL_LEFT);
	UNPACK_VALUE(GL_RIGHT);
	UNPACK_VALUE(GL_FRONT_AND_BACK);
	UNPACK_VALUE(GL_AUX0);
	UNPACK_VALUE(GL_AUX1);
	UNPACK_VALUE(GL_AUX2);
	UNPACK_VALUE(GL_AUX3);
	}
	return "UNKNOWN";
}

const char *GetLastErrorString()
{
	DWORD dwError = GetLastError();
	static char szBuffer[2048];
	szBuffer[0] = 0;
	DWORD dwStored = FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_IGNORE_INSERTS, 0, dwError, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), szBuffer, sizeof(szBuffer), 0);
	return szBuffer;
}

const char *GetClearMaskString(GLbitfield mask)
{
	static std::string string;
	string = "";
	BOOL bAdd = FALSE;
	UNPACK_DEFINE(mask, GL_COLOR_BUFFER_BIT);
	UNPACK_DEFINE(mask, GL_DEPTH_BUFFER_BIT);
	UNPACK_DEFINE(mask, GL_ACCUM_BUFFER_BIT);
	UNPACK_DEFINE(mask, GL_STENCIL_BUFFER_BIT);
	return string.c_str();
}

const char *GetEnableString(GLenum cap)
{
	switch (cap)
	{
		UNPACK_VALUE(GL_TEXTURE_2D);
	}
	static char szBuffer[2048];
	szBuffer[0] = 0;
	_snprintf_s(szBuffer, _TRUNCATE, "0x%X", cap);
	return szBuffer;
}

#else

const char *GetPixelFormatDescriptorFlagsString(DWORD Flags) { return NULL; };
const char *GetPixelFormatDescriptorString(const PIXELFORMATDESCRIPTOR *ppfd) { return NULL; };
const char *GetDrawBufferFlagsString(DWORD Flags) { return NULL; };
const char *GetLastErrorString() { return NULL; };
const char *GetClearMaskString(GLbitfield mask) { return NULL; };
const char *GetEnableString(GLenum cap) { return NULL; };

#endif
