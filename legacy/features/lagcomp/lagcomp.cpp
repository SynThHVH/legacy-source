#include "../features.hpp"

/* note: the lag comp isn't the worst but this cheat definetly needs a animation fix lmao */

namespace LagComp 
{
	// dynamic memory player
	std::deque<Record> pPlayer[ MAX_NETWORKID_LENGTH ];

	// local stuff
	float aflOldSimulationTime[ MAX_NETWORKID_LENGTH ];
	float aflOldShotTime[ MAX_NETWORKID_LENGTH ];
	float aflOldCycle[ MAX_NETWORKID_LENGTH ];
	int aiTickOnShot[ MAX_NETWORKID_LENGTH ];
	int aiTickLby[MAX_NETWORKID_LENGTH];
	matrix3x4_t pMatrix[ MAX_NETWORKID_LENGTH ][ 128 ];
	Vec3D pOrigin[ MAX_NETWORKID_LENGTH ];
	float aflOldCurtime[ MAX_NETWORKID_LENGTH ];
	bool bWasDormant[MAX_NETWORKID_LENGTH];
}

void LagComp::HandleShots( )
{
	static float flShotAtTime[ 65 ];

	if ( Rage::ShotInfo.size( ) != 0 )
	{
		AimbotShotInfo_t Shot = Rage::ShotInfo.at( 0 );

		if ( flShotAtTime[ Shot.iIndex ] != Shot.flTargetsSimtime )
		{
			PlayerInfo_t pInfo;
			Interfaces::Engine->GetPlayerInfo( Shot.iIndex, &pInfo );
			std::string Message;

			if ( Globals::bHit && Globals::bShot )
			{
				flShotAtTime[ Shot.iIndex ] = Shot.flTargetsSimtime;

				Message = "Hit: " + std::string( pInfo.m_cName ) + " With Resolve " + Shot.sResolveMode + "\n";

				Rage::ShotInfo.erase( Rage::ShotInfo.begin( ) );
			}
			else if ( Globals::bShot )
			{
				flShotAtTime[ Shot.iIndex ] = Shot.flTargetsSimtime;

				if (Shot.sResolveMode != "r:backtrack" && Shot.iMode == 0)
				{
					Globals::iMissedShots[Shot.iIndex][0] += 1;
				}

				Message = "Missed: " + std::string( pInfo.m_cName ) + " With Resolve " + Shot.sResolveMode + "\n";

				Rage::ShotInfo.erase( Rage::ShotInfo.begin( ) );
			}

			Globals::Msg( Message.c_str( ) );
		}
		else
			Rage::ShotInfo.erase( Rage::ShotInfo.begin( ) );
	}
	
	Globals::bHit = false;
	Globals::bShot = false;
}

void LagComp::FrameStage( int iStage )
{
	if ( !Interfaces::Engine->IsInGame( ) || !Interfaces::Engine->IsConnected( ) )
		return;

	if ( iStage == FRAME_RENDER_START )
		HandleShots( );

	auto pLocalEnt = Interfaces::User( );

	if ( !pLocalEnt )
		return;

	for ( int index = 1; index < MAX_NETWORKID_LENGTH; ++index )
	{
		CBaseEntity* pPlayerEntity = Interfaces::EntityList->m_pGetClientEntity( index );

		if ( !pPlayerEntity
			|| !pPlayerEntity->m_bIsAlive( )			
			|| pPlayerEntity->IsDormant( ) )
		{
			bWasDormant[index] = true;
			Resolver::bCanUpdate[index] = false;
			ClearRecords( index );
			continue;
		}

		if ( iStage == FRAME_NET_UPDATE_END && pLocalEnt->entindex( ) != index ) // any disable interp ever
		{
			Interpolation( pPlayerEntity, false );
		}

		if ( iStage == FRAME_RENDER_START )
		{
			aiTickOnShot[ index ] = -1;
			aiTickLby[index] = -1;

			if ( aflOldSimulationTime[ index ] != pPlayerEntity->m_flSimulationTime( ) )
				CreateRecord( pPlayerEntity, pLocalEnt );
			aflOldSimulationTime[ index ] = pPlayerEntity->m_flSimulationTime( );

			RestoreRecord( pPlayerEntity, pPlayer[ index ].size( ) - 1 );

			RemoveRecords( index );

			pPlayerEntity->m_bClientSideAnimation( ) = false;
		}

		bWasDormant[index] = false;
	}	
}

