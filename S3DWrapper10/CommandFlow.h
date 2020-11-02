#pragma once

#ifndef __S3D_WRAPPER_DIRECT3D_10_COMMAND_FLOW_H__
#define __S3D_WRAPPER_DIRECT3D_10_COMMAND_FLOW_H__



struct D3D10WRAPPER_COMMAND {
/*	
	DWORD	Signature;				// The magic word like MAKEFOURCC('i','Z','3','D');
	DWORD	ChunkSize;
*/

	CommandsId	CommandId;			// XOR'ed with MAKEFOURCC('i','Z','\0','\0');
	DWORD		DataSize;

	DWORD		EventID;

	Commands::Command	command_;
};



#endif