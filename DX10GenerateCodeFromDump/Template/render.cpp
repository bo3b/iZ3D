#include "pcmp.h"
#include "render.h"
#include "global.h"

using namespace std;
//////////////////////////////////////////////////////////////////////////

CRender* CRender::m_pRender = NULL;

/************************************************************************/
/* Init                                                                 */
/************************************************************************/
HRESULT CRender::Init()
{
	if ( m_pRender )
		return E_FAIL;

	m_pRender = new CRender();
	return m_pRender->InternalInit();
}

/************************************************************************/
/* Release                                                              */
/************************************************************************/
void CRender::Release()
{
	if ( !m_pRender )
		return;

	m_pRender->InternalRelease();
	delete m_pRender;
	m_pRender = NULL;
}

/************************************************************************/
/* DrawFrame                                                            */
/************************************************************************/
HRESULT CRender::DrawFrame()
{
	_ASSERT( m_pRender );
	return m_pRender->InternalDrawFrame();
}

//////////////////////////////////////////////////////////////////////////

/************************************************************************/
/* Render                                                               */
/************************************************************************/
CRender::CRender()
	:	m_pSwapChain		( NULL )
	,	m_pDevice			( NULL )
	,	m_pRenderTargetView	( NULL )
	,	m_pSimpleEffect		( NULL )
	,	m_pSimpleTechnique	( NULL )
	,	m_pInputLayout		( NULL )
{
	m_pDevContext = 0;
}

/************************************************************************/
/* ~Render                                                              */
/************************************************************************/
CRender::~CRender()
{
	_ASSERT( !m_pRenderTargetView );
	_ASSERT( !m_pSwapChain );
	_ASSERT( !m_pDevice );
}

/************************************************************************/
/* InternalInit                                                         */
/************************************************************************/
HRESULT CRender::InternalInit()
{
	if ( FAILED( InternalInitDevice()) )
		return E_FAIL;

	if ( FAILED( InternalCreateRenderTarget() ) )
		return E_FAIL;

	if ( FAILED( InternalInitViewport() ) )
		return E_FAIL;

	FrameInit(m_pDevice);

	//	if ( FAILED( InternalCreateTechnique() ) )
	//		return E_FAIL;

	//	if ( FAILED( InternalCreateMesh() ) )
	//		return E_FAIL;

	//	m_pDevice->IASetInputLayout( m_pInputLayout );

	return S_OK;
}

/************************************************************************/
/* InternalRelease                                                      */
/************************************************************************/
void CRender::InternalRelease()
{
	FrameRelease();

	SAFE_RELEASE( m_pInputLayout );
	SAFE_RELEASE( m_pSimpleEffect );
	SAFE_RELEASE( m_pRenderTargetView );
	SAFE_RELEASE( m_pSwapChain );
	SAFE_RELEASE( m_pDevice );
}

/************************************************************************/
/* InternalDrawFrame                                                    */
/************************************************************************/
HRESULT CRender::InternalDrawFrame()
{
	_ASSERT( m_pDevice );

	float BackBufferColor[4] = { 0.4f, 0.525f, 0.6f, 1.0f };
	DXDEVCONTEXT->ClearRenderTargetView( m_pRenderTargetView, BackBufferColor );

	FrameDraw(m_pDevice,m_pSwapChain,m_pRenderTargetView,pBackBuffer,m_pDevContext);
	/*
	ID3D10Texture2D* pBuffer;
	ID3D10RenderTargetView* g_pRenderTargetView = NULL;
	CALL(hr = swapChain,GetBuffer, 0, __uuidof( ID3D10Texture2D ), ( LPVOID* )&pBuffer );

	CALL(hr = device,CreateRenderTargetView, pBuffer, NULL, &g_pRenderTargetView );
	pBuffer->Release();

	CALL(device,OMSetRenderTargets, 1, &g_pRenderTargetView, NULL );
	*/

	m_pSwapChain->Present( 0, 0 );
	return S_OK;
}