void LagComp::CreateRecord( CBaseEntity* pEntity , CBaseEntity* pLocal )
{
	bool bShot = false;
	int iIndex = pEntity->entindex( );

	Record PushPlayer;

	CPlayerAnimstate* AnimState = pEntity->m_pAnimState( );

	if ( !AnimState )
		return;

	if ( !pEntity->m_pAnimOverlay( ) )
		return;

	CBaseCombatWeapon * pWeapon = pEntity->GetActiveWeapon( );

	PushPlayer.flSimDelta = pEntity->m_flSimulationTime( ) - aflOldSimulationTime[ iIndex ];

	if ( pWeapon )
	{
		float flShotTime = pWeapon->m_fLastShotTime( );

		if ( pEntity->m_flSimulationTime( ) >= flShotTime && aflOldSimulationTime[ iIndex ] < flShotTime ) // shottime is in bounds and if so they shot
		{
			bShot = true;
			if ( Menu::Config.onshotmode == 3 && iIndex != pLocal->entindex( ) ) // just return so the record isnt created v bad
				return;
		}
		else if ( aflOldShotTime[ iIndex ] != flShotTime ) // player shot and their shot isnt in the bounds so they are hideshotting dont record it
		{
			aflOldShotTime[ iIndex ] = flShotTime;
			return;
		}
	}

	pEntity->m_bClientSideAnimation() = true;
//	pEntity->m_bClientSideFrameReset() = true;
	Globals::UpdateAnimations = true;

	int iFlagsBackup = pEntity->m_fFlags( );
	Vec3D vVelocityBackup = pEntity->m_vecVelocity( );

	Vec3D OldPredAng;

	if ( pLocal && iIndex == pLocal->entindex( ) )
	{
		if ( !( pEntity->m_fFlags( ) & FL_ONGROUND ) )
		{
			pEntity->m_fFlags( ) |= FL_ONGROUND;
			pEntity->m_vecVelocity( ) = Vec3D( 0, 0, 0 );
		}

		if ( GetKeyState( Menu::Config.tpkey ) )
		{
			Interfaces::Prediction->GetLocalViewAngles( OldPredAng );
			Interfaces::Prediction->SetLocalViewAngles( pLocal->m_angEyeAngles( ) );
		}
	}

	if ( AnimState->m_iLastClientSideAnimationUpdateFramecount == Interfaces::GlobalVars->framecount )
		AnimState->m_iLastClientSideAnimationUpdateFramecount -= 1;

	Resolver::PreResolver( AnimState, pEntity, bShot, PushPlayer.bLbyUpdated);

	memcpy( PushPlayer.AnimLayers, pEntity->m_pAnimOverlay( ), ( sizeof( CAnimationLayer ) * 15 ) );

	PushPlayer.flCycleDelta = ( ( PushPlayer.AnimLayers[ 6 ].m_flCycle < aflOldCycle[ iIndex ] ) ? ( PushPlayer.AnimLayers[ 6 ].m_flCycle - ( aflOldCycle[ iIndex ] - 1.f ) ) : ( PushPlayer.AnimLayers[ 6 ].m_flCycle - aflOldCycle[ iIndex ] ) );

	AnimState->m_flDelta = fmaxf( Interfaces::GlobalVars->curtime - aflOldCurtime[ iIndex ], 0.0 );

	pEntity->UpdateClientSideAnimations( );
 
//	memcpy( PushPlayer.ClientAnimLayers, pEntity->m_pAnimOverlay( ), ( sizeof( CAnimationLayer ) * 15 ) );

	float flFeetDelta = Math::m_flNormalizeYaw(pEntity->m_angEyeAngles().y - AnimState->m_flGoalFeetYaw);

	if (flFeetDelta > 60.f)
		flFeetDelta = 60.f;
	if (flFeetDelta < -60.f)
		flFeetDelta = -60.f;

//	pEntity->m_flPoseParameter()[2] = 0.f;
	pEntity->m_flPoseParameter()[11] = (0.5f + (flFeetDelta / 120.f));

	PushPlayer.AnimLayers[ 0 ] = pEntity->m_pAnimOverlay( )[ 0 ];

	pEntity->m_bClientSideAnimation() = false;
	Globals::UpdateAnimations = false;

	pEntity->InvalidateBoneCache( );

	pEntity->SetAbsAngles(Vec3D(0, AnimState->m_flGoalFeetYaw, 0));
	pEntity->SetAbsOrigin(pEntity->m_vecOrigin()); 

	int iSomeFlagsBackup = *reinterpret_cast< int* >( reinterpret_cast< DWORD >( pEntity ) + 0xE4 );

	*reinterpret_cast< int* >( reinterpret_cast< DWORD >( pEntity ) + 0xE4) |= 8; // some flags *(_DWORD *)(v7 + 224 + 4) |= 8u;
	*reinterpret_cast< int* >( reinterpret_cast< DWORD >( pEntity ) +0xA58 ) = 0; // last setupbones frame *(_DWORD *)(v7 + 0xA64 + 4) = *(_DWORD *)(GlobalVars_0 + 4);

	if ( pLocal && iIndex != pLocal->entindex( ) )
	{
		*reinterpret_cast< int* >( reinterpret_cast< DWORD >( pEntity ) + 0xA30 ) = Interfaces::GlobalVars->framecount; // *(_DWORD *)(v6 + 0xA30) = *(_DWORD *)(GlobalVars_0 + 4);
		*reinterpret_cast< int* >( reinterpret_cast< DWORD >( pEntity ) + 0xA28 ) = 0; // *(_DWORD *)(v6 + 0xA28) = 0;
	}

	memcpy( pEntity->m_pAnimOverlay( ), PushPlayer.AnimLayers, ( sizeof( CAnimationLayer ) * 15 ) ); // restore the correct animlayers

	BYTE BackUpProcess = *reinterpret_cast< BYTE* >( reinterpret_cast< DWORD >( pEntity ) +0x270 ); // backup extrabone shit flag
	*reinterpret_cast< BYTE* >( reinterpret_cast< DWORD >( pEntity ) + 0x270) &= ~1; // fuck bone processing && *(_BYTE *)(this + 0x274) & 1

	while ( !pEntity->SetupBones( pMatrix[ iIndex ], 128, 0x00000100, pEntity->m_flSimulationTime( ) ) ) { };

	*reinterpret_cast< int* >( reinterpret_cast< DWORD >( pEntity ) + 0xE4) = iSomeFlagsBackup; // *(_DWORD *)(v7 + 224 + 4) |= 8u
	*reinterpret_cast< BYTE* >( reinterpret_cast< DWORD >( pEntity ) + 0x270) = BackUpProcess; // && *(_BYTE *)(this + 0x274) & 1

	if ( pLocal && iIndex == pLocal->entindex( ) ) // flag backup
	{
		pEntity->m_fFlags( ) = iFlagsBackup;
		pEntity->m_vecVelocity( ) = vVelocityBackup;
		if ( GetKeyState( Menu::Config.tpkey ) )
			Interfaces::Prediction->SetLocalViewAngles( OldPredAng );
	}

//	pEntity->GetCollisionBounds( PushPlayer.Min, PushPlayer.Max );

	PushPlayer.Max = pEntity->GetCollideable( )->OBBMaxs( );
	PushPlayer.Min = pEntity->GetCollideable( )->OBBMins( );

	memcpy( PushPlayer.mMatrix, pMatrix[ iIndex ], ( sizeof( matrix3x4_t ) * 128 ) ); // copy bones
	PushPlayer.flSimulationTime = pEntity->m_flSimulationTime( );
	PushPlayer.bShot = bShot;
	PushPlayer.vVelocity = pEntity->m_vecVelocity( );
	PushPlayer.vEyeAngles = pEntity->m_angEyeAngles( );
	PushPlayer.vOrigin = pEntity->m_vecOrigin( );
	pOrigin[ iIndex ] = pEntity->m_vecOrigin( );
	PushPlayer.AnimState = AnimState;
	PushPlayer.flPosParam = pEntity->m_flPoseParameter();
	PushPlayer.flGoalFeetYaw = AnimState->m_flGoalFeetYaw;
	PushPlayer.iFlags = pEntity->m_fFlags( );
	PushPlayer.flLby = pEntity->m_flLowerBodyYaw( );

	PushPlayer.sCurrentResolve = Resolver::ResolverMode[ iIndex ];
	PushPlayer.iCurrentMode = Resolver::iMode[iIndex];

	PushPlayer.iBoneCount = pEntity->m_iBoneCount();

	aflOldCycle[ iIndex ] = PushPlayer.AnimLayers[ 6 ].m_flCycle;
	aflOldCurtime[ iIndex ] = Interfaces::GlobalVars->curtime;

	pPlayer[ iIndex ].push_back( PushPlayer );
}

