#include "netvarhkmgr.hpp"

CNetvarHookManager netvarHookManager;
void CNetvarHookManager::Hook(std::string tableToHook, std::string propToHook, NetvarHookCallback callBack)
{
	auto clientClass = Interfaces::Client->GetAllClasses();

	while (clientClass)
	{
		std::string tableName = clientClass->m_pRecvTable->m_pNetTableName;

		if (tableName == tableToHook)
		{
			for (int i = 0; i < clientClass->m_pRecvTable->m_iCount; i++)
			{
				auto& prop = clientClass->m_pRecvTable->m_pProps[i];
				std::string propName = prop.m_pVarName;

				if (propName == propToHook)
				{
					prop.m_ProxyFn = callBack;
				}
			}
		}

		clientClass = clientClass->m_pNext;
	}
}