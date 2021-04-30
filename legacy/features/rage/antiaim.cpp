#include "../features.hpp"

/*
note:
	thanks bulber for this bullshit, will make it less pasted though :sunglasses:
	i literally made this comment at first before i looked at this code
	and before i changed this shit this nigga really had moving and air at target conditions,
	moving and air fakes,
	and moving and air spin speeds nigga like what the fuck are u doing bro
	i really hope that wasn't bolbi's doing and it was fucking acupz or whatever that lispers name is that pasted the bameware legacy source did it lmao
	also im not adding edge fuck that shit i dont need it
	+ wtf was that shit with the picking up weapons nigga just disable it on use its not 2020
*/

Antiaim antiaim;

void Antiaim::Do(CUserCmd* cmd)
{
	auto local_player = Interfaces::User();
	if (!local_player || local_player->m_iHealth() <= 0 || local_player->m_iMoveType() == MOVETYPE_NOCLIP || local_player->m_iMoveType() == MOVETYPE_LADDER || !Menu::Config.antiaim) return;

	auto pWeapon = local_player->GetActiveWeapon();
	if (!pWeapon) return;

	if (pWeapon->IsKnifeorNade())
	{
		Globals::bSendPacket = true; /* note: yeah nigga whats up now bitch u aint expectin that shit */
		return;
	}

	if ((cmd->buttons & IN_USE) || (cmd->buttons & IN_ATTACK)) return;

	cmd->viewangles.x = DoPitch();
	cmd->viewangles.y = DoYaw();

	cmd->viewangles = Math::NormalizeAngle(cmd->viewangles);
}

void Antiaim::UpdateLBY()
{
	auto local_player = Interfaces::User();
	if (!local_player || !(local_player->m_fFlags() & FL_ONGROUND)) return;

	auto animstate = local_player->m_pAnimState();
	if (!animstate) return;

	auto net_channel = Interfaces::Engine->GetNetChannel();
	if (!net_channel || net_channel->m_iChokedPackets) return;

	const float curtime = GetCurtime();

	if (animstate->speed_2d > 0.1 && !Globals::bIsFakeWalking)
	{
		flNextLBYUpdateTime = curtime + 0.22;
		flLastMoveTime = curtime;
	}

	if (static float last_lby; last_lby != local_player->m_flLowerBodyYaw())
	{
		last_lby = local_player->m_flLowerBodyYaw();
		if (fabs(curtime - flLastMoveTime) > 0.3 && fabs(Math::m_flNormalizeYaw(local_player->m_flLowerBodyYaw() - flLastAttemptedLBY)) > 35.f)
			flNextLBYUpdateTime = curtime + 1.1 + Interfaces::GlobalVars->interval_per_tick;
	}

	if (flNextLBYUpdateTime < curtime)
	{
		flNextLBYUpdateTime = curtime + 1.1;
		bWillLBYUpdate = true;
	}
}

float Antiaim::BreakLBY(float real_yaw, float desired_lby, bool double_flick)
{
	auto local_player = Interfaces::User();
	if (!local_player)
		return real_yaw;

	desired_lby = Math::GetYawDelta(desired_lby, local_player->m_flLowerBodyYaw()) < 35.f ? local_player->m_flLowerBodyYaw() : desired_lby;
	if (bWillLBYUpdate)
	{
		static bool switch_bool = false;
		switch_bool = !switch_bool;
		if (const float lby_delta = Math::m_flNormalizeYaw(desired_lby - real_yaw); fabs(lby_delta) < 100.f)
			desired_lby = real_yaw + (switch_bool ? lby_delta : -lby_delta);

		bWillLBYUpdate = false;
		flLastAttemptedLBY = desired_lby;
		return desired_lby;
	}

	if (fabs(Math::m_flNormalizeYaw(real_yaw - desired_lby)) > 130.f && fabs(Math::m_flNormalizeYaw(desired_lby - local_player->m_flLowerBodyYaw())) < 35.f)
	{
		if (fabs(GetCurtime() - flNextLBYUpdateTime) < Interfaces::GlobalVars->interval_per_tick)
			return desired_lby - 115.f;
		if (fabs(GetCurtime() - flNextLBYUpdateTime) > 1.1 - (Interfaces::GlobalVars->interval_per_tick * 5))
			return desired_lby + 115.f;
	}

	return real_yaw;
}

