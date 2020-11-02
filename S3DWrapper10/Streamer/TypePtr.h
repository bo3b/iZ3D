#ifndef __S3D_TYPEPTR_H__
#define __S3D_TYPEPTR_H__

//////////////////////////////////////////////////////////////////////////

class CTypePtr
{
public:
	virtual	void		MemFree		()	{ __debugbreak(); }
	virtual				~CTypePtr	()	{};
	CTypePtr&			operator=	( const CTypePtr& src_ );
protected:	
private:
};

/************************************************************************/
/* operator =                                                           */
/************************************************************************/
inline CTypePtr& CTypePtr::operator=	( const CTypePtr& /*src_*/ )
{
	__debugbreak();
	return *this;
}

//////////////////////////////////////////////////////////////////////////

template <typename T>
class CValPtr: public CTypePtr
{
public:
						CValPtr		( T*& p_ );
						CValPtr		( const CValPtr< T >& src_ );
						~CValPtr	();
	virtual		void	MemFree		();
	CValPtr< T >&		operator=	( const CValPtr< T >& src_ );
protected:
private:
	T*			m_pPtr;
};


/************************************************************************/
/* .ctor                                                                */
/************************************************************************/
template <typename T>
CValPtr<T>::CValPtr( T*& p_ )
{
#pragma warning(suppress : 4345)
	m_pPtr = DNew T();
	p_ = m_pPtr;
}

/************************************************************************/
/* copy .ctor                                                           */
/************************************************************************/
template <typename T>
CValPtr<T>::CValPtr	( const CValPtr< T >& src_ )
{
	m_pPtr = src_.m_pPtr;
}

/************************************************************************/
/* .dtor                                                                */
/************************************************************************/
template <typename T>
CValPtr<T>::~CValPtr()
{
}

/************************************************************************/
/* operator =                                                           */
/************************************************************************/
template <typename T>
CValPtr< T >&	CValPtr< T >::operator=	( const CValPtr< T >& src_ )
{
	m_pPtr = src_.m_pPtr;
	return *this;
}

/************************************************************************/
/* need free mem manualy!!!                                             */
/************************************************************************/
template <typename T>
void CValPtr<T>::MemFree()
{
	delete m_pPtr;
}


//////////////////////////////////////////////////////////////////////////

template <typename T>
class CArrayPtr: public CTypePtr
{
public:
						CArrayPtr	( T*& p_, SIZE_T nItemCount_ );
						CArrayPtr	( const CArrayPtr< T >& src_ );
						~CArrayPtr	();
	virtual		void	MemFree		();
	CArrayPtr< T >&		operator=	( const CArrayPtr< T >& src_ );
protected:
private:
	T*			m_pPtr;
};

/************************************************************************/
/* .ctor                                                                */
/************************************************************************/
template <typename T>
CArrayPtr<T>::CArrayPtr( T*& p_, SIZE_T nItemCount_ )
{
	_ASSERT ( nItemCount_ );
	m_pPtr = DNew T[ nItemCount_ ];
	p_ = m_pPtr;
}

/************************************************************************/
/* copy .ctor                                                           */
/************************************************************************/
template <typename T>
CArrayPtr<T>::CArrayPtr	( const CArrayPtr< T >& src_ )
{
	m_pPtr = src_.m_pPtr;
}

/************************************************************************/
/* .dtor                                                                */
/************************************************************************/
template <typename T>
CArrayPtr<T>::~CArrayPtr()
{
}

/************************************************************************/
/* operator =                                                           */
/************************************************************************/
template <typename T>
CArrayPtr< T >&	CArrayPtr< T >::operator=	( const CArrayPtr< T >& src_ )
{
	m_pPtr = src_.m_pPtr;
	return *this;
}

/************************************************************************/
/* need free mem manualy!!!                                             */
/************************************************************************/
template <typename T>
void CArrayPtr<T>::MemFree()
{
	delete[] m_pPtr;
}

//////////////////////////////////////////////////////////////////////////

#endif//__S3D_TYPEPTR_H__