//////////////////////////////////////////////////////////////////////////

extern DXGI_FORMAT mainFormat;

/************************************************************************/
/* InternalInitdevice                                                   */
/************************************************************************/
HRESULT CRender::InternalInitDevice()
{	
	// Look for 'NVIDIA PerfHUD' adapter
	// If it is present, override default settings
	IDXGIFactory *pDXGIFactory;
	HRESULT hRes;
	hRes = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&pDXGIFactory);
	// Search for a PerfHUD adapter.
	UINT nAdapter = 0;
	IDXGIAdapter* adapter = NULL;
	IDXGIAdapter* selectedAdapter = NULL;
	D3D10_DRIVER_TYPE driverType = D3D10_DRIVER_TYPE_HARDWARE;

	while (pDXGIFactory->EnumAdapters(nAdapter, &adapter) != DXGI_ERROR_NOT_FOUND)
	{
		if (adapter)
		{
			DXGI_ADAPTER_DESC adaptDesc;
			if (SUCCEEDED(adapter->GetDesc(&adaptDesc)))
			{
				const bool isPerfHUD = wcscmp(adaptDesc.Description, L"NVIDIA PerfHUD") == 0;
				// Select the first adapter in normal circumstances or the PerfHUD one if it exists.
				if(nAdapter == 0 || isPerfHUD)
					selectedAdapter = adapter;
				if(isPerfHUD)
					driverType = D3D10_DRIVER_TYPE_REFERENCE;
			}
		}
		++nAdapter;
	}

	DXGI_SWAP_CHAIN_DESC swDesc;
	memset( &swDesc, 0, sizeof( DXGI_SWAP_CHAIN_DESC ) );
	swDesc.BufferCount							= 1;
	swDesc.BufferUsage							= DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swDesc.BufferDesc.Width						= g_nWndWidth;
	swDesc.BufferDesc.Height					= g_nWndHeight;
	swDesc.BufferDesc.Format					= mainFormat;
	swDesc.BufferDesc.RefreshRate.Numerator		= 60;
	swDesc.BufferDesc.RefreshRate.Denominator	= 1;
	swDesc.OutputWindow							= g_hWnd;
	swDesc.SampleDesc.Count						= 1;
	swDesc.SampleDesc.Quality					= 0;
	swDesc.Windowed								= !g_bFullscreen;

	return DXCREATE(selectedAdapter,driverType,swDesc,m_pSwapChain,m_pDevice,m_pDevContext);

	/*return D3D10CreateDeviceAndSwapChain1( selectedAdapter, driverType , NULL, D3D10_CREATE_DEVICE_SINGLETHREADED, 
	D3D10_FEATURE_LEVEL_10_0,
	D3D10_1_SDK_VERSION,
	&swDesc, &m_pSwapChain, &m_pDevice );*/

	/*return D3D10CreateDeviceAndSwapChain1( NULL, D3D10_DRIVER_TYPE_HARDWARE , NULL, D3D10_CREATE_DEVICE_SINGLETHREADED, 
	D3D10_FEATURE_LEVEL_10_0,
	D3D10_1_SDK_VERSION,
	&swDesc, &m_pSwapChain, &m_pDevice );*/
}

/************************************************************************/
/* InternalInitViewport                                                 */
/************************************************************************/
HRESULT CRender::InternalInitViewport()
{
	/*D3D10_VIEWPORT vp;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	vp.Width	= g_nWndWidth;
	vp.Height	= g_nWndHeight;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	m_pDevice->RSSetViewports( 1, &vp );*/
	return S_OK;
}

