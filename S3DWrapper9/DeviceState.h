#pragma once

#ifndef FINAL_RELEASE

#include <map>

template<class T, int multiplier>
class block_array
{
public:
	explicit block_array() { }
	~block_array() { clear(); }

	void add(UINT start,const T *data,UINT count);

	UINT size() const { return (UINT)m_blocks.size(); }

	void get(int i,UINT &start,const T *&data,UINT &count) const
	{
		const block &b = m_blocks[i];
		start = b.start;
		count = b.count;
		data = b.data.data();
	}

	void clear()
	{
		m_blocks.clear();
	}

	explicit block_array(const block_array<T,multiplier> &ba) { *this = ba; }
	void operator =(const block_array<T,multiplier> &ba)
	{
		clear();
		for(unsigned i = 0;i < ba.m_blocks.size();i++)
		{
			const block &b = ba.m_blocks[i];
			add(b.start,b.data.data(),b.count);
		}
	}

	block_array(block_array<T,multiplier>&& other)
	{
		m_blocks = std::move(other.m_blocks);
	}
	
	block_array<T,multiplier>& operator=(block_array<T,multiplier>&& other)
	{
		m_blocks.clear();
		m_blocks = std::move(other.m_blocks);
		return *this;
	}

protected:
	class block
	{
	public:
		block(UINT s,UINT c,const T *d)
		{
			start = s;
			count = c;
			data.resize(count * multiplier);
			memcpy(data.data(),d,sizeof(T) * count * multiplier);
		}

		block(block&& other)
			: start(other.start)
			, count(other.count)
		{
			data = std::move(other.data);
			other.count = 0;
		}

		block& operator=(block&& other)
		{
			data.clear();
			start = other.start;
			count = other.count;
			data = std::move(other.data);
			other.count = 0;
			return *this;
		}

		UINT start;
		UINT count;
		std::vector<T> data;

		void extend(UINT s,UINT c,const T *d)
		{
			_ASSERT(s >= start);
			_ASSERT(s <= start + count);
			UINT newCount = c + (s - start);
			if (newCount > count)
			{
				data.resize(newCount * multiplier);
				count = newCount;
			}
			memcpy(&data[(s - start) * multiplier],d,sizeof(T) * c * multiplier);
		}

		void merge(const block& bi)
		{
			if (bi.start >= start)
			{
				_ASSERT((start + count) >= bi.start);
				UINT newCount = bi.count + (bi.start - start);
				if (newCount > count)
					data.resize(newCount * multiplier);
				memcpy(&data[count * multiplier],&bi.data[start + count - bi.start],
					sizeof(T) * (newCount - count) * multiplier);
				count = newCount;
			}
			else
			{
				_ASSERT((bi.start + bi.count) >= start);
				std::vector<T> newData;
				UINT diff = start - bi.start;
				UINT newCount = count + diff;
				newData.resize(newCount);
				memcpy(newData.data(),bi.data.data(),sizeof(T) * diff * multiplier);
				memcpy(&newData[diff * multiplier],data.data(),sizeof(T) * count * multiplier);
				data.swap(newData);
				start = bi.start;
				count = newCount;
			}
		}

		bool isOverlap(const block& bi)
		{
			return start <= bi.start  && start + count >= bi.start + bi.count;
		}

		bool isIntersect(const block& bi)
		{
			UINT end = start + count;
			UINT biEnd = bi.start + bi.count;
			return start <= bi.start  && end >= bi.start ||
				bi.start <= start  && biEnd >= start;
		}
	};

protected:
	std::vector<block> m_blocks;
};

struct float4
{
	float f[4];
	operator const float *() { return f; }
	void operator =(const float *src) { memcpy(f,src,sizeof(float)*4); }
};

template<typename T>
class CDSVariable
{
public:
	CDSVariable() : m_bHasValue(false) {}
	operator T& () { return m_T; };
	operator const T&() const {	return m_T;	}
	template <typename Q>
	operator Q*() {	return (Q*)m_T;	}
	template <typename Q>
	operator const Q*() const {	return (Q*)m_T;	}

