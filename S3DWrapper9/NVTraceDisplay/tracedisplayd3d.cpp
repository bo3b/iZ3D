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
 
#include "stdafx.h"
#include <d3dx9.h>
#include <d3dx9core.h>
#include <stdio.h>
#include <TCHAR.h>
#include <math.h>

#include "tracedisplayd3d.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

inline DWORD FloatToColor(float* c)
{
	return D3DCOLOR_COLORVALUE(c[0], c[1], c[2], c[3]);
}

#define TEXT_HEIGHT 15.0f
/**************************************************/
void CD3DTraceDisplay::Background()
{
	if (_draw_back)
	{
		D3DVIEWPORT9 view;
		_pd3dDevice->GetViewport(&view);
		DWORD color = FloatToColor(_background_color);

		_verts[0].x = _left * view.Width;
		_verts[0].y = (_bottom - _height) * view.Height;
		_verts[0].color = color;
		
		_verts[1].x = (_left + _width) * view.Width;
		_verts[1].y = (_bottom - _height) * view.Height;
		_verts[1].color = color;
		
		_verts[2].x = _left * view.Width;
		_verts[2].y = _bottom * view.Height;
		_verts[2].color = color;
		
		_verts[3].x = (_left + _width) * view.Width;
		_verts[3].y = _bottom * view.Height;
		_verts[3].color = color;

		HRESULT hRes = _pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, &_verts[0], sizeof(Vert));

		_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
		_pd3dDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
	}
}

/**************************************************/
void CD3DTraceDisplay::Text(const Entries::iterator &it, const float count)
{
	if (_draw_text)
	{
		RECT position;
		TCHAR msg[256];
		D3DVIEWPORT9 view;
		float delta = float(_width) / float(_entries.size());
		DWORD color = D3DCOLOR_COLORVALUE(it->r, it->g, it->b, it->a);
		
		_pd3dDevice->GetViewport(&view);
		
		if(_bDisplayMinMax)
			_stprintf(msg,_T("%S: %.2f [%.2f %.2f]"), it->trace->name().c_str(), it->trace->last(), it->trace->minVal(), it->trace->maxVal());
		else
			_stprintf(msg,_T("%S: %.2f"), it->trace->name().c_str(), it->trace->last(), it->trace->minVal(), it->trace->maxVal());
			
		SetRect(&position, (LONG) ((_left + (count * delta)) * view.Width), (LONG) (_bottom * view.Height) - (LONG) TEXT_HEIGHT, 0, 0);
		_pd3dFont->DrawText(NULL, msg, -1, &position, DT_NOCLIP, color);
	}
}

/**************************************************/
void CD3DTraceDisplay::DisplayLineStream()
{
	float count = 0;
	D3DVIEWPORT9 view;
	
	_pd3dDevice->GetViewport(&view);

	float fLocalBottom = _bottom - (TEXT_HEIGHT / (float) view.Height);
	float fLocalHeight = _height - (TEXT_HEIGHT / (float) view.Height);
	
	for(Entries::iterator it = _entries.begin(); it != _entries.end(); ++it)
	{
		unsigned int v = 0;
		DWORD color = D3DCOLOR_COLORVALUE(it->r, it->g, it->b, it->a);
		float bounds = it->trace->maxVal() - it->trace->minVal();
		for (size_t i = 0; i < it->trace->size(); ++i)
		{
			_verts[v].x = (_left + (_width * (float(i) / float(it->trace->capacity() - 1)))) * view.Width;
			_verts[v].y = (fLocalBottom - (fLocalHeight * (((*(it->trace))(i) - it->trace->minVal()) / bounds))) * view.Height;
			_verts[v].color = color;
			++v;
		}
		
		_pd3dDevice->DrawPrimitiveUP(D3DPT_LINESTRIP, v-1, &_verts[0], sizeof(Vert));
		Text(it, count);
		++count;
	}
}

/**************************************************/
void CD3DTraceDisplay::DisplayLineWrap()
{
	float count = 0;
	D3DVIEWPORT9 view;
	
	_pd3dDevice->GetViewport(&view);

	float fLocalBottom = _bottom - (TEXT_HEIGHT / (float) view.Height);
	float fLocalHeight = _height - (TEXT_HEIGHT / (float) view.Height);

	for(Entries::iterator it = _entries.begin(); it != _entries.end(); ++it)
	{
		unsigned int v = 0;
		DWORD color = D3DCOLOR_COLORVALUE(it->r, it->g, it->b, it->a);
		float bounds = it->trace->maxVal() - it->trace->minVal();
		for (size_t i=0;i<it->trace->size();++i)
		{
			_verts[v].x = (_left + (_width * (float(i) / float(it->trace->capacity() - 1)))) * view.Width;
			_verts[v].y = (fLocalBottom - (fLocalHeight * ((*(it->trace))[i] - it->trace->minVal()) / bounds)) * view.Height;
			_verts[v].color = color;
			++v;
		}
		
		_pd3dDevice->DrawPrimitiveUP(D3DPT_LINESTRIP, v-1 , &_verts[0], sizeof(Vert));
		Text(it, count);
		++count;
	}
}

/**************************************************/
void CD3DTraceDisplay::DisplayNeedle()
{
	float count = 0;

	for(Entries::iterator it = _entries.begin(); it != _entries.end(); ++it)
	{
		DWORD color = D3DCOLOR_COLORVALUE(it->r, it->g, it->b, it->a);

		Text(it, count);
		++count;
	}
}

/**************************************************/
void CD3DTraceDisplay::DisplayBar()
{
	D3DVIEWPORT9 view;
	float count = 0;
	float max = (float)_entries.size();
	
	_pd3dDevice->GetViewport(&view);

	float fLocalBottom = _bottom - (TEXT_HEIGHT / (float) view.Height);
	float fLocalHeight = _height - (TEXT_HEIGHT / (float) view.Height);

	for(Entries::iterator it = _entries.begin(); it != _entries.end(); ++it)
	{
		DWORD color = D3DCOLOR_COLORVALUE(it->r, it->g, it->b, it->a);
		float bounds = it->trace->maxVal() - it->trace->minVal();
		_verts[0].x = (_left + (count * (_width / max))) * view.Width;
		_verts[0].y = (fLocalBottom - it->trace->last() * (fLocalHeight / bounds)) * view.Height;
		_verts[0].color = color;
		
		_verts[1].x = (_left + (count + 1.0f) * (_width / max)) * view.Width;
		_verts[1].y = (fLocalBottom - it->trace->last() * (fLocalHeight / bounds)) * view.Height;
		_verts[1].color = color;
		_verts[2].x = _left+count*_width/max;
		_verts[2].y = fLocalBottom;
		_verts[2].x *= view.Width;
		_verts[2].y *= view.Height;
		_verts[2].color = color;
		_verts[3].x = _left+(1.0f+count)*_width/max;
		_verts[3].y = fLocalBottom;
		_verts[3].x *= view.Width;
		_verts[3].y *= view.Height;
		_verts[3].color = color;
		Text(it,count);
		_pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,2,&_verts[0],sizeof(Vert));
		++count;
	}
}