/************************************************************************/
/* InternalCreateRenderTarget                                           */
/************************************************************************/
HRESULT CRender::InternalCreateRenderTarget()
{
	pBackBuffer = NULL;
	if ( FAILED( m_pSwapChain->GetBuffer( 0, __uuidof(DXTEXTURE2D), ( LPVOID* ) &pBackBuffer ) ) )
		return E_FAIL;

	HRESULT hResult = m_pDevice->CreateRenderTargetView( pBackBuffer, NULL, &m_pRenderTargetView );
	pBackBuffer->Release();

	if ( FAILED( hResult ) )
		return E_FAIL;

	DXDEVCONTEXT->OMSetRenderTargets( 1, &m_pRenderTargetView, NULL );

	return S_OK;
}

/************************************************************************/
/* InternalCreateTechnique                                              */
/************************************************************************/
HRESULT CRender::InternalCreateTechnique()
{
	return S_FALSE;
	/*if ( FAILED( D3DX10CreateEffectFromFile( GetShaderPath( L"simple.fx" ), NULL, NULL, "fx_4_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, m_pDevice, NULL, NULL, &m_pSimpleEffect, NULL, NULL )))
	{
	MessageBox( NULL, L"failed create shader", GetShaderPath( L"simple.fx" ), MB_OK | MB_TOPMOST );
	return E_FAIL;
	}

	m_pSimpleTechnique = m_pSimpleEffect->GetTechniqueByName( "Simple" );
	if ( !m_pSimpleEffect )
	return E_FAIL;

	return S_OK;*/
}


/************************************************************************/
/* InternalCreateMesh                                                   */
/************************************************************************/
HRESULT CRender::InternalCreateMesh()
{	
	return S_FALSE;
	/*D3D10_INPUT_ELEMENT_DESC pLayout[]=
	{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 },
	};
	UINT nNumElements = 1;

	D3D10_PASS_DESC desc;
	if ( FAILED( m_pSimpleTechnique->GetPassByIndex( 0 )->GetDesc( &desc ) ) )
	return E_FAIL;

	if ( FAILED( m_pDevice->CreateInputLayout( pLayout, nNumElements, desc.pIAInputSignature, desc.IAInputSignatureSize, &m_pInputLayout ) ) )
	return E_FAIL;	

	return S_OK;*/
}

static char *fileData = 0;
static UINT64 fileLen;

void LoadFileToMemory()
{
	if (fileData)
		return;

	using namespace std;
	ifstream myFile;
	myFile.open("data2.bin", ios::in | ios::binary);
	if (!myFile.is_open())
	{
		myFile.open("..\\data2.bin", ios::in | ios::binary);
		if (!myFile.is_open())
			exit(-3);
	}

	UINT64 begin = myFile.tellg();
	myFile.seekg(0,ios::end);
	UINT64 end = myFile.tellg();
	myFile.seekg(0,ios::beg);

	fileLen = end - begin;
	fileData = new char[(UINT)fileLen];

	myFile.read(fileData,fileLen);
	myFile.close();

	const char buf[] = { 0,0,0,0,'i','Z','3','D',0,0,0,2 };
	_ASSERT(memcmp(fileData,buf,sizeof(buf)) == 0);
}

char *GetFromFile2(int pos,DWORD *pLen = 0,DWORD *pitch1 = 0,DWORD *pitch2 = 0)
{
	_ASSERT(pos >= 0);

	if (!fileData)
		LoadFileToMemory();

	_ASSERT(pos + sizeof(DWORD) * 3 <= fileLen);
	_ASSERT(*(DWORD *)(fileData + pos) == 2);
	pos += sizeof(DWORD);

	if (pitch1) *pitch1 = *(DWORD *)(fileData + pos);
	pos += sizeof(DWORD);

	if (pitch2) *pitch2 = *(DWORD *)(fileData + pos);
	pos += sizeof(DWORD);

	_ASSERT(pos + sizeof(DWORD) <= fileLen);
	DWORD *len = (DWORD *)(fileData + pos);
	pos += sizeof(DWORD);

	if (pLen) *pLen = *len;

	_ASSERT(pos + *len <= fileLen);
	return fileData + pos;
}