	T& operator->()	{	return m_T;	}
	const T& operator->() const	{	return m_T;	}
	T* operator&()	{	return &m_T;	}
	const T* operator&() const	{	return &m_T;	}
	bool HasValue() {	return m_bHasValue;	}	
	void clear() {	m_bHasValue = false; m_T = T();	}	
	CDSVariable& operator=(const T& src)
	{		
		m_T = src;
		m_bHasValue = true;
		return *this;
	}	
	void operator =(const CDSVariable &other)
	{
		m_bHasValue = other.m_bHasValue;
		if (m_bHasValue)
			m_T = other.m_T;
	}
	CDSVariable& operator=(CDSVariable&& other)
	{
		m_bHasValue = other.m_bHasValue;
		other.m_bHasValue = false;
		if (m_bHasValue)
			m_T = std::move(other.m_T);
		return *this;
	}
	void Capture(const CDSVariable& deviceState)
	{
		if (m_bHasValue && deviceState.m_bHasValue) 
			*this = deviceState.m_T;
	}
	void Apply(const CDSVariable& other)
	{
		if (other.m_bHasValue)
			*this = other.m_T;
	}

	T		m_T;

private:
	bool	m_bHasValue;
};

class CDeviceState
{
public:
	explicit CDeviceState();
	~CDeviceState();

	void Dump(BOOL bForce = FALSE);
	void DumpResources();
	void DumpBaseSurfaces(IDirect3DDevice9 *dev);

	void SetRenderState(D3DRENDERSTATETYPE State, DWORD Value);
	void SetMaterial(CONST D3DMATERIAL9* pMaterial);
	void SetTexture(DWORD Stage, IDirect3DBaseTexture9* pTexture);
	void SetTextureStageState(DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD Value);
	void SetTransform(D3DTRANSFORMSTATETYPE State, CONST D3DMATRIX* pMatrix);
	void SetStreamSource(UINT StreamNumber, IDirect3DVertexBuffer9* pStreamData, UINT OffsetInBytes, UINT Stride);
	void SetFVF(DWORD FVF);
	void SetVertexShader(IDirect3DVertexShader9* pShader);
	void SetPixelShader(IDirect3DPixelShader9* pShader);
	void SetIndices(IDirect3DIndexBuffer9* pIndexData);
	void SetVertexDeclaration(IDirect3DVertexDeclaration9* pDecl);
	void SetSamplerState(DWORD Sampler, D3DSAMPLERSTATETYPE Type, DWORD Value);
	void LightEnable(DWORD Index, BOOL Enable);
	void SetLight(DWORD Index, CONST D3DLIGHT9* pLight);
	void SetViewport(CONST D3DVIEWPORT9* pViewport);
	void SetScissorRect(CONST RECT* pRect);
	void SetStreamSourceFreq(UINT StreamNumber, UINT Divider);
	void SetClipPlane(DWORD Index, CONST float* pPlane);
	void SetCurrentTexturePalette(UINT PaletteNumber);
	void SetNPatchMode(float nSegments);

	void SetPixelShaderConstantF(UINT StartRegister, CONST float* pConstantData, UINT Vector4fCount);
	void SetPixelShaderConstantI(UINT StartRegister, CONST int* pConstantData, UINT Vector4iCount);
	void SetPixelShaderConstantB(UINT StartRegister, CONST BOOL* pConstantData, UINT BoolCount);
	void SetVertexShaderConstantF(UINT StartRegister, CONST float* pConstantData, UINT Vector4fCount);
	void SetVertexShaderConstantI(UINT StartRegister, CONST int* pConstantData, UINT Vector4iCount);
	void SetVertexShaderConstantB(UINT StartRegister, CONST BOOL* pConstantData, UINT BoolCount);

