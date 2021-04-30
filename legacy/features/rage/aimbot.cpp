#include "../features.hpp"

float Rage::flSimulationTime = 0.f;
float Rage::flTempDmg = 0.f;
int Rage::iTickUsed = -1;
bool Rage::bBacktracking = false;
CBaseEntity* Rage::pLocalEnt = nullptr;
std::deque<AimbotShotInfo_t> Rage::ShotInfo;
std::pair<matrix3x4_t*, int> Rage::iTargetMatrix;

std::deque<PointInfo_t> Rage::IteratePoints( CBaseEntity* pEntity, matrix3x4_t* pMatrix, bool bBacktrack )
{
	std::deque<PointInfo_t> Points;

	Vec3D flAngToLocal = Math::CalcAngle( pLocalEnt->m_vecOrigin( ), pEntity->m_vecOrigin( ) );

	Vec3D Forward, Forward1, Up, Down;

	Math::AngleVectors( Vec3D( 0.f, Math::m_flNormalizeYaw( flAngToLocal.y + 90.f ), 0.f ), &Forward );
	Math::NormalizeAngles( Forward );
	Math::AngleVectors( Vec3D( 0.f, Math::m_flNormalizeYaw( flAngToLocal.y - 90.f ), 0.f ), &Forward1 );
	Math::NormalizeAngles( Forward1 );
	Math::AngleVectors( Vec3D( flAngToLocal.x - 90.f, Math::m_flNormalizeYaw( flAngToLocal.y ), 0.f ), &Up );
	Math::NormalizeAngles( Up );
	Math::AngleVectors( Vec3D( flAngToLocal.x + 90.f, Math::m_flNormalizeYaw( flAngToLocal.y ), 0.f ), &Down );
	Math::NormalizeAngles( Down );

	Vec3D Mutipoint[ ] = { Forward, Forward1 };

	int iIndex = 0;
	int iLastMainHitbox = 0;

	auto Hitbox = [ & ]( int iHitbox, EMultiPointMode bMultiPoint = NONE )
	{
		float flRadius = 0.f;

		PointInfo_t Info_t;

		if ( !bMultiPoint )
		{
			Info_t.vPos = pEntity->GetHitboxPosition( iHitbox, pMatrix );
			Info_t.bMultiPoint = bMultiPoint;
			Info_t.iMainHitbox = iHitbox;
			Info_t.iMainIndex = iIndex;
			iLastMainHitbox = iIndex;
			Info_t.iIndex = iIndex++;
			Info_t.flDamage = 0.f;
			return Info_t;
		}
		else
		{
			Vec3D vHitBoxPos = pEntity->GetHitboxPosition( iHitbox, pMatrix, &flRadius );

			if ( iHitbox == HITBOX_HEAD )
				flRadius *= Menu::Config.headpointscale;
			else
				flRadius *= Menu::Config.bodypointscale;

			if ( bMultiPoint == LEFTRIGHT ) // have the left and right multipoints the next line after eachother or it wont work correctly
				Info_t.vPos = Vec3D( vHitBoxPos + ( Mutipoint[ iIndex % 2 ] * flRadius ) );
			else if ( bMultiPoint == UP )
				Info_t.vPos = Vec3D( vHitBoxPos + ( Up * flRadius ));
			else if ( bMultiPoint == DOWN )
				Info_t.vPos = Vec3D( vHitBoxPos + ( Down * flRadius ));

			Info_t.bMultiPoint = bMultiPoint;
			Info_t.iMainHitbox = iHitbox;
			Info_t.iIndex = iIndex++;
			Info_t.iMainIndex = iLastMainHitbox;
			Info_t.flDamage = 0.f;
			return Info_t;
		}
	};

	static float flLastVelocity[65];
	float flVelocity = fabs( pEntity->m_vecVelocity( ).m_flLength2D( ) );
	if ( flVelocity > 29.f )
		flLastVelocity[ pEntity->entindex() ] = Interfaces::GlobalVars->curtime;
	float flLastMovingDelta = Interfaces::GlobalVars->curtime - flLastVelocity[ pEntity->entindex( ) ];

	if ( !bBacktrack ) // not backtracking hitboxes
	{
		for ( int i = 0; i < 9; i++ )
		{
			if ( Menu::Config.hitboxes[ i ] && (Menu::Config.ignorelimbs ? ((HitboxToGroup( GroupToHitbox( i ) ) == LIMBS) ? (( flLastMovingDelta >= 1.f ) ? true : false) : true ) : true )  )
			{
				Points.push_back( Hitbox( GroupToHitbox( i ) ) );
				if ( Menu::Config.multipoint )
				{
					if ( GroupToHitbox( i ) == HITBOX_HEAD )
						Points.push_back( Hitbox( GroupToHitbox( i ), UP ) );

					if ( i <= 4 )
					{
						Points.push_back( Hitbox( GroupToHitbox( i ), LEFTRIGHT ) );
						Points.push_back( Hitbox( GroupToHitbox( i ), LEFTRIGHT ) );
					}
				}
			}
		}
	}
	else // backtracking hitboxes
	{
		if ( Menu::Config.backhitboxes[ 0 ] )
		{
			Points.push_back( Hitbox( HITBOX_HEAD ) );
		}
		if ( Menu::Config.backhitboxes[ 1 ] )
		{
			Points.push_back( Hitbox( HITBOX_PELVIS ) );
		}
		if ( Menu::Config.backhitboxes[ 2 ] )
		{
			Points.push_back( Hitbox( HITBOX_CHEST ) );
		}
		if ( Menu::Config.backhitboxes[ 3 ] )
		{
			Points.push_back( Hitbox( HITBOX_UPPER_CHEST ) );
		}
	}

	return Points;
}

