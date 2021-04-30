#pragma once

//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//
//=============================================================================//


#include "common.hpp"

#ifdef GetProp
#undef GetProp
#endif

struct RecvTable;
struct RecvProp;

class DVariant
{
public:
	union
	{
		float	m_Float;
		long	m_Int;
		char	*m_pString;
		void	*m_pData;
		float	m_Vector[ 3 ];
	};
};

struct RecvTable
{
	RecvProp		*m_pProps;
	int				m_iCount;
	void			*m_pDecoder;
	char			*m_pNetTableName;
	bool			m_bInitialized;
	bool			m_bInMainList;
};

struct RecvProp
{
	char					*m_pVarName;
	int						m_iRecvType;
	int						m_iFlags;
	int						m_iStringBufferSize;
	bool					m_bInsideArray;
	const void				*m_pExtraData;
	RecvProp				*m_pArrayProp;
	void*					m_ArrayLengthProxy;
	void*					m_ProxyFn;
	void*					m_DataTableProxyFn;
	RecvTable				*m_pDataTable;
	int						m_iOffset;
	int						m_iElementStride;
	int						m_iElements;
	const char				*m_pParentArrayPropName;
};

class CRecvProxyData
{
public:
	const RecvProp	*m_pRecvProp;
	char _pad[ 4 ];//csgo ( for l4d keep it commented out :) )
	DVariant		m_Value;
	int				m_iElement;
	int				m_ObjectID;
};
typedef void( *RecvVarProxyFn )( const CRecvProxyData *pData, void *pStruct, void *pOut );