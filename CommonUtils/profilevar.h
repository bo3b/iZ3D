#ifndef _PROFILEVAR_H_
#define _PROFILEVAR_H_

//test

//////////////////////////////////////////////////////////////////////////

class BaseVar
{
public:
							BaseVar		();
							~BaseVar	();
	static BaseVar*			First		()		 { return m_pFirst; };
	BaseVar*				Next		() const { return m_pNext; };

	virtual bool			IsChanged	() const = 0;
	virtual void			Save		( std::ostream& Stream_ ) const = 0;

protected:
private:

	static BaseVar*			m_pFirst;
	static BaseVar*			m_pLast;

	BaseVar*				m_pNext;

};

//////////////////////////////////////////////////////////////////////////

template < class Type_ > class ProfileVar: public BaseVar
{
public:
							ProfileVar	();
							ProfileVar	( const ProfileVar	< Type_ >& Src_ );
							ProfileVar	( Type_ tValue_ );

							ProfileVar	( Type_ tValue_, Type_ tDefValue_ );

							~ProfileVar	();							

	ProfileVar&				operator =	( const Type_& tValue_ );
	inline operator			Type_		()	const { return m_tValue; };
	
	virtual bool			IsChanged	() const { return m_tValue != m_tDefValue; };
	virtual void			Save		( std::ostream& Stream_ ) const;

protected:
private:
	Type_					m_tValue;
	Type_					m_tDefValue;
};

//------------------------------------------------------------------------
// Name: ProfileVar
// Desc: 
//------------------------------------------------------------------------
template< class Type_ > ProfileVar< Type_ >::ProfileVar()
:	BaseVar()
{
}

//------------------------------------------------------------------------
// Name: ProfileVar
// Desc: 
//------------------------------------------------------------------------
template< class Type_ > ProfileVar< Type_ >::ProfileVar( const ProfileVar	< Type_ >& Src_ )
:	BaseVar()
{
	m_tDefValue = Src_.m_tDefValue;
	m_tValue	= Src_.m_tValue;
}

//------------------------------------------------------------------------
// Name: ProfileVar
// Desc: 
//------------------------------------------------------------------------
template< class Type_ > ProfileVar< Type_ >::ProfileVar( Type_ tValue_, Type_ tDefValue_ )
:	BaseVar()
{
	m_tDefValue = tDefValue_;
	m_tValue	= tValue_;
}

//------------------------------------------------------------------------
// Name: ProfileVar
// Desc: 
//------------------------------------------------------------------------
template< class Type_ > ProfileVar< Type_ >::ProfileVar( Type_ tValue_ )
:	BaseVar()
{
	m_tDefValue = tValue_;
	m_tValue	= tValue_;
}

//------------------------------------------------------------------------
// Name: ~ProfileVar
// Desc: 
//------------------------------------------------------------------------
template< class Type_ > ProfileVar< Type_ >::~ProfileVar()
{
}

//------------------------------------------------------------------------
// Name: operator = 
// Desc: 
//------------------------------------------------------------------------
template< class Type_ > inline ProfileVar< Type_ >& ProfileVar< Type_ >::operator = ( const Type_& tValue_ )
{
	m_tValue = tValue_;
	return *this;
}

//------------------------------------------------------------------------
// Name: Save
// Desc: 
//------------------------------------------------------------------------
template< class Type_ > inline void ProfileVar< Type_ >::Save( std::ostream& Stream_ ) const
{
	Stream_ << m_tValue << std::endl;
}
//////////////////////////////////////////////////////////////////////////

#endif//_PROFILEVAR_H_