/* 
* Project : iZ3D Stereo Driver
* Copyright (C) iZ3D Inc. 2002 - 2010
*/

#pragma once

namespace IDirect3D9MethodNames
{
	enum Names
	{
		// IUnknown methods
		_QueryInterface_ = 0,
		_AddRef_ = 1,
		_Release_ = 2,

		// IDirect3D9 methods
		_RegisterSoftwareDevice_ = 3,
		_GetAdapterCount_ = 4,
		_GetAdapterIdentifier_ = 5,
		_GetAdapterModeCount_ = 6,
		_EnumAdapterModes_ = 7,
		_GetAdapterDisplayMode_ = 8,
		_CheckDeviceType_ = 9,
		_CheckDeviceFormat_ = 10,
		_CheckDeviceMultiSampleType_ = 11,
		_CheckDepthStencilMatch_ = 12,
		_CheckDeviceFormatConversion_ = 13,
		_GetDeviceCaps_ = 14,
		_GetAdapterMonitor_ = 15,
		_CreateDevice_ = 16,
		_MaxMethods_ = 17,

		// IDirect3D9Ex methods
		_GetAdapterModeCountEx_ = 17,
		_EnumAdapterModesEx_ = 18,
		_GetAdapterDisplayModeEx_ = 19,
		_CreateDeviceEx_ = 20,
		_GetAdapterLUID_ = 21,

		_MaxMethodsEx_
	};
}

namespace IDirect3DSwapChain9MethodNames
{
	enum Names
	{
		// IUnknown methods
		_QueryInterface_ = 0,
		_AddRef_ = 1,
		_Release_ = 2,

		// IDirect3DSwapChain9 methods
		_Present_ = 3,
		_GetFrontBufferData_ = 4,
		_GetBackBuffer_ = 5,
		_GetRasterStatus_ = 6,
		_GetDisplayMode_ = 7,
		_GetDevice_ = 8,
		_GetPresentParameters_ = 9,
		_MaxMethods_ = 10,
		
		// IDirect3DSwapChain9Ex methods
		_GetLastPresentCount_ = 10,
		_GetPresentStats_ = 11,
		_GetDisplayModeEx_ = 12,

		_MaxMethodsEx_
	};
}

namespace IDirect3DDevice9MethodNames
{
	enum Names
	{
		// IUnknown methods
		_QueryInterface_ = 0,
		_AddRef_ = 1,
		_Release_ = 2,

