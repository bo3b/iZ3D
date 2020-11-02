#pragma once
#include <Unknwn.h>
#include "..\CommonUtils\COMLikeRepository.h"
template < typename TDeletable, typename IDeletable >
struct Deletable : public IDeletable, IUnknown {
	// Query Interface
	HRESULT __stdcall QueryInterface(
		REFIID /*riid*/,
		void **ppvObject)
	{
		*ppvObject = 0;
		return E_FAIL;
	}

	// AddRef
	ULONG __stdcall AddRef(){
		return 0;
	}

	// Release
	ULONG __stdcall Release(){
		delete ( TDeletable* )this;
		return 0;
	}

	Deletable(){
		COMLikeRepository::instance().Add( this );
	}
};