float Antiaim::DoYaw()
{
	auto local_player = Interfaces::User();
	if (!local_player)
		return 0.f;

	bool is_jumping = !(local_player->m_fFlags() & FL_ONGROUND);
	bool is_moving = (local_player->m_vecVelocity().m_flLength2D() > 0.1f) && !is_jumping && !Globals::bIsFakeWalking;

	/// returns viewangles/at target viewangles
	auto GetViewangles = [local_player](bool at_target_enabled) -> float
	{
		Vec3D viewangles;
		Interfaces::Engine->GetViewAngles(viewangles);

		if (!at_target_enabled)
			return viewangles.y + 180.f;

		/// get at target based on closest fov
		float closest_fov = 360.f, closest_fov_yaw = 181.f;
		for (int i = 0; i < MAX_NETWORKID_LENGTH; i++)
		{
			CBaseEntity* entity = Interfaces::EntityList->m_pGetClientEntity(i);
			if (!entity || entity->m_iHealth() <= 0 || entity->GetTeam() == local_player->GetTeam() || entity->IsDormant())
				continue;

			const float current_yaw = Math::CalcAngle(local_player->m_vecOrigin(), entity->m_vecOrigin()).y;
			const float current_fov = fabs(Math::m_flNormalizeYaw(current_yaw - viewangles.y));
			if (current_fov < closest_fov)
			{
				closest_fov = current_fov;
				closest_fov_yaw = current_yaw;
			}
		}

		/// couldn't find a valid entity
		if (closest_fov == 360.f)
			return viewangles.y;

		return closest_fov_yaw;
	};

	/// returns the base real angle from the combobox setting
	auto GetBaseRealAngle = [local_player, GetViewangles](int setting, bool at_target_enabled, float spin_speed) -> float
	{
		/// base yaw
		switch (setting)
		{
		case 0: /// viewangle
			return local_player->m_angEyeAngles().y;
		case 1: /// 180
			return GetViewangles(at_target_enabled);
		case 2: /// lby
			return local_player->m_flLowerBodyYaw();
		case 3: /// spin
			return GetCurtime() * 7.5f * 100.f;
		}

		return 0.f;
	};

	/// returns the base fake angle from the combobox setting
	auto GetBaseFakeAngle = [local_player, GetViewangles](int setting, bool at_target_enabled, float spin_speed) -> float
	{
		/// base yaw
		switch (setting)
		{
		case 0: /// viewangle
			return local_player->m_angEyeAngles().y;
		case 1: /// 180
			return GetViewangles(at_target_enabled);
		case 2: /// lby
			return local_player->m_flLowerBodyYaw();
		case 3: /// lby inverse
			return local_player->m_flLowerBodyYaw() + 180.f;
		case 4: /// spin
			return GetCurtime() * 7.5f * 100.f;
		}

		return 0.f;
	};

	auto DoRealYaw = [this, is_jumping, is_moving, local_player, GetViewangles, GetBaseRealAngle]() -> float
	{
		float real_yaw = GetViewangles(false), lby = 0.f;

		if (is_jumping)
		{
			real_yaw = GetBaseRealAngle(Menu::Config.airyaw, Menu::Config.attargets, 0.f);

			/// offset
			if (Menu::Config.airyaw == 3) /// adaptive
				real_yaw += Math::m_flNormalizeYaw(GetViewangles(false) - GetViewangles(true)) > 0.f ? 0.f : -0.f;
			else
				real_yaw += 0.f;
		}
		else if (is_moving)
		{
			real_yaw = GetBaseRealAngle(Menu::Config.movingyaw, Menu::Config.attargets, 0.f);

			/// offset
			if (Menu::Config.movingyaw == 3) /// adaptive
				real_yaw += Math::m_flNormalizeYaw(GetViewangles(false) - GetViewangles(true)) > 0.f ? 0.f : -0.f;
			else
				real_yaw += 0.f;

		}
		else /// standing
		{
			real_yaw = GetBaseRealAngle(Menu::Config.realyaw, Menu::Config.attargets, 0.f);

			/// offset
			if (Menu::Config.realyaw == 3) /// adaptive
				real_yaw += Math::m_flNormalizeYaw(GetViewangles(false) - GetViewangles(true)) > 0.f ? 0.f : -0.f;
			else
				real_yaw += 0.f;

			/// make sure to do this before we add the jitter shit
			lby = real_yaw + 111.5f;
		}

		return BreakLBY(real_yaw, lby);
	};

	auto DoFakeYaw = [this, is_jumping, is_moving, local_player, GetViewangles, GetBaseFakeAngle]() -> float
	{
		float fake_yaw = GetViewangles(false);

		fake_yaw = GetBaseFakeAngle(Menu::Config.fakeyaw, Menu::Config.attargets, 0.f);

		/// offset
		if (Menu::Config.fakeyaw == 3) /// adaptive
			fake_yaw += Math::m_flNormalizeYaw(GetViewangles(false) - GetViewangles(true)) > 0.f ? 0.f : -0.f;
		else
			fake_yaw += 0.f;

		/// jitter range
		fake_yaw += Math::RandomFloat(0.f * -0.5f, 0.f * 0.5f);

		return fake_yaw;
	};

	return Globals::bSendPacket ? DoFakeYaw() : DoRealYaw();
}

