#ifndef __S3D_CMDFLOWSTREAMERS_H__
#define __S3D_CMDFLOWSTREAMERS_H__

#include "XMLStreamer.h"
#include "GlobalData.h"

/////////////////////////////////////////////////////////////////////////////

namespace WriteStreamer
{

	class CmdFlowWriteStreamer: public XMLStreamer
	{
		friend struct boost::details::pool::singleton_default<CmdFlowWriteStreamer>;
		private:
		CmdFlowWriteStreamer()
			:	XMLStreamer( XMLStreamer::TXML_OpenForWrite )
		{
		}
		virtual ~CmdFlowWriteStreamer(){};
	};

	CmdFlowWriteStreamer&					Get						();

	bool									IsOK					();

	void									CmdBegin				( D3D10DDI_HDEVICE hDevice_, Commands::Command* pCmd_, const char* szName_  );
	void									CmdEnd					();
	void									DeviceStateBegin		();
	void									DeviceStateEnd			();

	void									Comment					( char* szComment_, ... );

	template <typename T> void				Value					( const char* szComment_, T val_ )
	{
		Get().WriteValue(szComment_,val_);
	}

	template <typename T> void				ValueByRef				( const char* szComment_, const T* pVal_ )
	{
		Get().WriteValueByRef(szComment_,pVal_ );
	}

	template <typename T> void				Reference				( const char* szComment_, const T& val_ )
	{
		Get().WriteReference( szComment_, val_ );
	}

	template <typename T> void				Handle					( const char* szComment_, const T* pVal_ )
	{
		Get().WriteValue( szComment_, (UINT_PTR)pVal_ );
	}

	template<typename T> void				ArrayValues				( const char* szAttribute_, SIZE_T nCount_, const T*  pArray_)
	{
		Get().WriteArrayValues( szAttribute_, nCount_, pArray_ );
	}

	template<typename T, typename A> void	ArrayValues				( const char* szAttribute_, SIZE_T nCount_, const std::vector<T,A>& pArray_ )
	{
		Get().WriteArrayValues( szAttribute_, nCount_, pArray_ );
	}

	template<typename T, typename A, typename Tr> void	ArrayValues	( const char* szAttribute_, SIZE_T nCount_, const array<T,A, Tr>& pArray_ )
	{
		Get().WriteArrayValues( szAttribute_, nCount_, pArray_ );
	}

	template<typename T> void				ArrayReferences			( const char* szAttribute_, SIZE_T nCount_, const T*  pArray_)
	{
		Get().WriteArrayReferences( szAttribute_, nCount_, pArray_ );
	}

	template<typename T, typename A> void	ArrayReferences			( const char* szAttribute_, SIZE_T nCount_, const std::vector<T,A>& pArray_ )
	{
		Get().WriteArrayReferences( szAttribute_, nCount_, pArray_ );
	}

	template<typename T, typename A, typename Tr> void	ArrayReferences( const char* szAttribute_, SIZE_T nCount_, const array<T,A,Tr>& pArray_ )
	{
		Get().WriteArrayReferences( szAttribute_, nCount_, pArray_ );
}

}

/////////////////////////////////////////////////////////////////////////////

namespace ReadStreamer
{

	class CmdFlowReadStreamer: public XMLStreamer
	{
		friend struct boost::details::pool::singleton_default<CmdFlowReadStreamer>;
	private:
		CmdFlowReadStreamer()
			:	XMLStreamer( XMLStreamer::TXML_OpenForRead )
		{
		}
		virtual ~CmdFlowReadStreamer(){};
	};

	CmdFlowReadStreamer&					Get						();

	void									CmdBegin				( D3D10DDI_HDEVICE& hDevice_  );
	void									CmdEnd					();

	void									Skip					();

	template <typename T> void				Value					( T& val_ )
	{
		Get().ReadValue( val_ );
	}

	template <typename T> void				ValueByRef				( T* & val_ )
	{
		Get().ReadValueByRef( val_ );
	}

	template <typename T> void				Reference				( T& val_ )
	{
		Get().ReadReference( val_ );
	}

	template<typename T, typename A> void	ArrayValues				( std::vector<T,A>& pArray_, SIZE_T nCount_ )
	{
		Get().ReadArrayValues( nCount_, pArray_ );
	}

	template<typename T, typename A, typename Tr> void	ArrayValues	( array<T,A,Tr>& pArray_, SIZE_T nCount_ )
	{
		Get().ReadArrayValues( nCount_, pArray_ );
	}

	template<typename T> void	ArrayValues							( T* pArray_, SIZE_T nCount_ )
	{
		Get().ReadArrayValues( nCount_, pArray_ );
	}

	template<typename T, typename A> void	ArrayReferences			( std::vector<T,A>& pArray_, SIZE_T nCount_ )
	{
		Get().ReadArrayReferences( nCount_, pArray_ );
	}

	template<typename T, typename A, typename Tr> void	ArrayReferences	( array<T,A,Tr>& pArray_, SIZE_T nCount_ )
	{
		Get().ReadArrayReferences( nCount_, pArray_ );
	}

	template<typename T> void				ArrayReferences			( T* pArray_, SIZE_T nCount_ )
	{
		Get().ReadArrayReferences( nCount_, pArray_ );
	}

	template <typename T> void				Handle					( T& val_ )
	{
		val_.handle = NULL;
		Get().Skip();
		//Get().ReadValueByRef( val_, it_++ );
	}

}

/////////////////////////////////////////////////////////////////////////////

#endif//__S3D_CMDFLOWSTREAMERS_H__