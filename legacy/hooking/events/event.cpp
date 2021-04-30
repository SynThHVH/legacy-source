#include "../../features/features.hpp"
#include "../../utilities/bytes.hpp"

CEvent Event;

void CEvent::FireGameEvent( IGameEvent* pEvent )
{
	if ( !pEvent )
		return;

	auto pLocalEnt = Interfaces::User( );

	int iIndex = Interfaces::Engine->GetPlayerForUserID( pEvent->GetInt( "userid" ) );

	if ( !pLocalEnt || !pLocalEnt->m_bIsAlive( ) )
		return;

	if ( !( iIndex > 0 && iIndex < 65 ) )
		return;

	if ( !strcmp( pEvent->GetName( ), "player_hurt" ) )
	{
		auto pAttacker = Interfaces::EntityList->m_pGetClientEntity( Interfaces::Engine->GetPlayerForUserID( pEvent->GetInt( "attacker" ) ) );

		if ( pAttacker )
		{
			if ( pAttacker == pLocalEnt )
			{
				Globals::bHit = true;
				Globals::bShot = true;

				PlayerInfo_t pInfo;
				Interfaces::Engine->GetPlayerInfo( iIndex, &pInfo );

				std::string Message = "Hit: " + std::string( pInfo.m_cName ) + " for (" + std::to_string( pEvent->GetInt( "dmg_health" ) ) + ") damage " + "\n";
				//	Globals::Msg( Message.c_str( ) );
				Menu::PopUp.push_back( { Message,  Color( 0.f,255.f,0.f,255.f ), Interfaces::GlobalVars->realtime + 3.f } );

				Interfaces::Engine->ExecuteClientCmd( bubble );

				if ( Globals::Bullet.size( ) > 0 )
				{
					Globals::Bullet.at( Globals::Bullet.size( ) - 1 ).HitPlayer = true;
				}
			}
		}	
	}

	static float flLastShotTime = 0.f;
	if ( !strcmp( pEvent->GetName( ), "bullet_impact" ) ) // basic 
	{
		if ( Interfaces::Engine->GetLocalPlayer( ) == iIndex && (Interfaces::GlobalVars->realtime - flLastShotTime) > 0.065f )
		{
			BulletHit Bullet;

			Vec3D ShootPos = Vec3D( LagComp::pOrigin[ iIndex ].x, LagComp::pOrigin[ iIndex ].y, pLocalEnt->GetEyePosition().z);

			Bullet.ShootPos = ShootPos;
			Bullet.Hit = Vec3D( pEvent->GetFloat( "x" ), pEvent->GetFloat( "y" ), pEvent->GetFloat( "z" ) );
			Bullet.timeShot = Interfaces::GlobalVars->realtime;

			flLastShotTime = Interfaces::GlobalVars->realtime;

			Vec3D ViewAngles, ViewForward;
			ViewAngles = Math::CalcAngle( Bullet.ShootPos, Bullet.Hit );

			Math::AngleVectors( ViewAngles, &ViewForward );
			Math::NormalizeAngles( ViewForward );

			Bullet.ShootPos = Bullet.ShootPos + ( ViewForward * 15 );

			Globals::Bullet.push_back( Bullet );
		}
	}
}