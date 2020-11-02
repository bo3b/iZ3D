#pragma once

namespace iz3d{
	namespace utils{
		template < typename TReturn, typename P1, typename P2 >
		struct IFunction2{
			typedef TReturn TReturnType;
			typedef P1		TParam1;
			typedef P2		TParam2;
			virtual TReturn func( P1 p1, P2 p2 ) = 0;
		};

		template < typename TReturn, typename TClass,
			typename P1, typename P2 >
		class MemFunction2;

		template < typename TReturn, typename TClass, typename P1, typename P2 >
		class MemFunction2 : public IFunction2<TReturn, P1, P2>{
		public:
			typedef TReturn (TClass::* TFunction ) ( P1, P2 );
			MemFunction2( TFunction pFunc, TClass* pObj)
				:m_pFunc(pFunc), m_pObj(pObj)
			{
			}
			virtual TReturn func( P1 p1, P2 p2 ){
				return (m_pObj->*m_pFunc)( p1, p2 );
			}
		private:
			TFunction m_pFunc;
			TClass* m_pObj;
		};


		//////////////////////////////////////////////////////////////////////////
		template
			<typename TRetType>
		struct IFunction{
			typedef TRetType TReturnType;				
			virtual TRetType func() = 0;
		};

		template< typename TRetType, typename TOwner, typename TParam, typename TMemFunc >
		class TWrapperMemFunc1Par : public IFunction<TRetType>
		{
			TOwner* m_Owner;
			TParam m_Param;
			TMemFunc m_MemFunc;
		public:
			TWrapperMemFunc1Par( TOwner* owner, TParam par, TMemFunc memFunc )
				: m_Owner( owner ), m_Param( par ), m_MemFunc( memFunc )
			{}
			virtual TRetType func(){
				return ( m_Owner->*m_MemFunc )( m_Param );
			}
			virtual ~TWrapperMemFunc1Par(){}
		};

		template< typename TRetType, typename TFuncType, typename TParam >
		class TWrapperFunc1Par : public IFunction< TRetType >
		{
			TFuncType m_Func;
			TParam m_Param;
		public:
			TWrapperFunc1Par( TFuncType Func, TParam Param )
				: m_Func( Func ), m_Param( Param )
			{
			}
			virtual TRetType func(){
				return m_Func( m_Param );
			}

			virtual ~TWrapperFunc1Par(){}
		};
		typedef IFunction< void > TVoidFunction;

		template < typename TOwner, typename TParam, typename TMemFunc >
			TVoidFunction* CreatFunctionFromMemFunc1( TOwner* owner, TParam par, TMemFunc memFunc )
		{
			return new TWrapperMemFunc1Par<TOwner, TParam, TMemFunc( owner, par, memFunc );
		}

		template < typename TFuncType, typename TParam >
		TVoidFunction* CreatFunctionFromFunc1( TFuncType Func, TParam par )
		{
			return new TWrapperFunc1Par<void, TFuncType, TParam>( Func, par );
		}
	}
}