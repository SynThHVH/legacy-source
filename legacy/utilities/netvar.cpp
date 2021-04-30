#include "../utilities/include.hpp"

// from antario xd i like theirs

#undef GetProp 

std::unique_ptr<NetvarTree> g_pNetvars;

NetvarTree::NetvarTree( )
{
	const ClientClass* clientClass = Interfaces::Client->GetAllClasses( );

	while ( clientClass != nullptr )
	{
		const auto classInfo = std::make_shared<Node>( 0 );
		RecvTable* recvTable = clientClass->m_pRecvTable;

		this->PopulateNodes( recvTable, &classInfo->nodes );
		nodes.emplace( recvTable->m_pNetTableName, classInfo );

		clientClass = clientClass->m_pNext;
	}
}

void NetvarTree::PopulateNodes( RecvTable* recvTable, MapType* mapType )
{
	for ( auto i = 0; i < recvTable->m_iCount; i++ )
	{
		const RecvProp* prop = &recvTable->m_pProps[ i ];
		const auto  propInfo = std::make_shared<Node>( prop->m_iOffset );

		if ( prop->m_iRecvType == SendPropType::DPT_DataTable )
			this->PopulateNodes( prop->m_pDataTable, &propInfo->nodes );

		mapType->emplace( prop->m_pVarName, propInfo );
	}
}