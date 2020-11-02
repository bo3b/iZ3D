#pragma once 

//=============================================================================
//	DumpFile class draft code
//=============================================================================

#ifndef __S3D_WRAPPER_DIRECT3D_10_DUMP_FILE_H__
#define __S3D_WRAPPER_DIRECT3D_10_DUMP_FILE_H__

#include "stdafx.h"
#include <stdio.h>
#include <string.h>
#include "version.h"

#define	CURRENT_VERSION		((((PRODUCT_VERSION_MAJOR) & 0xffff)<<16) | ((PRODUCT_VERSION_MINOR) & 0xffff))


//-----------------------------------------------------------------------------
// Used-defined types
//-----------------------------------------------------------------------------

struct DUMP_FILE_HEADER
{
	DWORD	Signature;					// The magic word like MAKEFOURCC('i','Z','3','D');
	union {
		DWORD Version;
		struct {
			WORD	Minor;
			WORD	Major;
		} VersionPart;
	};
	DWORD	Signature2;
	DWORD	Reserved;
};	// sizeof(DUMP_FILE_HEADER) == 16 bytes


//-----------------------------------------------------------------------------
// Used-defined classes
//-----------------------------------------------------------------------------

class DumpFile {
public:
	DumpFile()						{ fp_ = NULL;					}
	~DumpFile();					{ Close();						}

	bool Open( const TCHAR *filename, DWORD signature2 = MAKEFOURCC('C','B','B','D') );
	bool Close()					{ if( fp_ ) fclose( fp );

	template <class T>				UINT GetSizeOf( T &data, UINT num=1 )
	{
		UINT	siz = sizeof( data )*num;
		return	(siz & 0x03) ? ((siz | 0x03)+1) : siz;
	}

	template <class T>				bool Write( const T &data );
	template <class T>				bool Write( const T *ptr, UINT num=1 );
	template <class T>				bool WriteMem( const T *ptr, UINT num=1 );
	//template <class T>			bool WriteResource( VOID *resource );

protected:
	FILE	*fp_;
	TCHAR	filename_[MAX_PATH];

private:
	DumpFile( DumpFile &t )			{}
	DumpFile& operator=( void )		{}
};


//-----------------------------------------------------------------------------
// Used-defined functions
//-----------------------------------------------------------------------------

bool DumpFile::Open( const TCHAR *filename, DWORD signature2 )
{
	if( fp_ ) Close();
	if( !filename ) return false;

	strcpy_s( filename_, MAX_PATH, filename );
	fopen_s( &fp_, filename_, "wb" );
	if( !fp_ ) return false;

	DUMP_FILE_HEADER header;
	header.Signature	= MAKEFOURCC('i','Z','3','D');
	header.Version		= CURRENT_VERSION;
	header.Signature2	= signature2;
	//header.Signature2	= MAKEFOURCC('C','B','B','D');		// 'CBBD' == Command Buffer Binary Dump
	header.Reserved		= 0;

	if( fwrite( (void *)&header, sizeof(DUMP_FILE_HEADER), 1, fp_ )!= 1 ) return false;

	return true;
}

template <class T>
bool DumpFile::Write( const T &data )
{
	if( fwrite( &data, GetSizeOf( data ), 1, fp_ )!= 1 ) return false;
	return true;
}

template <class T>
bool DumpFile::Write( const T *ptr, UINT num=1 )
{
	if( fwrite( &data, GetSizeOf( data, num ), 1, fp_ )!= 1 ) return false;
	return true;
}

template <class T>
bool DumpFile::WriteMem( const T *ptr, UINT num=1 )
{
	return true;
}

#if 0
		//if( !gInfo.DumpEnabled || !gInfo.DumpFrameEnabled ) return false;

		out <<	COMMAND_BEGIN( CommandsId_, "ResourceMap" );		// N.B! EventID is generated automatically

		out <<	DEVICE_REF( hDevice, "main device reference" ); 
		out <<	RESOURCE_REF( "hResource", hResource ); 
		out <<	UINT_VAL( "Subresource", Subresource ); 
		out <<	DDIMap; 
		out <<	Flags; 
		out <<	pMappedSubResource;

		out <<	COMMAND_END;

		//gInfo.pDumpFile_->Write( hDevice ); 
		//gInfo.pDumpFile_->Wrire( hResource ); 
		//gInfo.pDumpFile_->Write( Subresource ); 
		//gInfo.pDumpFile_->Write( DDIMap ); 
		//gInfo.pDumpFile_->Write( Flags ); 
		//gInfo.pDumpFile_->Write( pMappedSubResource );
#endif

#if 0
		if( gInfo.DumpEnabled && gInfo.DumpFrameEnabled )
		{
			// Write command header 
			gIngo.DumpFile->Write( MAKEFOURCC('i','Z','\0','\0') ^ (DWORD)(CommandId) );
			//(*gIngo.DumpFile) << CommandId;
			//(*gIngo.DumpFile) << EventID;
			gIngo.DumpFile->Write( sizeof(DWORD)*2 + sizeof(D3D10DDI_HRESOURCE)*NumBuffers_ );
			// Write command data
			gIngo.DumpFile->Write( StartBuffer_ ); 
			gIngo.DumpFile->Write( NumBuffers_ );
			if( NumBuffers_ > 0 )
				gIngo.DumpFile->WriteMem( phBuffers_, sizeof(D3D10DDI_HRESOURCE)*NumBuffers_); // pad to DWORD
		}
#endif

#endif
