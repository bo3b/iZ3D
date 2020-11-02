
HRESULT StateBlock_Apply(IDirect3DStateBlock9 *sblock);
HRESULT StateBlock_Capture(IDirect3DStateBlock9 *sblock);
HRESULT GetSurfaceLevel(IDirect3DTexture9 *tex,UINT level,IDirect3DSurface9 **surf);
HRESULT GetCubeMapSurface(IDirect3DCubeTexture9 *cube,D3DCUBEMAP_FACES face,UINT level,IDirect3DSurface9 **surf);
HRESULT BeginDeviceState();
void SetProgress(int value);
void SetEID(UINT64 eid);

#define texture_0 0

@@DECLARATION@@