UINT SizeOfFormatElement(DXGI_FORMAT format)
{
	switch (format)
	{
	case DXGI_FORMAT_R32G32B32A32_TYPELESS:
	case DXGI_FORMAT_R32G32B32A32_FLOAT:
	case DXGI_FORMAT_R32G32B32A32_UINT:
	case DXGI_FORMAT_R32G32B32A32_SINT:
		return 128;

	case DXGI_FORMAT_R32G32B32_TYPELESS:
	case DXGI_FORMAT_R32G32B32_FLOAT:
	case DXGI_FORMAT_R32G32B32_UINT:
	case DXGI_FORMAT_R32G32B32_SINT:
		return 96;

	case DXGI_FORMAT_R16G16B16A16_TYPELESS:
	case DXGI_FORMAT_R16G16B16A16_FLOAT:
	case DXGI_FORMAT_R16G16B16A16_UNORM:
	case DXGI_FORMAT_R16G16B16A16_UINT:
	case DXGI_FORMAT_R16G16B16A16_SNORM:
	case DXGI_FORMAT_R16G16B16A16_SINT:
	case DXGI_FORMAT_R32G32_TYPELESS:
	case DXGI_FORMAT_R32G32_FLOAT:
	case DXGI_FORMAT_R32G32_UINT:
	case DXGI_FORMAT_R32G32_SINT:
	case DXGI_FORMAT_R32G8X24_TYPELESS:
	case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
	case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
	case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
		return 64;

	case DXGI_FORMAT_R10G10B10A2_TYPELESS:
	case DXGI_FORMAT_R10G10B10A2_UNORM:
	case DXGI_FORMAT_R10G10B10A2_UINT:
	case DXGI_FORMAT_R11G11B10_FLOAT:
	case DXGI_FORMAT_R8G8B8A8_TYPELESS:
	case DXGI_FORMAT_R8G8B8A8_UNORM:
	case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
	case DXGI_FORMAT_R8G8B8A8_UINT:
	case DXGI_FORMAT_R8G8B8A8_SNORM:
	case DXGI_FORMAT_R8G8B8A8_SINT:
	case DXGI_FORMAT_R16G16_TYPELESS:
	case DXGI_FORMAT_R16G16_FLOAT:
	case DXGI_FORMAT_R16G16_UNORM:
	case DXGI_FORMAT_R16G16_UINT:
	case DXGI_FORMAT_R16G16_SNORM:
	case DXGI_FORMAT_R16G16_SINT:
	case DXGI_FORMAT_R32_TYPELESS:
	case DXGI_FORMAT_D32_FLOAT:
	case DXGI_FORMAT_R32_FLOAT:
	case DXGI_FORMAT_R32_UINT:
	case DXGI_FORMAT_R32_SINT:
	case DXGI_FORMAT_R24G8_TYPELESS:
	case DXGI_FORMAT_D24_UNORM_S8_UINT:
	case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
	case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
	case DXGI_FORMAT_B8G8R8A8_UNORM:
	case DXGI_FORMAT_B8G8R8X8_UNORM:
	case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
		return 32;

	case DXGI_FORMAT_R8G8_TYPELESS:
	case DXGI_FORMAT_R8G8_UNORM:
	case DXGI_FORMAT_R8G8_UINT:
	case DXGI_FORMAT_R8G8_SNORM:
	case DXGI_FORMAT_R8G8_SINT:
	case DXGI_FORMAT_R16_TYPELESS:
	case DXGI_FORMAT_R16_FLOAT:
	case DXGI_FORMAT_D16_UNORM:
	case DXGI_FORMAT_R16_UNORM:
	case DXGI_FORMAT_R16_UINT:
	case DXGI_FORMAT_R16_SNORM:
	case DXGI_FORMAT_R16_SINT:
	case DXGI_FORMAT_B5G6R5_UNORM:
	case DXGI_FORMAT_B5G5R5A1_UNORM:
		return 16;

	case DXGI_FORMAT_R8_TYPELESS:
	case DXGI_FORMAT_R8_UNORM:
	case DXGI_FORMAT_R8_UINT:
	case DXGI_FORMAT_R8_SNORM:
	case DXGI_FORMAT_R8_SINT:
	case DXGI_FORMAT_A8_UNORM:
		return 8;

		// Compressed format; http://msdn2.microsoft.com/en-us/library/bb694531(VS.85).aspx
	case DXGI_FORMAT_BC2_TYPELESS:
	case DXGI_FORMAT_BC2_UNORM:
	case DXGI_FORMAT_BC2_UNORM_SRGB:
	case DXGI_FORMAT_BC3_TYPELESS:
	case DXGI_FORMAT_BC3_UNORM:
	case DXGI_FORMAT_BC3_UNORM_SRGB:
	case DXGI_FORMAT_BC5_TYPELESS:
	case DXGI_FORMAT_BC5_UNORM:
	case DXGI_FORMAT_BC5_SNORM:
		return 128 / 4; // Far Cry 2 fix

		// Compressed format; http://msdn2.microsoft.com/en-us/library/bb694531(VS.85).aspx
		//case DXGI_FORMAT_R1_UNORM:
	case DXGI_FORMAT_BC1_TYPELESS:
	case DXGI_FORMAT_BC1_UNORM:
	case DXGI_FORMAT_BC1_UNORM_SRGB:
	case DXGI_FORMAT_BC4_TYPELESS:
	case DXGI_FORMAT_BC4_UNORM:
	case DXGI_FORMAT_BC4_SNORM:
		return 64 / 4; // Resident Evil 5 fix

		// Compressed format; http://msdn2.microsoft.com/en-us/library/bb694531(VS.85).aspx
	case DXGI_FORMAT_R9G9B9E5_SHAREDEXP:
		return 32;

		// These are compressed, but bit-size information is unclear.
	case DXGI_FORMAT_R8G8_B8G8_UNORM:
	case DXGI_FORMAT_G8R8_G8B8_UNORM:
		return 32;

		//case DXGI_FORMAT_UNKNOWN:
	default:
		_ASSERT(0);
		return 0;
	}
}