Vec3D Rage::Scan( CBaseEntity* pEntity, int iTick, std::deque<Record> pPlayerRecords, bool bOnShot = false )
{
	matrix3x4_t* pMatrix = pPlayerRecords.at( iTick ).mMatrix;

	flTempDmg = 0.f;
	iTickUsed = -1;

	auto Scan = [ & ] ( std::deque<PointInfo_t> HitPoints, float& flTempDmg, Vec3D& BestPoint  )
	{
		bool bResetDmg = true;

		if ( HitPoints.size(  ) > 0 )
			for ( int i = 0; i < HitPoints.size(  ); i++ )
			{
				PointInfo_t Point = HitPoints.at( i );

				HitPoints.at( i ).flDamage = Autowall::GetDamage( Point.vPos , pEntity );

				bool bExclude = false;
				float flVelocity = fabs( pEntity->m_vecVelocity(  ).m_flLength2D(  ) );

				if ( Menu::Config.smartex && Point.bMultiPoint )
				{
					if ( ( HitPoints.at( i ).flDamage - HitPoints.at( Point.iMainIndex ).flDamage ) <= 3.f )
						bExclude = true;
				}

				if ( HitboxToGroup( Point.iMainIndex ) == HEAD || HitboxToGroup( Point.iMainIndex ) == LIMBS )
				{
					if ( Menu::Config.baim[ 0 ] && !( pEntity->m_fFlags(  ) & FL_ONGROUND ) )
						bExclude = true;
					if ( Menu::Config.onshotmode == 2 && pPlayerRecords.at( iTick ).bShot )
						bExclude = true;
					/* note: simple fix for dumping fake on standing but there's some things you'll need to do for hitscan for this to work really nice */
					//if ( flVelocity <= 3.f )
						//bExclude = true;
				}

				if ( !bExclude )
				{
					Globals::AimPoints.push_back( Point.vPos );

					if ( Menu::Config.baim[ 1 ] && HitboxToGroup( Point.iMainIndex ) == BODY && HitPoints.at( i ).flDamage >= ( pEntity->m_iHealth( ) + 10 ) )
					{
						if (bResetDmg)
						{
							flTempDmg = 0.f;
							BestPoint = Vec3D(0, 0, 0);
							bResetDmg = false;
						}

						HitPoints.at(i).flDamage += 110; // im gay
					}
				}

				if (!bExclude && HitPoints.at( i ).flDamage > flTempDmg )
				{
					BestPoint = Point.vPos;
					flTempDmg = HitPoints.at( i ).flDamage;					
				}
			}
	};

	iTickUsed = iTick;

	LagComp::RestoreRecord( pEntity, iTick );

	flSimulationTime = pPlayerRecords.at( iTick ).flSimulationTime;

	Vec3D BestPoint = Vec3D( 0, 0, 0 );

	if ( !bOnShot )
		Scan( IteratePoints( pEntity, pMatrix, bBacktracking ), flTempDmg, BestPoint );
	else
	{
		Vec3D OnShotPoint = pEntity->GetHitboxPosition( HITBOX_HEAD, pMatrix );
		flTempDmg = Autowall::GetDamage( OnShotPoint, pEntity );
		if ( Menu::Config.primarydmg <= flTempDmg )
		{
			BestPoint = OnShotPoint;
			Globals::AimPoints.push_back( OnShotPoint );
		}
	}

	LagComp::RestoreRecord( pEntity, pPlayerRecords.size( ) - 1 ); // bring records back to normal

	return BestPoint;
}