	/*
		Full methods set:

		+ IDirect3DDevice9::LightEnable 
		+ IDirect3DDevice9::SetClipPlane 
	 	+ IDirect3DDevice9::SetCurrentTexturePalette 
		+ IDirect3DDevice9::SetFVF 
	 	+ IDirect3DDevice9::SetIndices 
		+ IDirect3DDevice9::SetLight 
		+ IDirect3DDevice9::SetMaterial 
		+ IDirect3DDevice9::SetNPatchMode 
		+ IDirect3DDevice9::SetPixelShader 
		+ IDirect3DDevice9::SetPixelShaderConstantB 
		+ IDirect3DDevice9::SetPixelShaderConstantF 
		+ IDirect3DDevice9::SetPixelShaderConstantI 
		+ IDirect3DDevice9::SetRenderState 
		+ IDirect3DDevice9::SetSamplerState 
		+ IDirect3DDevice9::SetScissorRect 
	 	+ IDirect3DDevice9::SetStreamSource 
		+ IDirect3DDevice9::SetStreamSourceFreq 
	 	+ IDirect3DDevice9::SetTexture 
		+ IDirect3DDevice9::SetTextureStageState 
		+ IDirect3DDevice9::SetTransform 
		+ IDirect3DDevice9::SetViewport 
	 	+ IDirect3DDevice9::SetVertexDeclaration 
		+ IDirect3DDevice9::SetVertexShader 
		+ IDirect3DDevice9::SetVertexShaderConstantB 
		+ IDirect3DDevice9::SetVertexShaderConstantF 
		+ IDirect3DDevice9::SetVertexShaderConstantI 
	*/

	explicit CDeviceState(const CDeviceState &state) { *this = state; }
	CDeviceState(CDeviceState&& state) { *this = std::move(state); }

	void operator =(const CDeviceState &state);
	CDeviceState& operator=(CDeviceState&& other);
	void Create( D3DSTATEBLOCKTYPE Type, const CDeviceState &state );
	void Capture( const CDeviceState &state );
	void Apply( const CDeviceState &state );
	void Clear();

protected:
	std::map<D3DRENDERSTATETYPE,DWORD> 						  m_renderState;
	std::map<DWORD,CAdapt<CComPtr<IDirect3DBaseTexture9>>> 	  m_textures;
	std::map<std::pair<DWORD,D3DTEXTURESTAGESTATETYPE>,DWORD> m_textureStageStates;
	std::map<std::pair<DWORD,D3DSAMPLERSTATETYPE>,DWORD> 	  m_samplerStates;
	std::map<D3DTRANSFORMSTATETYPE,D3DMATRIX> 				  m_transforms; // default: identity matrices
	std::map<DWORD,D3DLIGHT9> 								  m_lights; // default: empty
	std::map<DWORD,BOOL> 									  m_lightEnables; // default: empty
	std::map<UINT,UINT> 									  m_freqs; // 16 - 1
	std::map<DWORD,float4> 									  m_clipPlanes; // default: one with zero values

	struct SetStreamSourceData
	{
		IDirect3DVertexBuffer9* pStreamData;
		UINT OffsetInBytes;
		UINT Stride;
	};
	std::map<UINT,SetStreamSourceData>	m_streamSources;

	CDSVariable<UINT>			m_palette; // default: 65535
	CDSVariable<float>			m_nPatchMode;
	CDSVariable<D3DMATERIAL9>	m_material;

	CDSVariable<DWORD> 			m_fvf;

	CDSVariable<D3DVIEWPORT9>	m_viewport; // default: 0, 0, width, height, 0.0, 1.0
	CDSVariable<RECT> 			m_scissorRect; // default: 0, 0, width, height

	CDSVariable<CComPtr<IDirect3DVertexShader9>> 	 	m_pVShader;
	CDSVariable<CComPtr<IDirect3DPixelShader9>> 		m_pPShader;
	CDSVariable<CComPtr<IDirect3DIndexBuffer9>> 		m_pIndeces;
	CDSVariable<CComPtr<IDirect3DVertexDeclaration9>> 	m_pVDecl;

	block_array<float,	4> 				 	m_pshaderConstF;
	block_array<int,	4> 				 	m_pshaderConstI;
	block_array<BOOL,	1> 				 	m_pshaderConstB;
	block_array<float,	4>			 		m_vshaderConstF;
	block_array<int,	4>			 		m_vshaderConstI;
	block_array<BOOL,	1>			 		m_vshaderConstB;
};

#endif // FINAL_RELEASE


