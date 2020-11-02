#ifndef TRACEDISPLAY_H__
#define TRACEDISPLAY_H__
/**************************************************/
/***
 * Copyright (C) 2005
 * E. Scott Larsen and Jeff Kiel 
 * Copyright NVIDIA Corporation 2005
 * TO THE MAXIMUM EXTENT PERMITTED BY APPLICABLE LAW, THIS SOFTWARE IS PROVIDED *AS IS* AND NVIDIA AND
 * AND ITS SUPPLIERS DISCLAIM ALL WARRANTIES, EITHER EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO,
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  IN NO EVENT SHALL NVIDIA
 * OR ITS SUPPLIERS BE LIABLE FOR ANY SPECIAL, INCIDENTAL, INDIRECT, OR CONSEQUENTIAL DAMAGES WHATSOEVER
 * INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF BUSINESS PROFITS, BUSINESS INTERRUPTION, LOSS OF
 * BUSINESS INFORMATION, OR ANY OTHER PECUNIARY LOSS) ARISING OUT OF THE USE OF OR INABILITY TO USE THIS
 * SOFTWARE, EVEN IF NVIDIA HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 **/

#include "..\NVTraceDisplay\trace.h"

/**************************************************/
/// CTraceDisplay: 
/**
 * \class CTraceDisplay tracedisplay.h "tracedisplay.h"
 **
 * Measuring the current window	as [0,1] in	width and height, with 0,0 being the bottom	left,
 * CTraceDisplay displays a CTrace<float> in a subwindow positioned at _left,_bottom
 * with _width and _height (both [0,1]).
 **
 * You can set the background of the subwindow and also for each trace displayed (including alpha if you'd like).
 **/
/**************************************************/
class CTraceDisplay {
public:
	/// Different ways to display the Trace
	enum DISPLAY {
		LINE_STREAM,
		LINE_WRAP,
		NEEDLE,
		BAR,
	};

public:/// The useful interface
	CTraceDisplay(const float left = 0.0f, const float bottom = 0.0f, const float width = 0.5f, const float height = 0.33f)
		:_left(left), _bottom(bottom), _width(width), _height(height), _draw_back(true), _draw_text(true), _bDisplayMinMax(true) {BackgroundColor(0.1f);}
	virtual ~CTraceDisplay() {}

	virtual void Insert(::CTrace<float> *const t, const float r, const float g, const float b, const float a = 1.0f)
	{_entries.push_back(Entry(t, r, g, b, a));}
	void Insert(::CTrace<float> *const t, const float s) {Insert(t, s, s, s, 1.0f);}

	void Display(const DISPLAY &d)
	{
		Push();

		Background();
		switch (d){
		case LINE_STREAM: DisplayLineStream();break;
		case LINE_WRAP: DisplayLineWrap();break;
		case NEEDLE: DisplayNeedle();break;
		case BAR: DisplayBar();break;
		}

		Pop();
	}

	/// generic interface functions
	void BackgroundColor(const float r, const float g, const float b, const float a = 1.0f)
	{_background_color[0] = r; _background_color[1] = g; _background_color[2] = b; _background_color[3] = a;}
	void BackgroundColor(const float s)
	{BackgroundColor(s, s, s, 1.0f);}

	void Position(const float left, const float bottom, const float width, const float height)
	{_left = left; _bottom = bottom; _width = width; _height = height;}
	void Position(const float left, const float bottom)
	{_left = left; _bottom = bottom;}

	void DrawBack(const bool b) {_draw_back = b;}
	bool DrawBack() const {return _draw_back;}
	void DrawText(const bool b) {_draw_text=b;}
	bool DrawText() const {return _draw_text;}
	void DrawRange(const bool b) {_bDisplayMinMax = b;}
	bool DrawRange() const {return _bDisplayMinMax;}

	float GetLeft		() const { return _left; };
	float GetBottom	() const { return _bottom; };

protected:
	/// Enables multiple Traces to be displayed uniquely
	class Entry {
	public:
		Entry(::CTrace<float> *const t, const float r_, const float g_, const float b_, const float a_)
			:trace(t),r(r_),g(g_),b(b_),a(a_) {}

