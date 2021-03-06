#pragma once

class CGlobalVarsBase
{
//public:
	// for encoding m_flSimulationTime, m_flAnimTime
	//int GetNetworkBase( int nTick, int nEntity );

public:
	float     realtime;
	int       framecount;
	float     absoluteframetime;
	float     absoluteframestarttimestddev;
	float     curtime;
	float     frametime;
	int       maxClients;
	int       tickcount;
	float     interval_per_tick;
	float     interpolation_amount;
	int       simTicksThisFrame;
	int       network_protocol;
	void*     pSaveData;
	bool      m_bClient;
	bool      m_bbRemoteClient;

private:
	int       nTimestampNetworkingBase;
	int       nTimestampRandomizeWindow;
};

/*inline int CGlobalVarsBase::GetNetworkBase( int nTick, int nEntity )
{
	int nEntityMod = nEntity % nTimestampRandomizeWindow;
	int nBaseTick = nTimestampNetworkingBase * (int)( ( nTick - nEntityMod ) / nTimestampNetworkingBase );
	return nBaseTick;
}
*/