#ifndef __S3D_ARCHIVEFILE_H__
#define __S3D_ARCHIVEFILE_H__

//////////////////////////////////////////////////////////////////////////
#include <map>
class ArchiveFile
{
public:

	typedef enum
	{
		TArchive_UnknownMode,
		TArchive_OpenForDeflate,
		TArchive_OpenForInflate,
	}
	TArchiveFileMode;

	typedef struct 
	{
		size_t	nOffset;
		size_t	nSizeBeforArchive;
		size_t	nSizeAfterArchive;
	}
	TArchiveChunk;

public:
	ArchiveFile		();
	~ArchiveFile	();

	virtual bool	Init			( const TCHAR* szFileName, TArchiveFileMode mode, int nArchiveLevel = 0 );
	virtual void	Release			();

	int				Inflate			( const char* pData, size_t nDataSize, const TArchiveChunk& ch );
	int				Deflate			( const char* pData, size_t nDataSize, TArchiveChunk& ch );

	inline void		SetArchiveLevel	( int nLevel ) { m_nArchiveLevel = nLevel; };

protected:

	enum
	{
		TArchive_BufferSize = 16384,
	};

	FILE*						m_pArchiveFile;
	TArchiveFileMode			m_tFileMode;
	int							m_nArchiveLevel;
	std::wstring				m_sArchiveFileName;
	unsigned char				m_pArchiveBuffer[ TArchive_BufferSize ];

private:	
};

//////////////////////////////////////////////////////////////////////////

class ZipArchive
{
public:

	ZipArchive				();
	~ZipArchive				();

	bool	Init					( const TCHAR* szFileName_, int nCompressionLevel_ = 0 );
	void	Release					();
	bool	AddFile					( const TCHAR* szFileName_ );

	bool	ZipAllFiles				() const;							

	bool	GetUncompressedFileSize	( const TCHAR* szFileName_, unsigned& nUncompressedSize_ ) const;	
	bool	UnzipFile				( const TCHAR* szSrcFileName_, const TCHAR* szDstFileName_ );
	bool	UnzipFileToMemory		( const TCHAR* szSrcFileName_, BYTE* pMem_ );

protected:

	enum
	{
		TZip_BufferSize = 16384,
	};

	struct InternalFuncLess : public std::binary_function< std::wstring, std::wstring, bool >
	{
		bool operator()( const std::wstring &s1_, const std::wstring &s2_ ) const
		{
			return _wcsicmp( s1_.c_str(), s2_.c_str() ) < 0;
		}
	};
	typedef std::map< std::wstring, bool, InternalFuncLess > TZipFilesMap;

private:

	TZipFilesMap			m_Files;
	std::wstring			m_sZipFileName;
	int						m_nCompressionLevel;
	char					m_pZipBuffer[ TZip_BufferSize ];
};

//////////////////////////////////////////////////////////////////////////

#endif//__S3D_ARCHIVEFILE_H__