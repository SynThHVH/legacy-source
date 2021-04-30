#pragma once

typedef IClientNetworkable*   ( *CreateClientClassFn )( int entnum, int serialNum );
typedef IClientNetworkable*   ( *CreateEventFn )( );

class ClientClass
{
public:
	void*			m_pCreateFn;
	void*			m_pCreateEventFn;
	char			*m_pNetworkName;
	RecvTable		*m_pRecvTable;
	ClientClass		*m_pNext;
	int				m_ClassID;
};