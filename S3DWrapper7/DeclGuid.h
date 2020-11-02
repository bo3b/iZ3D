/* IZ3D_FILE: $Id$ 
*
* Project : iZ3D Stereo Driver
* Copyright (C) iZ3D Inc. 2002 - 2009
*
* $Author$
* $Revision$
* $Date$
* $LastChangedBy$
* $URL$
*/
// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

/*
* Interface IID's
*/
interface DECLSPEC_UUID("3BBA0080-2421-11CF-A31A-00AA00B93356") IDirect3D;
interface DECLSPEC_UUID("6aae1ec1-662a-11d0-889d-00aa00bbb76a") IDirect3D2;
interface DECLSPEC_UUID("bb223240-e72b-11d0-a9b4-00aa00c0993e") IDirect3D3;
interface DECLSPEC_UUID("f5049e77-4861-11d2-a407-00a0c90629a8") IDirect3D7;

interface DECLSPEC_UUID("64108800-957d-11d0-89ab-00a0c9054129") IDirect3DDevice;
interface DECLSPEC_UUID("93281501-8cf8-11d0-89ab-00a0c9054129") IDirect3DDevice2;
interface DECLSPEC_UUID("b0ab3b60-33d7-11d1-a981-00c04fd7b174") IDirect3DDevice3;
interface DECLSPEC_UUID("f5049e79-4861-11d2-a407-00a0c90629a8") IDirect3DDevice7;
interface DECLSPEC_UUID("2CDCD9E0-25A0-11CF-A31A-00AA00B93356") IDirect3DTexture;
interface DECLSPEC_UUID("93281502-8cf8-11d0-89ab-00a0c9054129") IDirect3DTexture2;
interface DECLSPEC_UUID("4417C142-33AD-11CF-816F-0000C020156E") IDirect3DLight;
interface DECLSPEC_UUID("4417C144-33AD-11CF-11d1-0000C020156E") IDirect3DMaterial;
interface DECLSPEC_UUID("93281503-8cf8-11d0-89ab-00a0c9054129") IDirect3DMaterial2;
interface DECLSPEC_UUID("ca9c46f4-d3c5-11d1-b75a-00600852b312") IDirect3DMaterial3;
interface DECLSPEC_UUID("4417C145-33AD-11CF-816F-0000C020156E") IDirect3DExecuteBuffer;
interface DECLSPEC_UUID("4417C146-33AD-11CF-816F-0000C020156E") IDirect3DViewport;
interface DECLSPEC_UUID("93281500-8cf8-11d0-89ab-00a0c9054129") IDirect3DViewport2;
interface DECLSPEC_UUID("b0ab3b61-33d7-11d1-a981-00c04fd7b174") IDirect3DViewport3;
interface DECLSPEC_UUID("7a503555-4a83-11d1-a5db-00a0c90367f8") IDirect3DVertexBuffer;
interface DECLSPEC_UUID("f5049e7d-4861-11d2-a407-00a0c90629a8") IDirect3DVertexBuffer7;

interface DECLSPEC_UUID("6C14DB80-A733-11CE-A521-0020AF0BE560") IDirectDraw;
interface DECLSPEC_UUID("B3A6F3E0-2B43-11CF-A2DE-00AA00B93356") IDirectDraw2;
interface DECLSPEC_UUID("9c59509a-39bd-11d1-8c4a-00c04fd930c5") IDirectDraw4;
interface DECLSPEC_UUID("15e65ec0-3b9c-11d2-b92f-00609797ea5b") IDirectDraw7;
interface DECLSPEC_UUID("6C14DB81-A733-11CE-A521-0020AF0BE560") IDirectDrawSurface;
interface DECLSPEC_UUID("57805885-6eec-11cf-9441-a82303c10e27") IDirectDrawSurface2;
interface DECLSPEC_UUID("DA044E00-69B2-11D0-A1D5-00AA00B8DFBB") IDirectDrawSurface3;
interface DECLSPEC_UUID("0B2B8630-AD35-11D0-8EA6-00609797EA5B") IDirectDrawSurface4;
interface DECLSPEC_UUID("06675a80-3b9b-11d2-b92f-00609797ea5b") IDirectDrawSurface7;
interface DECLSPEC_UUID("6C14DB84-A733-11CE-A521-0020AF0BE560") IDirectDrawPalette;
interface DECLSPEC_UUID("6C14DB85-A733-11CE-A521-0020AF0BE560") IDirectDrawClipper;
interface DECLSPEC_UUID("4B9F0EE0-0D7E-11D0-9B06-00A0C903A3B8") IDirectDrawColorControl;
interface DECLSPEC_UUID("69C11C3E-B46B-11D1-AD7A-00C04FC29B4E") IDirectDrawGammaControl;
