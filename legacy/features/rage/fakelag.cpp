#include "../features.hpp"

/* note: never touched the fakelag, never felt a reason too lmao */

void Rage::FakeLag( )
{
	if ( !Interfaces::Engine->IsInGame( ) || !Interfaces::Engine->IsConnected( ) )
		return;

	if ( Menu::Config.legitback )
		return;

	if ( !Menu::Config.fakelag )
		return;

	auto pLocalEnt = Interfaces::User( );

	if ( !pLocalEnt || !pLocalEnt->m_bIsAlive( ) )
		return;

	auto NetChannel = Interfaces::Engine->GetNetChannel( );

	if ( !NetChannel )
		return;

	static bool Hitinit = false;
	bool SkipTick = false;
	static bool canHit = false;
	bool HitCheck = false;
	static bool LagPeek = false;

	static float LagTime = TICKS_TO_TIME( 14.f );

	if ( Menu::Config.onpeek )
	{
		Vec3D Head = Vec3D( pLocalEnt->m_vecOrigin( ).x, pLocalEnt->m_vecOrigin( ).y, ( pLocalEnt->GetHitboxPosition( 0, LagComp::pMatrix[ pLocalEnt->entindex( ) ] ).z + 10.f ) );
		Vec3D HeadExtr = ( Head + ( pLocalEnt->m_vecVelocity( ) * LagTime ) );
		Vec3D OriginExtr = ( ( pLocalEnt->m_vecOrigin( ) + ( pLocalEnt->m_vecVelocity( ) * LagTime ) ) + Vec3D( 0, 0, 8.f ) );

		for ( int index = 1; index < MAX_NETWORKID_LENGTH; ++index )
		{
			CBaseEntity* pPlayerEntity = Interfaces::EntityList->m_pGetClientEntity( index );

			if ( !pPlayerEntity
				
				|| !pPlayerEntity->m_bIsAlive( )
				|| pPlayerEntity->IsDormant( )
				|| pLocalEnt->GetTeam( ) == pPlayerEntity->GetTeam( ) )
				continue;

			Vec3D EnemyHead = Vec3D( pPlayerEntity->m_vecOrigin( ).x, pPlayerEntity->m_vecOrigin( ).y, ( pPlayerEntity->GetHitboxPosition( 0, LagComp::pMatrix[ pPlayerEntity->entindex( ) ] ).z + 10.f ) );

			if ( fabs( pLocalEnt->m_vecVelocity( ).m_flLength2D( ) ) > 2.5f && ( Autowall::CanHitFloatingPoint( HeadExtr, EnemyHead ) || Autowall::CanHitFloatingPoint( OriginExtr, EnemyHead ) ) )
			{
				if ( !Hitinit )
				{
					canHit = true;
					Hitinit = true;
				}
				HitCheck = true;
			}
		}

		if ( !HitCheck )
			Hitinit = false;

		if ( canHit )
		{
			Globals::bSendPacket = true;
			SkipTick = true;
			LagPeek = true;
			canHit = false;
		}

		if ( LagPeek && !SkipTick )
		{
			if ( NetChannel->m_iChokedPackets < 14 )
				Globals::bSendPacket = false;
			else
				LagPeek = false;
		}

		if ( !LagPeek && !SkipTick )
		{
			Globals::bSendPacket = ( NetChannel->m_iChokedPackets >= Menu::Config.fakelagamnt );
		}
	}
	else
	{
		Hitinit = false;
		SkipTick = false;
		canHit = false;
		LagPeek = false;

		Globals::bSendPacket = ( NetChannel->m_iChokedPackets >= Menu::Config.fakelagamnt );
	}
}