		::CTrace<float> *trace;
		float r,g,b,a;
	};

	typedef std::vector<Entry> Entries;

protected:
	/// virtuals for D3D/OpenGL
	virtual void Push() = 0;
	virtual void Pop() = 0;
	virtual void Background() = 0;
	virtual void Text(const Entries::iterator& it,const float count) = 0;
	virtual void DisplayLineStream() = 0;
	virtual void DisplayLineWrap() = 0;
	virtual void DisplayNeedle() = 0;
	virtual void DisplayBar() = 0;

protected:/// internal stuff
	Entries _entries;
	float _left;
	float _bottom;
	float _width;
	float _height;
	float _background_color[4];
	bool _draw_back;
	bool _draw_text;
	bool _bDisplayMinMax;

private:
	CTraceDisplay(const CTraceDisplay &that);
	const CTraceDisplay &operator=(const CTraceDisplay &that);
};

class Vert {
public:
	Vert() : x(0),y(0),z(1),w(1.0f){}

	float x,y,z,w;
	DWORD color;
};

class CD3DTraceDisplay : public CTraceDisplay {
public:
	CD3DTraceDisplay(const float left = 0.0f, const float top = 0.0f, const float width = 0.5f, const float height = 0.33f) : 
	  _pd3dDevice(NULL), _pd3dFont(NULL), _verts(NULL), _VertFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE), _nverts(4),
		  CTraceDisplay(left, top + height, width, height)
	  {_verts = new Vert[_nverts]; _bottom = top + height;}
	  virtual ~CD3DTraceDisplay() 
	  {Release(); delete [] _verts;}

	  virtual void Insert(::CTrace<float> *const t, const float r, const float g, const float b, const float a = 1.0f)
	  {
		  CTraceDisplay::Insert(t, r, g, b, a);
		  if (t->capacity() > _nverts)
		  {
			  _nverts = (unsigned int) t->capacity();
			  delete [] _verts;
			  _verts = new Vert[_nverts];
		  }
	  }

	  void Release() 
	  {_pd3dDevice = NULL; if(_pd3dFont)	{_pd3dFont->Release(); _pd3dFont = NULL;}}
	  void SetDevice(IDirect3DDevice9 *pd3dDevice, LPD3DXFONT pd3dFont)
	  {Release(); _pd3dDevice = pd3dDevice;  if((_pd3dFont = pd3dFont) != NULL) _pd3dFont->AddRef();}

protected:
	virtual void Push() 
	{
		_pd3dDevice->GetRenderState(D3DRS_ZFUNC, &_dwZFunc);
		_pd3dDevice->GetRenderState(D3DRS_ALPHABLENDENABLE, &_dwAlphaBlendEnable);
		_pd3dDevice->GetRenderState(D3DRS_SRCBLEND, &_dwSrcBlend);
		_pd3dDevice->GetRenderState(D3DRS_DESTBLEND, &_dwDstBlend);

		_pd3dDevice->SetVertexShader(NULL);
		_pd3dDevice->SetPixelShader(NULL);
		_pd3dDevice->SetFVF(_VertFVF);
		_pd3dDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_ALWAYS);
		_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		_pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		_pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	}
	virtual void Pop() 
	{
		_pd3dDevice->SetRenderState(D3DRS_ZFUNC, _dwZFunc);
		_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, _dwAlphaBlendEnable);
		_pd3dDevice->SetRenderState(D3DRS_SRCBLEND, _dwSrcBlend);
		_pd3dDevice->SetRenderState(D3DRS_DESTBLEND, _dwDstBlend);
	}
	virtual void Background();
	virtual void Text(const Entries::iterator& it,const float count);
	virtual void DisplayLineStream();
	virtual void DisplayLineWrap();
	virtual void DisplayNeedle();
	virtual void DisplayBar();

protected:
	IDirect3DDevice9 *_pd3dDevice;
	LPD3DXFONT _pd3dFont;
	DWORD _dwZFunc, _dwAlphaBlendEnable, _dwSrcBlend, _dwDstBlend;

	const DWORD _VertFVF;
	Vert *_verts;
	unsigned int _nverts;
};

#endif