DXTEXTURE2D *LoadTexture2D(DXDEVICE *device,
	UINT width,UINT height,UINT layers,UINT mips,DXGI_FORMAT format,const int *poses)
{
	DXDESCTEX2D td;
	td.Width = width;
	td.Height = height;
	td.MipLevels = mips;
	td.ArraySize = layers;
	td.Format = format;
	td.SampleDesc.Count = 1;
	td.SampleDesc.Quality = 0;
	td.Usage = (DXUSAGE)D3D10_USAGE_DEFAULT;
	td.BindFlags = 0;
	td.CPUAccessFlags = 0;
	td.MiscFlags = 0;

	DXSUBRESDATA *sd = new DXSUBRESDATA[mips * layers];
	for(UINT i = 0;i < mips * layers;i++)
	{
		DWORD len,pitch1,pitch2;
		sd[i].pSysMem = GetFromFile2(poses[i],&len,&pitch1,&pitch2);
		sd[i].SysMemPitch = pitch1;
		sd[i].SysMemSlicePitch = pitch2;

		if (!pitch1)
		{
			int w = width >> i;
			sd[i].SysMemPitch = (w < 4 ? 4 : w) * SizeOfFormatElement(format) / 8; // close to right expression
		}

		//_ASSERT(sd[i].SysMemPitch != sd[i].SysMemSlicePitch);
	}

	DXTEXTURE2D *texture;
	HRESULT hr = device->CreateTexture2D(&td,sd,&texture);
	_ASSERT(SUCCEEDED(hr));

	delete[]sd;

	return texture;
}

