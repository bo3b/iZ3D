/*
 *  StdioOutputStream.h
 *  BuddySystem
 *
 *  Created by Stuart A. Malone on 8/28/09.
 *  Copyright 2009 Llamagraphics, Inc. All rights reserved.
 *
 */

#ifndef LLAMAXML_STDIOOUTPUTSTREAM_H
#define LLAMAXML_STDIOOUTPUTSTREAM_H

#if (! __GNUC__) || __APPLE__
	#pragma once
#endif


#include "LlamaXML/OutputStream.h"
#include <stdio.h>

namespace LlamaXML {

	class StdioOutputStream: public OutputStream {
		StdioOutputStream(const char * path);
		virtual ~StdioOutputStream();
	    
		virtual void WriteData(const char * buffer, uint32_t length);
	
	private:
		FILE *	mFile;
	};
	
}

#endif
