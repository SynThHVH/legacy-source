#include "netvarhks.hpp"
#include "../../features/lagcomp/resolver.hpp"

/* note: was going to use it for the resolver but i decided not too and was like fuck it ill just leave it */

void EyeAnglesPitchHook(const CRecvProxyData* pData, void* pStruct, void* pOut)
{
	*reinterpret_cast<float*>(pOut) = pData->m_Value.m_Float;

	auto entity = reinterpret_cast<CBaseEntity*>(pStruct);
	if (!entity)
		return;
}

void EyeAnglesYawHook(const CRecvProxyData* pData, void* pStruct, void* pOut)
{
	*reinterpret_cast<float*>(pOut) = pData->m_Value.m_Float;

	auto entity = reinterpret_cast<CBaseEntity*>(pStruct);
	if (!entity) 
		return;
}