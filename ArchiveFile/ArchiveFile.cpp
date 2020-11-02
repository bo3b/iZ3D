#include "pcmp.h"
#include "ArchiveFile.h"
#include <zlib.h>
#include "zip.h"
#include "unzip.h"
#include <sys\stat.h>
#include <time.h>

using namespace std;

//////////////////////////////////////////////////////////////////////////

ArchiveFile::ArchiveFile()
:	m_tFileMode( TArchive_UnknownMode )
{
}

ArchiveFile::~ArchiveFile()
{
}

bool ArchiveFile::Init( const TCHAR* szFileName, TArchiveFileMode mode, int nArchiveLevel /* = 0  */ )
{
	_ASSERT( m_tFileMode == TArchive_UnknownMode );
	_ASSERT( mode != TArchive_UnknownMode );
	m_sArchiveFileName = szFileName;
	m_tFileMode = mode;
	m_nArchiveLevel = nArchiveLevel;

	switch( m_tFileMode )
	{
	case TArchive_OpenForDeflate:
		_tfopen_s( &m_pArchiveFile, szFileName, _T("wb") );
		break;
	case TArchive_OpenForInflate:
		_tfopen_s( &m_pArchiveFile, szFileName, _T("rb") );
		break;
	}

	if ( !m_pArchiveFile )
	{
		_ASSERT( m_pArchiveFile );
		m_tFileMode = TArchive_UnknownMode;
	}
	return true;
}

void ArchiveFile::Release()
{
	if ( m_pArchiveFile) 
		fclose( m_pArchiveFile );

	m_tFileMode = TArchive_UnknownMode;	
}

int ArchiveFile::Inflate( const char* pData, size_t nDataSize, const TArchiveChunk& ch )
{
	_ASSERT( m_tFileMode == TArchive_OpenForInflate );
	_ASSERT( pData && nDataSize == ch.nSizeBeforArchive );

	fseek( m_pArchiveFile, ( long )ch.nOffset, SEEK_SET );
	size_t nArchiveSize = ch.nSizeAfterArchive;

	z_stream stream;
	stream.zalloc	= Z_NULL;
	stream.zfree	= Z_NULL;
	stream.opaque	= Z_NULL;
	stream.avail_in	= 0;
	stream.next_in	= Z_NULL;
	int	nRes		= Z_OK;

	nRes = inflateInit(&stream);
	if( nRes != Z_OK )
		return nRes;

	do
	{
		stream.avail_in = (uInt)fread( m_pArchiveBuffer, 1, ( nArchiveSize > TArchive_BufferSize ) ? TArchive_BufferSize : nArchiveSize, m_pArchiveFile );

		if ( ferror( m_pArchiveFile ))
		{
			inflateEnd( &stream );
			return Z_ERRNO;
		}

		if ( !stream.avail_in )
			break;

		stream.next_in = m_pArchiveBuffer;

		do
		{
			stream.avail_out	= (uInt)nDataSize;
			stream.next_out		= (Bytef*)pData;
			nRes = inflate( &stream, Z_NO_FLUSH );

			switch (nRes)
			{
			case Z_NEED_DICT:
				nRes = Z_DATA_ERROR;
			case Z_DATA_ERROR:
			case Z_MEM_ERROR:
				inflateEnd(&stream);
				return nRes;
			}

			unsigned int nActualSize = ( unsigned int) nDataSize - stream.avail_out;
			_ASSERT( nActualSize >= 0 );
			pData		+= nActualSize;
			nDataSize	-= nActualSize;			
		}
		while( stream.avail_out == 0 && nDataSize );

		nArchiveSize = ( nArchiveSize >= TArchive_BufferSize ) ? nArchiveSize - TArchive_BufferSize : 0;		
	}
	while( nArchiveSize > 0 );

	return Z_OK;
}

