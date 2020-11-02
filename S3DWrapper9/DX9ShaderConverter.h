/* 
* Project : iZ3D Stereo Driver
* Copyright (C) iZ3D Inc. 2002 - 2010
*/

#pragma once

#include <atlbase.h>
#include <string>
#include <vector>
#include <d3d9.h>
#include <d3dx9.h>
#include "boost/unordered_map.hpp"
#include <hash_map>
#include "ShaderAnalyzer.h"
#include "ProxyDevice9.h"

template <typename Q>
struct SHADER_PRIVATE_DATA
{
	DWORD   shaderGlobalIndex;
	DWORD	CRC32;
	DWORD	textures; // 32 - textures
	ShaderMatrices matricesData;
	Q*		stereoShader;

	void Clear()
	{
		shaderGlobalIndex = 0;
		CRC32 = 0;
		textures = 0xFFFFFFFF;
		stereoShader = NULL;
		matricesData.matrixSize = 0;
	}
};

struct PS_PRIVATE_DATA : SHADER_PRIVATE_DATA<IDirect3DPixelShader9>
{
	static const DWORD stageOffset = 0;
	static DWORD GetDataSizeInDwords() { return sizeof(PS_PRIVATE_DATA) / sizeof(DWORD); }
	bool	vPosUsed;
	WORD	add4VectorRegister;          // vPos shift data register index 
	bool	shaderIsHeavy;				// used for heavy fullscreen passes (i.e. SSAO) splitting
	bool	splitThisShaderDrawPass;	// --"--

	void Clear()
	{
		SHADER_PRIVATE_DATA::Clear();
		vPosUsed = false;
		add4VectorRegister = 0;
		splitThisShaderDrawPass = shaderIsHeavy = false;
	}
};

C_ASSERT((sizeof(PS_PRIVATE_DATA) % sizeof(DWORD)) == 0);

struct VS_PRIVATE_DATA : public SHADER_PRIVATE_DATA<IDirect3DVertexShader9>
{
	static const DWORD stageOffset = D3DDMAPSAMPLER;
	static DWORD GetDataSizeInDwords() { return sizeof(VS_PRIVATE_DATA) / sizeof(DWORD); }
	WORD	dp4VectorRegister;          // stereo shader data register index 
	WORD	ZNearRegister; 
	bool	shaderIsMono;

	void Clear()
	{
		SHADER_PRIVATE_DATA::Clear();
		dp4VectorRegister = 0;   
		ZNearRegister = 65535;          
		shaderIsMono = false;
	}
};

C_ASSERT((sizeof(VS_PRIVATE_DATA) % sizeof(DWORD)) == 0);

#define	CASHE_FILE_VERSION		15
#define	PRODUCT_CURRENT_VERSION	( ((((PRODUCT_VERSION_MAJOR&0xff)<<8)|(PRODUCT_VERSION_MINOR&0xff))<<16) | (CASHE_FILE_VERSION&0xffff) )

#pragma pack (push, 1)
struct SHADER_CACHE_DATA 
{
	DWORD	CRC32;
	DWORD	textures;
	WORD	flags;						// isTransposed, isIncorrectProj, isShaderMono
	WORD	matrixSize;					// [0, 1]
	WORD	matrixRegister;				// [0, MAX_REG_NUMBER]
	WORD	reserved;

	enum {
		// vs
		isTransposed	= 0x0001,
		isIncorrectProj	= 0x0002,
		isShaderMono	= 0x0004,
		isAnalyzeOkay	= 0x0008,

		// ps
		isvPosUsed		= 0x0001,
		isShaderHeavy	= 0x0002,

		ReservedBits	= 0xfff8
	};

	void Clear()
	{
		CRC32 = 0;
		textures = 0xFFFFFFFF;
		flags = matrixSize = matrixRegister = 0;
		reserved = 0;
	}
};
#pragma pack(pop)

C_ASSERT(sizeof(SHADER_CACHE_DATA) == 16);

typedef std::pair<DWORD, SHADER_CACHE_DATA>				SHADER_CACHE_DATA_PAIR;
typedef boost::unordered_map<DWORD, SHADER_CACHE_DATA>	SHADER_CACHE_DATA_MAP;
typedef	SHADER_CACHE_DATA_MAP::iterator					SHADER_CACHE_DATA_MAP_ITERATOR;

class ShaderCache
{
public:
	void	Init(TCHAR* ext);
	bool	Load( );
	bool	Save( );
	bool	Release( );