		// IDirect3DDevice9 methods
		_TestCooperativeLevel_ = 3,
		_GetAvailableTextureMem_ = 4,
		_EvictManagedResources_ = 5,
		_GetDirect3D_ = 6,
		_GetDeviceCaps_ = 7,
		_GetDisplayMode_ = 8,
		_GetCreationParameters_ = 9,
		_SetCursorProperties_ = 10,
		_SetCursorPosition_ = 11,
		_ShowCursor_ = 12,
		_CreateAdditionalSwapChain_ = 13,
		_GetSwapChain_ = 14,
		_GetNumberOfSwapChains_ = 15,
		_Reset_ = 16,
		_Present_ = 17,
		_GetBackBuffer_ = 18,
		_GetRasterStatus_ = 19,
		_SetDialogBoxMode_ = 20,
		_SetGammaRamp_ = 21,
		_GetGammaRamp_ = 22,
		_CreateTexture_ = 23,
		_CreateVolumeTexture_ = 24,
		_CreateCubeTexture_ = 25,
		_CreateVertexBuffer_ = 26,
		_CreateIndexBuffer_ = 27,
		_CreateRenderTarget_ = 28,
		_CreateDepthStencilSurface_ = 29,
		_UpdateSurface_ = 30,
		_UpdateTexture_ = 31,
		_GetRenderTargetData_ = 32,
		_GetFrontBufferData_ = 33,
		_StretchRect_ = 34,
		_ColorFill_ = 35,
		_CreateOffscreenPlainSurface_ = 36,
		_SetRenderTarget_ = 37,
		_GetRenderTarget_ = 38,
		_SetDepthStencilSurface_ = 39,
		_GetDepthStencilSurface_ = 40,
		_BeginScene_ = 41,
		_EndScene_ = 42,
		_Clear_ = 43,
		_SetTransform_ = 44,
		_GetTransform_ = 45,
		_MultiplyTransform_ = 46,
		_SetViewport_ = 47,
		_GetViewport_ = 48,
		_SetMaterial_ = 49,
		_GetMaterial_ = 50,
		_SetLight_ = 51,
		_GetLight_ = 52,
		_LightEnable_ = 53,
		_GetLightEnable_ = 54,
		_SetClipPlane_ = 55,
		_GetClipPlane_ = 56,
		_SetRenderState_ = 57,
		_GetRenderState_ = 58,
		_CreateStateBlock_ = 59,
		_BeginStateBlock_ = 60,
		_EndStateBlock_ = 61,
		_SetClipStatus_ = 62,
		_GetClipStatus_ = 63,
		_GetTexture_ = 64,
		_SetTexture_ = 65,
		_GetTextureStageState_ = 66,
		_SetTextureStageState_ = 67,
		_GetSamplerState_ = 68,
		_SetSamplerState_ = 69,
		_ValidateDevice_ = 70,
		_SetPaletteEntries_ = 71,
		_GetPaletteEntries_ = 72,
		_SetCurrentTexturePalette_ = 73,
		_GetCurrentTexturePalette_ = 74,
		_SetScissorRect_ = 75,
		_GetScissorRect_ = 76,
		_SetSoftwareVertexProcessing_ = 77,
		_GetSoftwareVertexProcessing_ = 78,
		_SetNPatchMode_ = 79,
		_GetNPatchMode_ = 80,
		_DrawPrimitive_ = 81,
		_DrawIndexedPrimitive_ = 82,
		_DrawPrimitiveUP_ = 83,
		_DrawIndexedPrimitiveUP_ = 84,
		_ProcessVertices_ = 85,
		_CreateVertexDeclaration_ = 86,
		_SetVertexDeclaration_ = 87,
		_GetVertexDeclaration_ = 88,
		_SetFVF_ = 89,
		_GetFVF_ = 90,
		_CreateVertexShader_ = 91,
		_SetVertexShader_ = 92,
		_GetVertexShader_ = 93,
		_SetVertexShaderConstantF_ = 94,
		_GetVertexShaderConstantF_ = 95,
		_SetVertexShaderConstantI_ = 96,
		_GetVertexShaderConstantI_ = 97,
		_SetVertexShaderConstantB_ = 98,
		_GetVertexShaderConstantB_ = 99,
		_SetStreamSource_ = 100,
		_GetStreamSource_ = 101,
		_SetStreamSourceFreq_ = 102,
		_GetStreamSourceFreq_ = 103,
		_SetIndices_ = 104,
		_GetIndices_ = 105,
		_CreatePixelShader_ = 106,
		_SetPixelShader_ = 107,
		_GetPixelShader_ = 108,
		_SetPixelShaderConstantF_ = 109,
		_GetPixelShaderConstantF_ = 110,
		_SetPixelShaderConstantI_ = 111,
		_GetPixelShaderConstantI_ = 112,
		_SetPixelShaderConstantB_ = 113,
		_GetPixelShaderConstantB_ = 114,
		_DrawRectPatch_ = 115,
		_DrawTriPatch_ = 116,
		_DeletePatch_ = 117,
		_CreateQuery_ = 118,
		_MaxMethods_ = 119,

		// IDirect3DDevice9Ex methods
		_SetConvolutionMonoKernel_ = 119,
		_ComposeRects_ = 120,
		_PresentEx_ = 121,
		_GetGPUThreadPriority_ = 122,
		_SetGPUThreadPriority_ = 123,
		_WaitForVBlank_ = 124,
		_CheckResourceResidency_ = 125,
		_SetMaximumFrameLatency_ = 126,
		_GetMaximumFrameLatency_ = 127,
		_CheckDeviceState_ = 128,
		_CreateRenderTargetEx_ = 129,
		_CreateOffscreenPlainSurfaceEx_ = 130,
		_CreateDepthStencilSurfaceEx_ = 131,
		_ResetEx_ = 132,
		_GetDisplayModeEx_ = 133,

		_MaxMethodsEx_
	};
	
inline int GetMutableIndex(Names entryIndex)
	{

		switch(entryIndex)
		{
		case _SetMaterial_:
			return 0;
		case _SetRenderState_:
			return 1;
		case _SetTexture_:
			return 2;
		case _SetTextureStageState_:
			return 3;
		case _SetSamplerState_:
			return 4;
		case _SetFVF_:
			return 5;
		case _SetVertexShader_:
			return 6;
		case _SetVertexShaderConstantF_:
			return 7;
		case _SetVertexShaderConstantI_:
			return 8;
		case _SetVertexShaderConstantB_:
			return 9;
		case _SetStreamSource_:
			return 10;
		case _SetStreamSourceFreq_:
			return 11;
		case _SetIndices_:
			return 12;
		case _SetPixelShader_:
			return 13;
		case _SetPixelShaderConstantF_:
			return 14;
		case _SetPixelShaderConstantI_:
			return 15;
		case _SetPixelShaderConstantB_:
			return 16;
		}
		return -1;
	}
}