int ArchiveFile::Deflate( const char* pData, size_t nDataSize, TArchiveChunk& ch )
{
	_ASSERT( m_tFileMode == TArchive_OpenForDeflate );
	_ASSERT( pData && nDataSize );
	memset( &ch, 0, sizeof( TArchiveChunk ) );

	ch.nSizeBeforArchive	= nDataSize;
	ch.nOffset				= ftell( m_pArchiveFile );

	z_stream stream;
	stream.zalloc	= Z_NULL;
	stream.zfree	= Z_NULL;
	stream.opaque	= Z_NULL;
	int	nRes		= Z_OK;
	int nFlush		= Z_NO_FLUSH;

	nRes = deflateInit( &stream, m_nArchiveLevel );
	if ( nRes != Z_OK )
		return nRes;

	do
	{
		stream.avail_in = (uInt)(( nDataSize > TArchive_BufferSize ) ? TArchive_BufferSize : nDataSize );
		nFlush = ( nDataSize <= TArchive_BufferSize ) ? Z_FINISH : Z_NO_FLUSH;
		stream.next_in = (Bytef*)pData;

		do 
		{
			stream.avail_out	= TArchive_BufferSize;
			stream.next_out		= m_pArchiveBuffer;
			nRes = deflate( &stream, nFlush );
			unsigned int nActualSize = TArchive_BufferSize - stream.avail_out;
			ch.nSizeAfterArchive += nActualSize;

			if ( fwrite( m_pArchiveBuffer, 1, nActualSize, m_pArchiveFile ) != nActualSize || ferror( m_pArchiveFile ) )
			{
				deflateEnd( &stream );
				return Z_ERRNO;
			}
		}
		while(stream.avail_out == 0 );


		if ( nDataSize > TArchive_BufferSize )	
		{
			pData		+= TArchive_BufferSize;
			nDataSize	-= TArchive_BufferSize;
		}
		else
			nDataSize = 0;

	}
	while( nFlush != Z_FINISH );

	deflateEnd(&stream);
	return Z_OK;
}

//////////////////////////////////////////////////////////////////////////

ZipArchive::ZipArchive()
:	m_nCompressionLevel ( 0		)
{
}

ZipArchive::~ZipArchive()
{
}

bool ZipArchive::Init( const TCHAR* szFileName_, int nCompressionLevel_ /* = 0 */ )
{
	m_sZipFileName = szFileName_;
	m_nCompressionLevel = nCompressionLevel_;
	return true;
}

void ZipArchive::Release()
{
}

bool ZipArchive::AddFile( const TCHAR* szFileName_ )	
{
	_ASSERT( szFileName_ );
	pair< TZipFilesMap::iterator, bool > result = m_Files.insert( TZipFilesMap::value_type( wstring( szFileName_ ), true ) );
	return result.second;
}

bool ZipArchive::ZipAllFiles() const
{
	if ( !m_Files.size() )
		return false;

	TZipFilesMap::const_iterator it = m_Files.begin();
	TCHAR* szFileName = (TCHAR*)m_sZipFileName.c_str();
	zipFile pZip = zipOpen( szFileName, 0 );
	if ( !pZip )
		return false;

	bool bResult = false;
	while ( it != m_Files.end() )
	{
		bResult = false;
		const wchar_t* szFullFileName	= ( const TCHAR* )it->first.c_str();
		const wchar_t* szLastSlash		= wcsrchr( szFullFileName, '\\' );
		const wchar_t* szFileName		= ( szLastSlash ) ? szLastSlash + 1 : szFullFileName;

		FILE* pFile = NULL;
		_wfopen_s( &pFile, szFullFileName, L"rb" );
		_ASSERT( pFile );
		if( !pFile )
			continue;

		zip_fileinfo zfi	= {0};
		struct _stat fStat	= {0};
		_fstat(_fileno(pFile), &fStat );
		struct tm fileTime;
		localtime_s( &fileTime, &fStat.st_mtime );
		tm_zip* zipTime		= &zfi.tmz_date;
		memcpy( zipTime, &fileTime, sizeof( tm_zip ) );

		if ( ZIP_OK == zipOpenNewFileInZip( pZip, szFileName, &zfi, NULL, 0, NULL, 0, NULL, Z_DEFLATED, m_nCompressionLevel ) )
		{
			while( !feof( pFile ) )
			{
				bResult = false;
				size_t nRealCount = fread( ( void* )m_pZipBuffer, sizeof( BYTE ), TZip_BufferSize, pFile );
				if ( ferror( pFile ) )
					break;

				if ( ZIP_OK == zipWriteInFileInZip( pZip, m_pZipBuffer, ( unsigned int )nRealCount ) )
					bResult = true;
			}
			bResult = bResult && ( Z_OK == zipCloseFileInZip( pZip ) );
		}
		bResult = bResult && ( fclose( pFile ) == 0 );		
		bResult ? it++ : m_Files.end();
	}

	bResult = bResult && ( Z_OK == zipClose( pZip, NULL ) );
	return bResult;
}


