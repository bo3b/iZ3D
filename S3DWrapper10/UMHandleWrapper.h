#pragma once

//#define ADD_SIGN

// D3D10DDIARG_CREATERE...
template <typename HTYPE>
class UMHandleWrapper
{
public:
	UMHandleWrapper();
#ifdef ADD_SIGN
	char* SIGN;
#endif	
	HTYPE			GetWrapperHandle	()	{ HTYPE hOrig = { this }; return hOrig;	}
};

template <typename HTYPE>
UMHandleWrapper<HTYPE>::UMHandleWrapper()
{
#ifdef ADD_SIGN
	SIGN = "iZ3D";
#endif
}

template <class T>
inline UINT_PTR GetOriginalHandle(UINT_PTR handle)
{ 
	if (handle)
		return (UINT_PTR)(((T*)handle)->GetHandle().pDrvPrivate); 
	else
		return handle;
}

template <class T, class H>
inline H GetOriginalHandle(H handle)
{ 
	if (handle.pDrvPrivate)
		return ((T*)handle.pDrvPrivate)->GetHandle(); 
	else
		return handle;
}

template <class T>
inline UINT_PTR GetPrimaryResourceHandle(UINT_PTR handle)
{ 
	if (handle)
		return (UINT_PTR)(((T*)handle)->GetPrimaryHandle().pDrvPrivate); 
	else
		return handle;
}

template <class T, class H>
inline H GetPrimaryResourceHandle(H handle)
{ 
	if (handle.pDrvPrivate)
		return ((T*)handle.pDrvPrivate)->GetPrimaryHandle(); 
	else
		return handle;
}

template <class T, class H>
inline bool IsUsedStereoHandle(H handle)
{
	if (handle.pDrvPrivate)
		return ((T*)handle.pDrvPrivate)->IsStereo();
	else 
		return false;
}

template <class T, class H>
inline H UnwrapCurrentViewHandle(H handle)
{
	H hRes;
	if (handle.pDrvPrivate)
		hRes = ((T*)handle.pDrvPrivate)->GetCurrentViewHandle();
	else
		hRes.pDrvPrivate = NULL;
	return hRes;
}

template <class T>
inline UINT_PTR UnwrapCurrentViewHandle(UINT_PTR handle)
{
	if ( handle ) 
		return (UINT_PTR)((T*)handle)->GetCurrentViewHandle().pDrvPrivate;
	else
		return NULL;	
}

template <typename H, class T>
inline void InitWrapper(H handle, T* &pWrapper)
{ 
	pWrapper = (T*)handle.pDrvPrivate; 
}