float Antiaim::DoPitch()
{
	auto local_player = Interfaces::User();
	if (!local_player)
		return 0.f;

	auto GetPitchFromSetting = [](int setting) -> float
	{
		Vec3D viewangles;
		Interfaces::Engine->GetViewAngles(viewangles);

		switch (setting)
		{
		case 0:
			return viewangles.x;
		case 1:
			return 89.f;
		case 2:
			return -89.f;
		default:
			return Math::RandomFloat(-89.f, 89.f);
		}
	};

	return GetPitchFromSetting(Menu::Config.pitch);
}

/* note: dogshit ass old aas, left them here in case anyone wants to base off of those instead because the new ones have some problems */
/*
Vec3D vRealAngle = Vec3D(0,0,0);
bool FreestandingSide;

void FreeStanding( CUserCmd* pCmd, CBaseEntity* pLocal) // should have rewritten
{
	static float FinalAngle;
	bool bside1 = false;
	bool bside2 = false;
	bool autowalld = false;

//	float tempdis = 999999999.f;
	Vec2D LocalOrg = Vec2D( pLocal->m_vecOrigin( ).x, pLocal->m_vecOrigin( ).y );

//	float closeYaw = 999.f;

	for ( int index = 1; index < MAX_NETWORKID_LENGTH; ++index )
	{
		CBaseEntity* pPlayerEntity = Interfaces::EntityList->m_pGetClientEntity( index );

		if ( !pPlayerEntity
			
			|| !pPlayerEntity->m_bIsAlive( )
			|| pPlayerEntity->IsDormant( )
			|| pLocal->GetTeam( ) == pPlayerEntity->GetTeam( ) )
			continue;

		Vec2D EnemyOrg = Vec2D( pPlayerEntity->m_vecOrigin( ).x, pPlayerEntity->m_vecOrigin( ).y );

	//	if ( tempdis > fabs( Math::Distance2D( EnemyOrg, LocalOrg ) ) )
	//	{
	//		closeYaw = Math::m_flNormalizeYaw( Math::CalcAngle( pLocal->m_vecOrigin( ), pPlayerEntity->m_vecOrigin( ) ).y );
	//		tempdis = fabs( Math::Distance2D( EnemyOrg, LocalOrg ) );
	//	}

		float angToLocal = Math::CalcAngle( pLocal->m_vecOrigin( ), pPlayerEntity->m_vecOrigin( ) ).y;
		Vec3D ViewPoint = pPlayerEntity->m_vecOrigin( ) + Vec3D( 0, 0, 90 );

		Vec2D Side1 = Vec2D(( 45 * sin( Math::m_flDegToRad( angToLocal ) ) ),( 45 * cos( Math::m_flDegToRad( angToLocal ) ) ) );
		Vec2D Side2 = Vec2D(( 45 * sin( Math::m_flDegToRad( angToLocal + 180 ) ) ) ,( 45 * cos( Math::m_flDegToRad( angToLocal + 180 ) ) ) );

		Vec2D Side3 = Vec2D(( 50 * sin( Math::m_flDegToRad( angToLocal ) ) ),( 50 * cos( Math::m_flDegToRad( angToLocal ) ) ) );
		Vec2D Side4 = Vec2D(( 50 * sin( Math::m_flDegToRad( angToLocal + 180 ) ) ) ,( 50 * cos( Math::m_flDegToRad( angToLocal + 180 ) ) ) );

		Vec3D Origin = pLocal->m_vecOrigin( );

		Vec2D OriginLeftRight[ ] = { Vec2D( Side1.x, Side1.y ), Vec2D( Side2.x, Side2.y ) };

		Vec2D OriginLeftRightLocal[ ] = { Vec2D( Side3.x, Side3.y ), Vec2D( Side4.x, Side4.y ) };

		for ( int side = 0; side < 2; side++ )
		{
			Vec3D OriginAutowall = Vec3D( Origin.x + OriginLeftRight[ side ].x,  Origin.y - OriginLeftRight[ side ].y , Origin.z + 80 );
			Vec3D OriginAutowall2 = Vec3D( ViewPoint.x + OriginLeftRightLocal[ side ].x,  ViewPoint.y - OriginLeftRightLocal[ side ].y , ViewPoint.z );

			if ( Autowall::CanHitFloatingPoint( OriginAutowall, ViewPoint, pPlayerEntity ) )
			{
				if ( side == 0 )
				{
					FreestandingSide = true;
					bside1 = true;
					FinalAngle = angToLocal + 90;
				}
				else if ( side == 1 )
				{
					FreestandingSide = false;
					bside2 = true;
					FinalAngle = angToLocal - 90;
				}
				autowalld = true;
			}
			else
			{
				for ( int side222 = 0; side222 < 2; side222++ )
				{
					Vec3D OriginAutowall222 = Vec3D( Origin.x + OriginLeftRight[ side222 ].x,  Origin.y - OriginLeftRight[ side222 ].y , Origin.z + 80 );

					if ( Autowall::CanHitFloatingPoint( OriginAutowall222, OriginAutowall2, pPlayerEntity ) )
					{
						if ( side222 == 0 )
						{
							FreestandingSide = true;
							bside1 = true;
							FinalAngle = angToLocal + 90;
						}
						else if ( side222 == 1 )
						{
							FreestandingSide = false;
							bside2 = true;
							FinalAngle = angToLocal - 90;
						}
						autowalld = true;
					}
				}
			}
		}
	}

	if ( !autowalld || ( bside1 && bside2 ) )
	{
	//	if ( closeYaw != 999.f && Menu.Config.AtTarget )
	//		g::pCmd->viewangles.y = g_Math.NormalizeYaw( closeYaw + SentYaw );
	//	else
		//	g::pCmd->viewangles.y += SentYaw;
		pCmd->viewangles.y += 180.f;
	}
	else
		pCmd->viewangles.y = Math::m_flNormalizeYaw( FinalAngle );
}


void Rage::AntiAim( CUserCmd* pCmd )
{
	if ( !Interfaces::Engine->IsInGame( ) || !Interfaces::Engine->IsConnected( ) )
		return;

	if ( Menu::Config.legitback )
		return;

	if ( !Menu::Config.antiaim )
		return;

	auto pLocalEnt = Interfaces::User( );

	if ( !pLocalEnt || !pLocalEnt->m_bIsAlive( ) )
		return;

	auto pWeapon = pLocalEnt->GetActiveWeapon( );

	if ( !pWeapon )
		return;

	if ( pWeapon->IsKnifeorNade( ) )
	{
		Globals::bSendPacket = true;
		return;
	}

	if ( pCmd->buttons & IN_ATTACK )
		return;

	if ( pCmd->buttons & IN_USE )
	{
		Globals::bSendPacket = true;
		return;
	}

	pCmd->viewangles.x = 89.99999f;


	FreeStanding( pCmd, pLocalEnt );

	bool bCommandSwitch = pCmd->command_number % 2;

	if ( Menu::Config.desync ) // ew wtf
	{
		if ( Menu::Config.desyncmove < 2 )
		{
			float &flMoveType = ((Menu::Config.desyncmove) ? pCmd->sidemove : pCmd->forwardmove);

			if ( !( GetAsyncKeyState( VK_SPACE ) ) && !( pCmd->buttons & IN_MOVELEFT ) && !( pCmd->buttons & IN_MOVELEFT ) && !( pCmd->buttons & IN_MOVERIGHT ) && !( pCmd->buttons & IN_FORWARD ) && !( pCmd->buttons & IN_BACK ) )
				if ( pCmd->buttons & IN_DUCK )
					( bCommandSwitch ) ? flMoveType += -4.6 : flMoveType += 4.6;
				else
					( bCommandSwitch ) ? flMoveType += -1.6 : flMoveType += 1.6;
		}
		else
		{
			static bool bSwitch = false;
			if ( bCommandSwitch )
				bSwitch = !bSwitch;
			float &flMoveType = ( ( bSwitch ) ? pCmd->sidemove : pCmd->forwardmove );
			if ( !( GetAsyncKeyState( VK_SPACE ) ) && !( pCmd->buttons & IN_MOVELEFT ) && !( pCmd->buttons & IN_MOVELEFT ) && !( pCmd->buttons & IN_MOVERIGHT ) && !( pCmd->buttons & IN_FORWARD ) && !( pCmd->buttons & IN_BACK ) )
				if ( pCmd->buttons & IN_DUCK )
					( bCommandSwitch ) ? flMoveType += -4.6 : flMoveType += 4.6;
				else
					( bCommandSwitch ) ? flMoveType += -1.6 : flMoveType += 1.6;
		}

		if ( !Globals::bSendPacket )
		{
			pCmd->viewangles.y += Menu::Config.desyncoffset;
		}
	}
}
*/