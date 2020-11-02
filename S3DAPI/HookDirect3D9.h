#pragma once
#include <d3d9.h>

template<typename Signature, Signature>
struct function_offset;

#ifndef REGISTER_FUNCTION_OFFSET
#define REGISTER_FUNCTION_OFFSET(Func, Offset) template<> struct function_offset<decltype(Func), Func> { static const int value = Offset; };
#endif

REGISTER_FUNCTION_OFFSET(&IDirect3D9::QueryInterface, 0)
REGISTER_FUNCTION_OFFSET(&IDirect3D9::AddRef, 1)
REGISTER_FUNCTION_OFFSET(&IDirect3D9::Release, 2)
REGISTER_FUNCTION_OFFSET(&IDirect3D9::RegisterSoftwareDevice, 3)
REGISTER_FUNCTION_OFFSET(&IDirect3D9::GetAdapterCount, 4)
REGISTER_FUNCTION_OFFSET(&IDirect3D9::GetAdapterIdentifier, 5)
REGISTER_FUNCTION_OFFSET(&IDirect3D9::GetAdapterModeCount, 6)
REGISTER_FUNCTION_OFFSET(&IDirect3D9::EnumAdapterModes, 7)
REGISTER_FUNCTION_OFFSET(&IDirect3D9::GetAdapterDisplayMode, 8)
REGISTER_FUNCTION_OFFSET(&IDirect3D9::CheckDeviceType, 9)
REGISTER_FUNCTION_OFFSET(&IDirect3D9::CheckDeviceFormat, 10)
REGISTER_FUNCTION_OFFSET(&IDirect3D9::CheckDeviceMultiSampleType, 11)
REGISTER_FUNCTION_OFFSET(&IDirect3D9::CheckDepthStencilMatch, 12)
REGISTER_FUNCTION_OFFSET(&IDirect3D9::CheckDeviceFormatConversion, 13)
REGISTER_FUNCTION_OFFSET(&IDirect3D9::GetDeviceCaps, 14)
REGISTER_FUNCTION_OFFSET(&IDirect3D9::GetAdapterMonitor, 15)
REGISTER_FUNCTION_OFFSET(&IDirect3D9::CreateDevice, 16)
