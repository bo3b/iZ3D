#include "stdafx.h"
#include "profilevar.h"

BaseVar* BaseVar::m_pFirst = NULL;
BaseVar* BaseVar::m_pLast  = NULL;

//////////////////////////////////////////////////////////////////////////

BaseVar::BaseVar()
:	m_pNext	( NULL )
{
	if ( m_pLast )
	{
		m_pLast->m_pNext = this;
		m_pLast = this;
	}
	else
		m_pFirst = m_pLast = this;
}


BaseVar::~BaseVar()
{
	BaseVar* pPrev	= m_pFirst;
	BaseVar* pNext	= pPrev->m_pNext;

	while ( pNext && pNext != this  )
	{
		pPrev	= pNext;
		pNext	= pNext->m_pNext;
	}

	if ( pPrev != m_pLast )
	{
		_ASSERT( pNext == this );
		
		if ( pNext == m_pLast )
		{
			pPrev->m_pNext = NULL;
			m_pLast = pPrev;
		}
		else		
			pPrev = pNext->m_pNext;		

	}
	else
		m_pFirst = m_pLast = NULL;
	
}

//////////////////////////////////////////////////////////////////////////