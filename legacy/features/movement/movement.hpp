#pragma once

namespace Movement
{
	// Functions
	void Bunnyhop( CUserCmd* pCmd );
	void AutoStrafe( CUserCmd* pCmd );

	// Variables
	static float m_flTrueDirection,
		m_flRoughDirection,
		m_flWishDirection,
		m_flCalculatedDirection,
		m_flStep;

	static bool m_bInTransition,
		m_bIsBhopping;
}