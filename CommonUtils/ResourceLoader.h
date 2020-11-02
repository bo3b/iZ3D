#ifndef RESOURCELOADER_H
#define RESOURCELOADER_H

//////////////////////////////////////////////////////////////////////////

class ResourceLoader
{
public:
				ResourceLoader	();
	bool		Load			(HMODULE hModule, LPCTSTR lpName, LPCTSTR lpType);
	void*		GetData()		const { return m_pMem; };
	DWORD		GetSize()		const { return m_Size; };
private:
	void*		m_pMem;
	DWORD		m_Size;
};

//////////////////////////////////////////////////////////////////////////

class PNGResource: public ResourceLoader
{
public:
				PNGResource		( HMODULE hModule_, LPCTSTR lpName_ );
				~PNGResource	();

	BYTE*		GetTexData		() const { return m_pTexData; };
	UINT		GetTexWidth		() const { return m_nTexWidth; };
	UINT		GetTexHeight	() const { return m_nTexHeight; };
	UINT		GetBitDepth		() const { return m_nBitDepth; };
	UINT		GetRowBytes		() const { return m_nRowBytes; };
protected:
private:
	UINT		m_nImageId;

	UINT		m_nTexWidth;
	UINT		m_nTexHeight;
	UINT		m_nBitDepth;
	UINT		m_nRowBytes;
	BYTE*		m_pTexData;
};


//////////////////////////////////////////////////////////////////////////

#endif//RESOURCELOADER_H