DXTEXTURE3D *LoadTexture3D(DXDEVICE *device,
	UINT width,UINT height,UINT depth,UINT mips,DXGI_FORMAT format,const int *poses)
{
	DXDESCTEX3D td;
	td.Width = width;
	td.Height = height;
	td.Depth = depth;
	td.MipLevels = mips;
	td.Format = format;
	td.Usage = (DXUSAGE)D3D10_USAGE_DEFAULT;
	td.BindFlags = 0;
	td.CPUAccessFlags = 0;
	td.MiscFlags = 0;

	DXSUBRESDATA *sd = new DXSUBRESDATA[mips];
	for(UINT i = 0;i < mips;i++)
	{
		DWORD len,pitch1,pitch2;
		sd[i].pSysMem = GetFromFile2(poses[i],&len,&pitch1,&pitch2);
		sd[i].SysMemPitch = pitch1;
		sd[i].SysMemSlicePitch = pitch2;

		if (!pitch1)
		{
			int w = width >> i;
			sd[i].SysMemPitch = (w < 4 ? 4 : w) * SizeOfFormatElement(format) / 8; // close to right expression
		}

		if (!pitch2)
		{
			sd[i].SysMemSlicePitch = sd[i].SysMemPitch * height;
		}

		//_ASSERT(sd[i].SysMemPitch != sd[i].SysMemSlicePitch);
	}

	DXTEXTURE3D *texture;
	HRESULT hr = device->CreateTexture3D(&td,sd,&texture);
	_ASSERT(SUCCEEDED(hr));

	delete[]sd;

	return texture;
}

DWORD LoadFromFile2(int pos,unsigned char *data)
{
	DWORD len;
	char *pData = GetFromFile2(pos,&len);
	memcpy(data,pData,len);
	return len;
}

/*void MapTexture2D(ID3D10Texture2D *buffer,int subres,int pos)
{
	D3D10_MAPPED_TEXTURE2D bufdata;
	HRESULT hr = buffer->Map(subres,D3D10_MAP_WRITE_DISCARD,0,&bufdata);
	_ASSERT(SUCCEEDED(hr));
	LoadFromFile2(pos,(unsigned char *)bufdata.pData);
	buffer->Unmap(subres);
}

void MapTexture1D(ID3D10Texture1D *buffer,int subres,int pos)
{
	unsigned char *bufdata;
	HRESULT hr = buffer-> Map(subres,D3D10_MAP_WRITE_DISCARD,0,(void **)&bufdata);
	_ASSERT(SUCCEEDED(hr));
	LoadFromFile2(pos,bufdata);
	buffer->Unmap(subres);
}*/

void MapBuffer(ID3D10Buffer *buffer,int pos,ID3D10Device *)
{
	unsigned char *bufdata;
	HRESULT hr = buffer->Map(D3D10_MAP_WRITE_DISCARD,0,(void **)&bufdata);
	_ASSERT(SUCCEEDED(hr));
	LoadFromFile2(pos,bufdata);
	buffer->Unmap();
}

void MapBuffer(ID3D11Buffer *buffer,int pos,ID3D11DeviceContext *ctx)
{
	D3D11_MAPPED_SUBRESOURCE subres;
	HRESULT hr = ctx->Map(buffer,0,D3D11_MAP_WRITE_DISCARD,0,&subres);
	_ASSERT(SUCCEEDED(hr));
	LoadFromFile2(pos,(unsigned char *)subres.pData);
	ctx->Unmap(buffer,0);
}

DXBUFFER *LoadBuffer(DXDEVICE *device,int pos,UINT bindFlags)
{
	DWORD len,pitch;
	DXSUBRESDATA sd;
	sd.pSysMem = GetFromFile2(pos,&len,&pitch);
	if (!pitch) pitch = len;
	sd.SysMemPitch = pitch;

	DXDESCBUFFER bd;
	bd.Usage = (DXUSAGE)D3D10_USAGE_DYNAMIC;
	bd.ByteWidth = len;
	bd.BindFlags = bindFlags;
	bd.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
	bd.MiscFlags = 0;

	DXBUFFER *buf;
	HRESULT hr = device->CreateBuffer(&bd, &sd, &buf);
	_ASSERT(SUCCEEDED(hr));

	return buf;
}