	size_t	size( ) { return ShaderCacheData.size(); }
	SHADER_CACHE_DATA_MAP_ITERATOR	find( DWORD crc )	{ return ShaderCacheData.find(crc); }
	SHADER_CACHE_DATA_MAP_ITERATOR	end( )				{ return ShaderCacheData.end(); }
	void insert( DWORD crc, SHADER_CACHE_DATA &data )	{ ShaderCacheData.insert(SHADER_CACHE_DATA_PAIR( crc, data )); }
private:
	FILE *	OpenCacheFile( );
	FILE *	WriteHeader( );

	SHADER_CACHE_DATA_MAP ShaderCacheData;
	TCHAR	CacheFileName[MAX_PATH];
};

template <typename T, typename Q >
class DX9ShaderConverter
{
public:
	DX9ShaderConverter(void);
	~DX9ShaderConverter(void) { Clear(); }

	void    Initialize(ProxyDevice9* pD3DDevice);
	void    Clear();

	T			m_CurrentShaderData;
#ifndef FINAL_RELEASE
	LARGE_INTEGER analyzingTime; 
#endif

	static DWORD			m_GlobalCounter;
	static DWORD			m_ModifiedCounter;
	static ShaderCache		m_ShaderCacheData;

protected:
	ProxyDevice9*			m_pD3DDevice;

	HRESULT CreateStereoShader(CONST DWORD *pMonoFunction, UINT Size, char *shaderText, Q** ppShader);
	HRESULT CreateShader(CONST DWORD *pFunction, IDirect3DVertexShader9** ppShader);
	HRESULT CreateShader(CONST DWORD *pFunction, IDirect3DPixelShader9** ppShader);

	char*   PrepareShaderText(char* inputShader, ID3DXBuffer** outputText, ID3DXBuffer** pComment = NULL);
	bool	FindFreeRegister( DWORD &maxTmpRegister, char* tmpRegisterName, char * first_oPos, char * after_last_oPos, char * shaderText );
	void	WriteShaderTextToFile(TCHAR* fileName, char* shaderText, DWORD Size, bool compileShader = false);
	virtual void WriteShaderDataToFile( FILE * fp ) = 0;
	const ShaderProfileData*	FindProfile(ShaderProfileDataMap &privateDataMap);
	void	CalculateCRCEx( CONST DWORD * pFunction, UINT Size );

	void	SearchMatrixAndAdd( char* commentText, const char* matrixName );
	bool	AddMatrix( int registerIndex );
};

class DX9VertexShaderConverter : public DX9ShaderConverter<VS_PRIVATE_DATA, IDirect3DVertexShader9>
{
	DWORD						m_NumTempRegisters;
	WORD						m_FreeConstantRegister;
	WORD						m_FreeConstantRegister2;
public:
	DX9VertexShaderConverter();
	void	Initialize(ProxyDevice9* pD3DDevice);
	HRESULT	Convert(CONST DWORD *pFunction, IDirect3DVertexShader9** ppShader, IDirect3DVertexShader9** ppModifiedShader );
	bool	ProcessShader( CONST DWORD * pFunction, UINT Size, CComPtr<ID3DXBuffer> &pModifiedShader );

	ShaderAnalysisResult Analyze(DWORD shaderVersion, char* pureShaderText );
	bool	Modify(DWORD shaderVersion, char *shaderText, ID3DXBuffer** outputText, bool AddZNearCheck);
	virtual void WriteShaderDataToFile( FILE * fp );

	void	Convert_1_x_ShaderTo_2_x(DWORD shaderVersion, char* shaderText, ID3DXBuffer** outputText);

	friend bool ShaderTest(char* inputShaderText);
};

#define D3DPS30_NUMCONSTANTS    224

class DX9PixelShaderConverter : public DX9ShaderConverter<PS_PRIVATE_DATA, IDirect3DPixelShader9>
{
public:
	DX9PixelShaderConverter();
	void    Initialize(ProxyDevice9* pD3DDevice);
	HRESULT Convert(CONST DWORD *pFunction, IDirect3DPixelShader9** ppShader, IDirect3DPixelShader9** ppModifiedShader );
	bool	ProcessShader( CONST DWORD * pFunction, UINT Size, CComPtr<ID3DXBuffer> &pModifiedShader );
	bool	Analyze( DWORD shaderVersion, char* pureShaderText );
	bool	AnalyzePSComplexity( char* pureShaderText );
	bool	Modify( DWORD shaderVersion, char *shaderText, ID3DXBuffer** outputText );
	virtual void WriteShaderDataToFile( FILE * fp );
};

template <typename T, typename Q>
void DX9ShaderConverter<T, Q>::Clear()
{
	m_pD3DDevice = 0;
	if( m_ShaderCacheData.size() > 20 )
		m_ShaderCacheData.Save( );
	m_ShaderCacheData.Release();
}
