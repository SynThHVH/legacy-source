#pragma once

// idk y stacker wanted todo like m_ kinda annoying

class IClientEntityList
{
public:
	VF( m_pGetClientNetworkable, IClientNetworkable*, 0, int iIndex );
	VF( m_pGetClientEntity, CBaseEntity*, 3, int iIndex );
	VF( m_pGetClientEntityHandle, CBaseEntity*, 4, CBaseHandle pHandle );
	VF( m_iHighestEntityIndex, int, 6 );
};
