#include "../features.hpp"

/* note: i honestly have no fucking clue why this is here, shits dumb asf lmao */

void Legit::Backtrack( CUserCmd* pCmd ) // best legit backtrack ever xd
{
	if ( !Menu::Config.legitback )
		return;

	if ( !Interfaces::Engine->IsInGame( ) )
		return;

	auto pLocalEnt = Interfaces::User( );

	if ( !pLocalEnt || !pLocalEnt->m_bIsAlive( ) )
		return;

	if ( !pLocalEnt->GetActiveWeapon( ) )
		return;

	pLocalEnt->GetActiveWeapon( )->m_UpdateAccuracyPenalty( );

	static int Height, Width;
	Interfaces::Engine->GetScreenSize( Width, Height );

	Vec3D punchAngle = pLocalEnt->m_aimPunchAngle( );

	float x = Width / 2;
	float y = Height / 2;

	static int dy = Height / 90;
	static int dx = Width / 90;
	x -= ( dx*( punchAngle.y ) );
	y += ( dy*( punchAngle.x ) );

	Vec2D screenPunch = Vec2D( x, y );

	int ClosestDistance = 9999;
	float BestSimTime = 0.f;
	CBaseEntity* BestEnt = nullptr;

	for ( int index = 1; index < MAX_NETWORKID_LENGTH; ++index )
	{
		CBaseEntity* pPlayerEntity = Interfaces::EntityList->m_pGetClientEntity( index );

		if ( !pPlayerEntity
			|| !pPlayerEntity->m_bIsAlive( )
			|| pPlayerEntity->IsDormant( )
			|| pPlayerEntity == pLocalEnt
			|| pPlayerEntity->GetTeam( ) == pLocalEnt->GetTeam( ) )
			continue;

		if ( LagComp::pPlayer[ index ].size( ) > 0 )
		{
			for ( int tick = 0; tick < LagComp::pPlayer[ index ].size( ); tick++ )
			{
				Vec3D HeadW2S;
				if ( !Interfaces::DebugOverlay->m_bWorldToScreen( LagComp::pPlayer[ index ].at( tick ).mMatrix[ 8 ].GetOrigin( ), HeadW2S ) )
					continue;

				if ( abs( Math::Distance2D( Vec2D( HeadW2S.x, HeadW2S.y ), screenPunch ) ) < ClosestDistance )
				{
					BestEnt = pPlayerEntity;
					BestSimTime = LagComp::pPlayer[ index ].at( tick ).flSimulationTime;
					ClosestDistance = abs( Math::Distance2D( Vec2D( HeadW2S.x, HeadW2S.y ), screenPunch ) );
				}
			}
		}
	}

	float flServerTime = float( pLocalEnt->m_iTickBase( )) * Interfaces::GlobalVars->interval_per_tick;
	bool bCanShoot = ( pLocalEnt->GetActiveWeapon( )->m_flNextPrimaryAttack( ) <= flServerTime );

	if (( pCmd->buttons & IN_ATTACK ) && BestSimTime != 0.f && bCanShoot )
		pCmd->tick_count = TIME_TO_TICKS( BestSimTime + LagComp::LerpTime( ) );
}