Vec3D Rage::HitScan( CBaseEntity* pEntity )
{
	int iIndex = pEntity->entindex( );
	bBacktracking = false;
	std::deque<Record> pPlayerRecords = LagComp::pPlayer[ iIndex ];

	if ( pPlayerRecords.size( ) <= 0 )
		return Vec3D( 0, 0, 0 );

	Vec3D vPoint = Vec3D( 0, 0, 0 );

	bBacktracking = false;
	if (Menu::Config.backtracklby && LagComp::aiTickLby[iIndex] != -1)
		vPoint = Rage::Scan(pEntity, LagComp::aiTickLby[iIndex], pPlayerRecords);
	bBacktracking = true;

	if (vPoint != Vec3D(0, 0, 0))
		return vPoint;

	bBacktracking = true;
	if ( Menu::Config.onshotmode == 1 && LagComp::aiTickOnShot[ iIndex ] != -1 )
		vPoint = Rage::Scan( pEntity, LagComp::aiTickOnShot[ iIndex ], pPlayerRecords, true );

	if ( vPoint != Vec3D( 0, 0, 0 ) )
		return vPoint;

	bBacktracking = false;
	vPoint = Rage::Scan( pEntity, pPlayerRecords.size( ) - 1, pPlayerRecords );

	if ( vPoint != Vec3D( 0, 0, 0 ) )
		return vPoint;

	if ( fabs(pPlayerRecords.at(0).vVelocity.m_flLength2D()) <= 29.f)
		return Vec3D( 0, 0, 0 );

	bBacktracking = true;
	vPoint = Rage::Scan( pEntity, 0, pPlayerRecords );

	if ( vPoint != Vec3D( 0, 0, 0 ) )
		return vPoint;

	return Vec3D( 0, 0, 0 );
}

