#ifndef __S3D_ARCHIVEFILE_H__
#define __S3D_ARCHIVEFILE_H__

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

	bool			Init			( const TCHAR* szFileName, TArchiveFileMode mode, int nArchiveLevel = 0 );
	void			Release			();

	int				Inflate			( const char* pData, size_t nDataSize, const TArchiveChunk& ch );
	int				Deflate			( const char* pData, size_t nDataSize, TArchiveChunk& ch );

	inline void		SetArchiveLevel	( int nLevel ) { m_nArchiveLevel = nLevel; };

protected:
private:	

	enum
	{
		TArchive_BufferSize = 16384,
	};

	FILE*						m_pArchiveFile;
	TArchiveFileMode			m_tFileMode;
	int							m_nArchiveLevel;
	std::wstring				m_sArchiveFileName;
	unsigned char				m_pArchiveBuffer[ TArchive_BufferSize ];	
};

#endif//__S3D_ARCHIVEFILE_H__