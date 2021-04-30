#include "../features.hpp"

/* note: the movement features aren't too bad so that's another good thing to paste lmao */

void Movement::Bunnyhop( CUserCmd* pCmd )
{
	if ( !Menu::Config.bhop )
		return;

	if ( !( pCmd->buttons & IN_JUMP ) )
	{
		return;
	}

	auto pLocalEnt = Interfaces::User( );

	if ( !pLocalEnt || !pLocalEnt->m_bIsAlive( ) )
	{
		return;
	}

	int iType = pLocalEnt->m_iMoveType( );
	if ( iType == MOVETYPE_LADDER || iType == MOVETYPE_NOCLIP )
	{
		return;
	}

	pCmd->buttons &= ~IN_JUMP;

	if ( pLocalEnt->m_fFlags( ) & FL_ONGROUND )
	{
		pCmd->buttons |= IN_JUMP;
	}
}

void Movement::AutoStrafe( CUserCmd *pCmd )
{
	if ( !Menu::Config.strafer )
		return;

	auto pLocalEnt = Interfaces::User( );

	if ( !pLocalEnt || !pLocalEnt->m_bIsAlive( ) )
	{
		return;
	}

	enum Directions 
	{
		FORWARDS = 0,
		BACKWARDS = 180,
		LEFT = 90,
		RIGHT = -90,
		QUADRANT1 = 45,
		QUADRANT2 = -45,
		QUADRANT3 = -135,
		QUADRANT4 = 135
	};

	m_bIsBhopping = pCmd->buttons & IN_JUMP;
	if ( !m_bIsBhopping && pLocalEnt->m_fFlags( ) & FL_ONGROUND )
	{
		m_flCalculatedDirection = Directions::FORWARDS;
		m_bInTransition = false;
		return;
	}

	Vec3D vBase{ };
	Interfaces::Engine->GetViewAngles( vBase );

	auto flGetRoughDirection = [ & ]( float flTrueDirection ) -> float
	{
		std::array< float, 8 > uMinimum = { Directions::FORWARDS, Directions::BACKWARDS, Directions::LEFT, Directions::RIGHT, Directions::QUADRANT1, Directions::QUADRANT2, Directions::QUADRANT3, Directions::QUADRANT4 };
		float flBestAngle, flBestDelta = 181.f;

		for ( size_t i = 0; i < uMinimum.size( ); ++i ) 
		{
			float flRoughDirection = vBase.y + uMinimum.at( i );
			float flDelta = fabsf( Math::m_flNormalizeYaw( flTrueDirection - flRoughDirection ) );

			if ( flDelta < flBestDelta )
			{
				flBestAngle = flRoughDirection;
				flBestDelta = flDelta;
			}
		}

		return flBestAngle;
	};

	m_flTrueDirection = pLocalEnt->m_vecVelocity( ).m_vAngle( ).y;

	if ( ( pCmd->buttons & IN_FORWARD ) && !( pCmd->buttons & IN_MOVELEFT ) && !( pCmd->buttons & IN_MOVERIGHT ) )
	{
		m_flWishDirection = vBase.y + Directions::FORWARDS;
	}
	else if ( ( pCmd->buttons & IN_BACK ) && !( pCmd->buttons & IN_MOVELEFT ) && !( pCmd->buttons & IN_MOVERIGHT ) )
	{
		m_flWishDirection = vBase.y + Directions::BACKWARDS;
	}
	else if ( ( pCmd->buttons & IN_MOVELEFT ) && !( pCmd->buttons & IN_FORWARD ) && !( pCmd->buttons & IN_BACK ) )
	{
		m_flWishDirection = vBase.y + Directions::LEFT;
	}
	else if ( ( pCmd->buttons & IN_MOVERIGHT ) && !( pCmd->buttons & IN_FORWARD ) && !( pCmd->buttons & IN_BACK ) )
	{
		m_flWishDirection = vBase.y + Directions::RIGHT;
	}
	else if ( ( pCmd->buttons & IN_FORWARD ) && ( pCmd->buttons & IN_MOVELEFT ) )
	{
		m_flWishDirection = vBase.y + Directions::QUADRANT1;
	}
	else if ( ( pCmd->buttons & IN_FORWARD ) && ( pCmd->buttons & IN_MOVERIGHT ) )
	{
		m_flWishDirection = vBase.y + Directions::QUADRANT2;
	}
	else if ( ( pCmd->buttons & IN_BACK ) && ( pCmd->buttons & IN_MOVERIGHT ) )
	{
		m_flWishDirection = vBase.y + Directions::QUADRANT3;
	}
	else if ( ( pCmd->buttons & IN_BACK ) && ( pCmd->buttons & IN_MOVELEFT ) )
	{
		m_flWishDirection = vBase.y + Directions::QUADRANT4;
	}
	else 
	{
		pCmd->buttons |= IN_FORWARD;
		m_flWishDirection = vBase.y + Directions::FORWARDS;
	}

	// TODO: Fix clamping
	float flSpeedRotation = min( Math::m_flRadToDeg( std::asinf( 30.f / pLocalEnt->m_vecVelocity( ).m_flLength2D( ) ) ) * 0.5f, 45.f );
	if ( m_bInTransition ) 
	{
		// Unused, might make an option in future if improved
		float flIdealStep = flSpeedRotation + m_flCalculatedDirection;
		m_flStep = fabsf( Math::m_flNormalizeYaw( m_flCalculatedDirection - flIdealStep ) );

		if ( fabsf( Math::m_flNormalizeYaw( m_flWishDirection - m_flCalculatedDirection ) ) > m_flStep ) 
		{
			float flAdd = Math::m_flNormalizeYaw( m_flCalculatedDirection + m_flStep );
			float flSub = Math::m_flNormalizeYaw( m_flCalculatedDirection - m_flStep );

			if ( fabsf( Math::m_flNormalizeYaw( m_flWishDirection - flAdd ) ) >= fabsf( Math::m_flNormalizeYaw( m_flWishDirection - flSub ) ) )
			{
				m_flCalculatedDirection -= m_flStep;
			}
			else {
				m_flCalculatedDirection += m_flStep;
			}
		}
		else 
		{
			m_bInTransition = false;
		}
	}
	else 
	{
		m_flRoughDirection = flGetRoughDirection( m_flTrueDirection );
		m_flCalculatedDirection = m_flRoughDirection;

		if ( m_flRoughDirection != m_flWishDirection )
		{
			m_bInTransition = true;
		}
	}

	pCmd->forwardmove = 0.f;
	pCmd->sidemove = pCmd->command_number % 2 ? 450.f : -450.f;

	float flDirection = ( pCmd->command_number % 2 ? flSpeedRotation : -flSpeedRotation ) + m_flCalculatedDirection;

	float rotation = Math::m_flDegToRad( vBase.y - flDirection );

	float flCosRotation = cos( rotation );
	float flSinRotation = sin( rotation );

	float flForwardmove = ( flCosRotation * pCmd->forwardmove ) - ( flSinRotation * pCmd->sidemove );
	float flSidemove = ( flSinRotation * pCmd->forwardmove ) + ( flCosRotation * pCmd->sidemove );

	pCmd->forwardmove = flForwardmove;
	pCmd->sidemove = flSidemove;
}