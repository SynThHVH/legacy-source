#pragma once
#include "../../../utilities/include.hpp"
#include "../../../source-sdk/valve/recv.hpp"

class CRecvProxyData;
typedef void(*NetvarHookCallback)(const CRecvProxyData* pData, void* pStruct, void* pOut);

class CNetvarHookManager
{
public:
	void Hook(std::string table, std::string prop, NetvarHookCallback callback);
private:
};
extern CNetvarHookManager netvarHookManager;