void LagComp::RestoreRecord( CBaseEntity* pEntity, int iTick )
{
	int iIndex = pEntity->entindex( );

	if ( pPlayer[ iIndex ].size( ) <= 0 )
		return;

	if ( iTick >= pPlayer[ iIndex ].size( ) || iTick < 0 )
		return;

	Record Player = pPlayer[ iIndex ].at( iTick );

	auto pLocalEnt = Interfaces::User( );

	if ( !pLocalEnt || pLocalEnt != pEntity )
	{
		pEntity->m_fFlags( ) = Player.iFlags;

		pEntity->GetCollideable( )->OBBMaxs( ) = Player.Max;
		pEntity->GetCollideable( )->OBBMins( ) = Player.Min;

		pEntity->m_iBoneCount( ) = Player.iBoneCount;

		memcpy( pEntity->GetBoneCache( ).m_tBase( ), Player.mMatrix, ( sizeof( matrix3x4_t ) * pEntity->m_iBoneCount( ) ) );
		pEntity->m_vecVelocity( ) = Player.vVelocity;
		pEntity->SetAbsVelocity( Player.vVelocity );
		pEntity->m_angEyeAngles( ) = Player.vEyeAngles;
		pEntity->m_vecOrigin( ) = Player.vOrigin;
		pEntity->SetAbsOrigin( Player.vOrigin );
		pEntity->m_flLowerBodyYaw( ) = Player.flLby;
	}

	pEntity->m_flPoseParameter() = Player.flPosParam;

	pEntity->SetAbsAngles( Vec3D( 0, Player.flGoalFeetYaw, 0 ) );
	pEntity->m_pAnimState() = Player.AnimState;
	memcpy( pEntity->m_pAnimOverlay( ), Player.AnimLayers, ( sizeof( CAnimationLayer ) * 15 ) );
}

void LagComp::RemoveRecords( int iIndex )
{
	if ( pPlayer[ iIndex ].size( ) <= 0 )
		return;
	
	for ( int tick = 0; tick < pPlayer[ iIndex ].size( ); tick++ )
	{
		if ( !ValidTick( pPlayer[ iIndex ].at( tick ).flSimulationTime ) )
		{
			pPlayer[ iIndex ].erase( pPlayer[ iIndex ].begin( ) + tick );
		}
		else 
		{
			if (pPlayer[iIndex].at(tick).bShot)
				aiTickOnShot[iIndex] = tick;
			if (pPlayer[iIndex].at(tick).bLbyUpdated)
				aiTickLby[iIndex] = tick;
		}
	}
}

void LagComp::ClearRecords( int iIndex )
{
	if ( pPlayer[ iIndex ].size( ) > 0 )
		pPlayer[ iIndex ].clear( );

	aflOldSimulationTime[ iIndex ] = 0.f;
	aiTickOnShot[ iIndex ] = -1;
	aiTickLby[iIndex] = -1;
	aflOldCurtime[ iIndex ] = 0;
}