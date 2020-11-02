/*
 *  StdioInputStream.h
 *  BuddySystem
 *
 *  Created by Stuart A. Malone on 8/28/09.
 *  Copyright 2009 Llamagraphics, Inc. All rights reserved.
 *
 */

#ifndef LLAMAXML_STDIOINPUTSTREAM_H
#define LLAMAXML_STDIOINPUTSTREAM_H

#if (! __GNUC__) || __APPLE__
	#pragma once
#endif


#include "LlamaXML/InputStream.h"
#include <stdio.h>

namespace LlamaXML {

	class StdioInputStream: public InputStream {
		StdioInputStream(const char * path);
		virtual ~StdioInputStream();
		
		virtual uint32_t ReadUpTo(char * buffer, uint32_t length);
		virtual void Restart();
		virtual bool EndOfFile();
	
	private:
		FILE *	mFile;
	};
	
}

#endif