bool Rage::HitChance( CBaseEntity* pEntity, CBaseCombatWeapon* pWeapon, Vec3D vAngle, int iHitChance )
{
	if ( iHitChance == 0 )
		return true;

	static float flSeeds = 256.f;
	int iNeededHits = (float(iHitChance) * ( 2.56f ));

	Vec3D forward, right, up;

	Math::AngleVectors( vAngle, &forward, &right, &up );

	int iHits = 0;

	Vec3D vEyePos = pLocalEnt->GetEyePosition( );

	for ( int i = 1; i <= flSeeds; i++ )
	{
		float Inaccuracy = Math::RandomFloat( 0.f, 1.f ) * pWeapon->m_flInaccuracy();
		float Spread = Math::RandomFloat( 0.f, 1.f ) * pWeapon->m_flSpread();

		Vec3D spreadView( ( cos( Math::RandomFloat( 0.f, 2.f * PI ) ) * Inaccuracy ) + ( cos( Math::RandomFloat( 0.f, 2.f * PI ) ) * Spread ), ( sin( Math::RandomFloat( 0.f, 2.f * PI ) ) * Inaccuracy ) + ( sin( Math::RandomFloat( 0.f, 2.f * PI ) ) * Spread ), 0 ), direction;
		direction = Vec3D( forward.x + ( spreadView.x * right.x ) + ( spreadView.y * up.x ), forward.y + ( spreadView.x * right.y ) + ( spreadView.y * up.y ), forward.z + ( spreadView.x * right.z ) + ( spreadView.y * up.z ) );
		direction.Normalize( );

		Vec3D vSpread, vViewForward;

		Math::VectorAngles( direction, up, vSpread );
		Math::NormalizeAngles( vSpread );

		Math::AngleVectors( vSpread, &vViewForward );
		vViewForward.NormalizeInPlace( );

		vViewForward = vEyePos + ( vViewForward * pWeapon->m_pWeaponData( )->m_flRange );

		trace_t Trace;

		Interfaces::Trace->ClipRayToEntity( Ray_t( vEyePos, vViewForward ), MASK_SHOT | CONTENTS_GRATE, pEntity, &Trace );

		if ( Trace.m_pEntity == pEntity )
			iHits++;

		if ( iHits >= iNeededHits )
			return true;
	}

	return false;
}