bool ZipArchive::UnzipFile( const TCHAR* szSrcFileName_, const TCHAR* szDstFileName_ )
{
	bool bResult = false;

	char szFileNameAnsi[ MAX_PATH ];
	_ASSERT( wcslen( szSrcFileName_ ) < MAX_PATH );

#ifdef _UNICODE	
	WideCharToMultiByte(CP_ACP, 0, szSrcFileName_, -1, (LPSTR)szFileNameAnsi, MAX_PATH, NULL, NULL );
#else 
	strcpy( szFileNameAnsi, szSrcFileName_ );
#endif

	unzFile pZip = unzOpen( m_sZipFileName.c_str() );
	if ( !pZip )
		return false;

	bResult =  ( unzLocateFile( pZip, szFileNameAnsi, 0 ) == UNZ_OK );

	if ( bResult )
	{
		bResult = bResult && ( unzOpenCurrentFile( pZip ) == UNZ_OK );
		unz_file_info inf;

		FILE* pFile = NULL;
		_wfopen_s( &pFile, szDstFileName_, L"wb" );
		bResult = bResult && pFile;

		if ( bResult )
		{
			bResult = bResult && ( unzGetCurrentFileInfo( pZip, &inf, NULL, 0, NULL, 0, NULL , 0 ) == UNZ_OK );
			int nUncompressedSize = inf.uncompressed_size;

			int nRealCount = 0;
			do 
			{
				nRealCount = unzReadCurrentFile( pZip, m_pZipBuffer, TZip_BufferSize );
				fwrite( m_pZipBuffer, 1, nRealCount, pFile );
			}
			while ( nRealCount );
		}

		fclose( pFile );

		bResult = bResult && ( unzCloseCurrentFile( pZip ) == UNZ_OK );
	}

	bResult = bResult && ( unzClose( pZip ) == UNZ_OK )  ;

	return bResult;
}

bool ZipArchive::GetUncompressedFileSize( const TCHAR* szFileName_, unsigned& nUncompressedSize_ ) const
{
	bool bResult = false;

	char szFileNameAnsi[ MAX_PATH ];
	size_t nSize = wcslen( szFileName_ );
	_ASSERT( wcslen( szFileName_ ) < MAX_PATH );

#ifdef _UNICODE	
	WideCharToMultiByte(CP_ACP, 0, szFileName_, -1, (LPSTR)szFileNameAnsi, MAX_PATH, NULL, NULL );
#else 
	strcpy( szFileNameAnsi, szFileName_ );
#endif

	unzFile pZip = unzOpen( m_sZipFileName.c_str() );
	if ( !pZip )
		return false;

	bResult =  ( unzLocateFile( pZip, szFileNameAnsi, 0 ) == UNZ_OK );

	if ( bResult )
	{
		bResult = bResult && ( unzOpenCurrentFile( pZip ) == UNZ_OK );
		unz_file_info inf;

		if ( bResult )
		{
			bResult = bResult && ( unzGetCurrentFileInfo( pZip, &inf, NULL, 0, NULL, 0, NULL , 0 ) == UNZ_OK );
			nUncompressedSize_ = inf.uncompressed_size;
		}
		bResult = bResult && ( unzCloseCurrentFile( pZip ) == UNZ_OK );
	}

	bResult = bResult && ( unzClose( pZip ) == UNZ_OK )  ;

	return bResult;
}

bool ZipArchive::UnzipFileToMemory( const TCHAR* szSrcFileName_, BYTE* pMem_ )
{
	bool bResult = false;

	char szFileNameAnsi[ MAX_PATH ];
	_ASSERT( wcslen( szSrcFileName_ ) < MAX_PATH );

#ifdef _UNICODE	
	WideCharToMultiByte(CP_ACP, 0, szSrcFileName_, -1, (LPSTR)szFileNameAnsi, MAX_PATH, NULL, NULL );
#else 
	strcpy( szFileNameAnsi, szSrcFileName_ );
#endif

	unzFile pZip = unzOpen( m_sZipFileName.c_str() );
	if ( !pZip )
		return false;

	bResult =  ( unzLocateFile( pZip, szFileNameAnsi, 0 ) == UNZ_OK );

	if ( bResult )
	{
		bResult = bResult && ( unzOpenCurrentFile( pZip ) == UNZ_OK );
		unz_file_info inf;	

		if ( bResult )
		{
			bResult = bResult && ( unzGetCurrentFileInfo( pZip, &inf, NULL, 0, NULL, 0, NULL , 0 ) == UNZ_OK );
			int nRealCount = 0;
			nRealCount = unzReadCurrentFile( pZip, pMem_, inf.uncompressed_size );
			_ASSERT( inf.uncompressed_size == nRealCount );
		}	

		bResult = bResult && ( unzCloseCurrentFile( pZip ) == UNZ_OK );
	}

	bResult = bResult && ( unzClose( pZip ) == UNZ_OK )  ;

	return bResult;
}

//////////////////////////////////////////////////////////////////////////