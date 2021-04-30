#pragma once

#define clamp(a,b,c) ( (a) > (c) ? (c) : ( (a) < (b) ? (b) : (a) ) ) // could use std but i found this in source sdk lmao

#define TIME_TO_TICKS( dt )		((int)( 0.5f + (float)(dt) / Interfaces::GlobalVars->interval_per_tick ))
#define TICKS_TO_TIME( t )		(Interfaces::GlobalVars->interval_per_tick *( t ))

struct Record
{
	float flSimulationTime = -1.f;
	bool bShot = false;
	Vec3D vVelocity;
	matrix3x4_t mMatrix[128];
	Vec3D Max, Min;
	std::array<float, 24> flPosParam;
	CPlayerAnimstate* AnimState;
	CAnimationLayer AnimLayers[ 15 ];
	CAnimationLayer ClientAnimLayers[15];
	Vec3D vEyeAngles;
	Vec3D vOrigin;
	float flGoalFeetYaw;
	float flLby;
	int iFlags;
	float flCycleDelta;
	float flSimDelta;
	std::string sCurrentResolve;
	int iCurrentMode;
	int iBoneCount;
	bool bLbyUpdated = false;
};

namespace LagComp
{
	// essentials
	static float LerpTime( ) 
	{
		if ( !Interfaces::Cvar )
		{
			return 0.0f;
		}

		int iUpdateRate = Interfaces::Cvar->FindVar( "cl_updaterate" )->ReadInt( );
		ConVar *pMinUpdateRate = Interfaces::Cvar->FindVar( "sv_minupdaterate" );
		ConVar *pMaxUpdateRate = Interfaces::Cvar->FindVar( "sv_maxupdaterate" );

		if ( pMinUpdateRate && pMaxUpdateRate )
		{
			iUpdateRate = clamp( iUpdateRate,  pMinUpdateRate->ReadInt( ), pMaxUpdateRate->ReadInt( ) );
		}

		float flLerpRatio = Interfaces::Cvar->FindVar( "cl_interp_ratio" )->ReadFloat( );
		if ( flLerpRatio == 0 )
		{
			flLerpRatio = 1.0f;
		}

		float flLerpAmount = Interfaces::Cvar->FindVar( "cl_interp" )->ReadFloat( );

		ConVar *pMin = Interfaces::Cvar->FindVar( "sv_client_min_interp_ratio" );
		ConVar *pMax = Interfaces::Cvar->FindVar( "sv_client_max_interp_ratio" );
		if ( pMin && pMax && pMin->ReadFloat( ) != -1 )
		{
			flLerpRatio = clamp( flLerpRatio, pMin->ReadFloat( ), pMax->ReadFloat( ) );
		}
		else
		{
			if ( flLerpRatio == 0 )
			{
				flLerpRatio = 1.0f;
			}
		}

		return max( flLerpAmount, flLerpRatio / iUpdateRate );

	}

	static bool ValidTick( float flSimtime )
	{
		if ( !Interfaces::Engine->GetNetChannelInfo( ) )
			return false;

		float CorrectCurtime = Interfaces::GlobalVars->curtime;

		if ( Interfaces::User( ) && Interfaces::User( )->m_bIsAlive( ) )
			CorrectCurtime = float( Interfaces::User( )->m_iTickBase( ) ) * Interfaces::GlobalVars->interval_per_tick;

		float flCorrect = clamp( Interfaces::Engine->GetNetChannelInfo( )->GetAvgLatency( FLOW_OUTGOING ) + Interfaces::Engine->GetNetChannelInfo( )->GetAvgLatency( FLOW_INCOMING ) + LerpTime( ), 0.f, 0.2f );

		return ( fabs( flCorrect - ( CorrectCurtime - flSimtime ) ) < 0.2f );
	}

	static void Interpolation( CBaseEntity* pEntity, bool bValue )
	{
		auto m_dwVarMap = reinterpret_cast< uintptr_t >( pEntity ) + 36;
		for ( auto size = 0; size < *reinterpret_cast< int* >( m_dwVarMap + 20 ); size++ )
		{
			*reinterpret_cast< uintptr_t* >( *reinterpret_cast< uintptr_t* >( m_dwVarMap ) + size * 12 ) = int(bValue);
		}
	}

	// dynamic memory player
	extern std::deque<Record> pPlayer[ MAX_NETWORKID_LENGTH ];

	// local stuff
	extern float aflOldSimulationTime[ MAX_NETWORKID_LENGTH ];
	extern float aflOldShotTime[ MAX_NETWORKID_LENGTH ];
	extern float aflOldCycle[ MAX_NETWORKID_LENGTH ];
	extern float aflOldCurtime[ MAX_NETWORKID_LENGTH ];
	extern bool bWasDormant[MAX_NETWORKID_LENGTH];
	extern int aiTickOnShot[ MAX_NETWORKID_LENGTH ];
	extern int aiTickLby[MAX_NETWORKID_LENGTH];
	extern matrix3x4_t pMatrix[ MAX_NETWORKID_LENGTH ][128];
	extern Vec3D pOrigin[ MAX_NETWORKID_LENGTH ];

	extern bool bNextFrameRebuild;

	// main functions
	extern void FrameStage( int iStage );

	extern void CreateRecord( CBaseEntity* pEntity, CBaseEntity* pLocal );

	extern void RestoreRecord( CBaseEntity* pEntity, int iTick );

	extern void RemoveRecords( int Index );

	extern void ClearRecords( int Index );
	
	extern void HandleShots( );
}