#include "pcmp.h"
#include "Blob.h"
#include "ArchiveFile.h"
#include <Shlwapi.h>
#include "..\CommonUtils\CommonResourceFolders.h"

Blob::Blob()
{
}

Blob::~Blob()
{
}

bool Blob::Initialize( const TCHAR* pResFile )
{
	ZipArchive ar;
	TCHAR path[MAX_PATH];
	iz3d::resources::GetAllUsersDirectory( path );
	PathAppend(path, _T("resources.zip") );
	unsigned size;

	if ( !ar.Init( path ) )
		return false; 

	if ( !ar.GetUncompressedFileSize( pResFile, size ) )
		return false;

	data_.resize( size );
	ar.UnzipFileToMemory( pResFile, &data_[0] );
	ar.Release();

	return true;
}

const void* Blob::GetData() const
{
	_ASSERT(data_.size() > 0);
	return &data_[0];
}

const size_t Blob::GetSize() const
{
	return data_.size();
}