void Rage::Aimbot( CUserCmd* pCmd )
{
	if ( Globals::AimPoints.size( ) > 0 )
		Globals::AimPoints.clear( );

	if ( !Interfaces::Engine->IsInGame( ) || !Interfaces::Engine->IsConnected( ) )
		return;

	if ( Menu::Config.legitback )
		return;

	if ( !Menu::Config.ragebot )
		return;

	pLocalEnt = Interfaces::User( );

	float flBestDmg = 0.f;
	Vec3D vBestPoint;
	float flBestSimtime = 0.f;
	int iBestTick = -1;
	bool bBestBacktrack = false;
	static int iSendTicks = 0;
	static float flSeenTime[ 65 ];

	if ( !pLocalEnt || !pLocalEnt->m_bIsAlive( ) )
		return;

	auto pWeapon = pLocalEnt->GetActiveWeapon( );

	if ( !pWeapon )
		return;

	if ( pWeapon->m_iClip1() == 0 )
		return;

	auto pWeaponData = pWeapon->m_pWeaponData( );

	if ( !pWeaponData )
		return;

	if ( pWeapon->IsKnifeorNade( ) )
		return;

	auto NetChannel = Interfaces::Engine->GetNetChannel( );

	if ( !NetChannel )
		return;

	CBaseEntity* pTarget = nullptr;
	std::string sResolveMode = "";
	int iBestResolveMode = 0;

	static float flLastShotTime = 0.f;
	float flServerTime = float( pLocalEnt->m_iTickBase( ) ) * Interfaces::GlobalVars->interval_per_tick;

	if ( fabs( flServerTime - flLastShotTime ) > 2.f )
		flLastShotTime = 0.f;

	bool bCanShoot = ( ( pWeapon->m_flNextPrimaryAttack( ) < flServerTime ) && ( pLocalEnt->m_flNextAttack( ) < flServerTime ) && ( flLastShotTime < flServerTime ) );

	static int iNextTick = 0;
	if ( abs( iNextTick - pCmd->tick_count ) > 10 )
		iNextTick = 0;

//	Globals::iTargetShot.second = -1;
	
	bool bCanDamage = false;

	for ( int index = 1; index < MAX_NETWORKID_LENGTH; ++index )
	{
		CBaseEntity* pPlayerEntity = Interfaces::EntityList->m_pGetClientEntity( index );

		if ( !pPlayerEntity
			|| !pPlayerEntity->m_bIsAlive( )			
			|| pLocalEnt->entindex( ) == index
			|| pPlayerEntity->IsDormant( )
			|| pLocalEnt->GetTeam( ) == pPlayerEntity->GetTeam( ) )
		{
			flSeenTime[ index ] = 0.f;
			continue;
		}

		if ( LagComp::pPlayer[ index ].size( ) <= 0 )
			continue;

		Vec3D vPoint = HitScan( pPlayerEntity );

		double MainMinDmg = Menu::Config.primarydmg;

		if ( flTempDmg <= 0.f )
			flSeenTime[ index ] = 0.f;
		if ( flTempDmg > 0.f )
		{
			bCanDamage = true;

			if ( flSeenTime[ index ] == 0.f )
				flSeenTime[ index ] = TICKS_TO_TIME( pCmd->tick_count ) + TICKS_TO_TIME( 18 );

			if ( Menu::Config.primarydmg <= flTempDmg )
			{
				flSeenTime[ index ] = TICKS_TO_TIME( pCmd->tick_count ) + TICKS_TO_TIME( 18 );
			}
			else if ( flSeenTime[ index ] < TICKS_TO_TIME( pCmd->tick_count ) )
				MainMinDmg = Menu::Config.seconddmg;
		}

		if ( vPoint != Vec3D( 0, 0, 0 ) && flBestDmg < flTempDmg && flTempDmg >= MainMinDmg )
		{
			pTarget = pPlayerEntity;
			vBestPoint = vPoint;
			flBestSimtime = flSimulationTime;
			iBestTick = iTickUsed;
			bBestBacktrack = bBacktracking;
			flBestDmg = flTempDmg;
			sResolveMode = LagComp::pPlayer[ index ].at( iTickUsed ).sCurrentResolve;
			iBestResolveMode = LagComp::pPlayer[index].at(iTickUsed).iCurrentMode;
		}
	}
	
	if ( bCanDamage )
		Stop( pCmd, pWeapon );

	if ( iSendTicks > 0 )
	{
		Globals::bSendPacket = true;
		iSendTicks -= 1;
	}
	else if ( pTarget && iBestTick != -1 )
	{
		int iIndex = pTarget->entindex( );

		Vec3D vTargetAngle = Math::CalcAngle( pLocalEnt->GetEyePosition( ), vBestPoint );

		LagComp::RestoreRecord( pTarget, iBestTick );

		if ( !( pCmd->buttons & IN_ATTACK ) && iNextTick < pCmd->tick_count && bCanShoot && HitChance( pTarget, pWeapon, vTargetAngle, Menu::Config.hitchance ) )
		{
			iNextTick = pCmd->tick_count + 1;
			pCmd->viewangles = vTargetAngle - ( Globals::AimPunchAng * Interfaces::Cvar->FindVar( "weapon_recoil_scale" )->ReadFloat( ) );
			pCmd->tick_count = TIME_TO_TICKS( flBestSimtime + LagComp::LerpTime( ) );
			pCmd->buttons |= IN_ATTACK;

			Globals::bSendPacket = true;
			iSendTicks = 2;

			AimbotShotInfo_t Shot;
			Shot.flShotRealTime = Interfaces::GlobalVars->realtime;
			Shot.flTargetsSimtime = pTarget->m_flSimulationTime( );
			Shot.iIndex = iIndex;
			Shot.iMode = iBestResolveMode;
			(bBestBacktrack ? Shot.sResolveMode = "LBY Backtrack" : Shot.sResolveMode = sResolveMode );
			Shot.iTick = iBestTick;
			Shot.bMoving = (pTarget->m_vecVelocity().m_flLength2D() > 1.f);
			ShotInfo.push_back( Shot );

			iTargetMatrix.first = LagComp::pPlayer[ iIndex ].at( iBestTick ).mMatrix;
			iTargetMatrix.second = iIndex;

			flLastShotTime = flServerTime + pWeapon->FireRate( );
		}

		LagComp::RestoreRecord( pTarget, LagComp::pPlayer[ iIndex ].size( ) - 1 );
	}
}