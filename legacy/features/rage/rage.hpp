#pragma once

enum EMultiPointMode
{
	NONE=0,
	LEFTRIGHT,
	UP,
	DOWN
};

enum EHitGroups
{
	HEAD = 0,
	BODY,
	LIMBS
};

struct PointInfo_t
{
	Vec3D vPos;
	bool bMultiPoint;
	int iIndex;
	int iMainHitbox;
	int iMainIndex;
	float flDamage;
};

struct AimbotShotInfo_t
{
	float flTargetsSimtime;
	float flShotRealTime;
	int iIndex;
	std::string sResolveMode;
	int iMode;
	int iTick;
	bool bMoving;
};

namespace Rage
{
	// aimbot
	extern float flSimulationTime;
	extern float flTempDmg;
	extern int iTickUsed;
	extern bool bBacktracking;
	extern CBaseEntity* pLocalEnt;
	extern std::deque<AimbotShotInfo_t> ShotInfo;
	extern std::pair<matrix3x4_t*, int> iTargetMatrix;

	static EHitGroups HitboxToGroup( int i )
	{
		if ( i >= 0 && i <= 1 )
			return HEAD;
		else if ( i >= 3 && i <= 7 )
			return BODY;
		else
			return LIMBS;
	}

	static int GroupToHitbox( int i )
	{
		switch ( i )
		{
		case 0:
			return HITBOX_HEAD;
			break;
		case 1:
			return HITBOX_PELVIS;
			break;
		case 2:
			return HITBOX_BELLY;
			break;
		case 3:
			return HITBOX_CHEST;
			break;
		case 4:
			return HITBOX_UPPER_CHEST;
			break;
		case 5:
			return HITBOX_LEFT_THIGH;
			break;
		case 6:
			return HITBOX_RIGHT_THIGH;
			break;
		case 7:
			return HITBOX_LEFT_FOOT;
			break;
		case 8:
			return HITBOX_RIGHT_FOOT;
			break;
		}
	}

	static Vec3D DDecayAimPunchAngleReversed( CBaseEntity *pEntity, Vec3D& vPunchAngleVelocity ) // thanks mre521 https://www.unknowncheats.me/forum/counterstrike-global-offensive/149894-reversed-norecoil-cs-go-and-explanation.html
	{
		Vec3D vPunchAngle, vPunchAngleExpDecay;
		float flExpDecay, flExpDecayResult;
		float flLinDecay, flLinDecayResult;

		float tick_interval;
		float flPunchAngleExpDecayLength;

		ConVar* weapon_recoil_decay2_exp;
		ConVar* weapon_recoil_decay2_lin;
		ConVar* weapon_recoil_vel_decay;

		ConVar* weapon_recoil_scale;

		weapon_recoil_decay2_exp = Interfaces::Cvar->FindVar( "weapon_recoil_decay2_exp" );
		weapon_recoil_decay2_lin = Interfaces::Cvar->FindVar( "weapon_recoil_decay2_lin" );
		weapon_recoil_vel_decay = Interfaces::Cvar->FindVar( "weapon_recoil_vel_decay" );

		weapon_recoil_scale = Interfaces::Cvar->FindVar( "weapon_recoil_scale" );

		Vec3D& rPunchAngle = pEntity->m_aimPunchAngle( );
		vPunchAngle[ 0 ] = rPunchAngle[ 0 ];
		vPunchAngle[ 1 ] = rPunchAngle[ 1 ];
		vPunchAngle[ 2 ] = rPunchAngle[ 2 ];

		flExpDecay = weapon_recoil_decay2_exp->ReadFloat( );
		flLinDecay = weapon_recoil_decay2_lin->ReadFloat( );

		tick_interval = Interfaces::GlobalVars->interval_per_tick;

		flExpDecayResult = exp( -flExpDecay * tick_interval );

		vPunchAngleExpDecay = vPunchAngle * flExpDecayResult;
		flPunchAngleExpDecayLength = vPunchAngleExpDecay.m_flLength( );

		flLinDecayResult = flLinDecay * tick_interval;

		if ( ( flPunchAngleExpDecayLength > 0.0 ) && ( flPunchAngleExpDecayLength > flLinDecayResult ) )
		{
			float flMultiplier;

			flMultiplier = 1.0f - ( flLinDecayResult / flPunchAngleExpDecayLength );
			vPunchAngleExpDecay *= flMultiplier;
		}
		else
		{
			vPunchAngleExpDecay.x = vPunchAngleExpDecay.y = vPunchAngleExpDecay.z = 0;
		}

		vPunchAngle = ( vPunchAngleVelocity * tick_interval * 0.5 ) + vPunchAngleExpDecay;

		flExpDecayResult = exp( tick_interval * -weapon_recoil_vel_decay->ReadFloat( ) );

		vPunchAngleVelocity *= flExpDecayResult;
		vPunchAngle += ( vPunchAngleVelocity * tick_interval * 0.5f );

		Vec3D result;
		result[ 0 ] = ( float ) vPunchAngle[ 0 ];
		result[ 1 ] = ( float ) vPunchAngle[ 1 ];
		result[ 2 ] = ( float ) vPunchAngle[ 2 ];

		return result;
	}

	static Vec3D DecayAimPunchAngleReversed( CBaseEntity *pEntity ) // thanks mre521 https://www.unknowncheats.me/forum/counterstrike-global-offensive/149894-reversed-norecoil-cs-go-and-explanation.html
	{
		return DDecayAimPunchAngleReversed( pEntity, pEntity->m_aimPunchAngleVel( ) );
	}

	std::deque<PointInfo_t> IteratePoints( CBaseEntity* pEntity, matrix3x4_t* pMatrix, bool bBacktrack );
	Vec3D Scan( CBaseEntity* pEntity, int iTick, std::deque<Record> pPlayerRecords, bool bOnShot );
	Vec3D HitScan( CBaseEntity* pEntity );
	bool HitChance( CBaseEntity* pEntity, CBaseCombatWeapon* pWeapon, Vec3D vAngle, int iHitChance );
	void Aimbot( CUserCmd* pCmd ); 

	// antiaim
	static void CorrectMovement( Vec3D& oldAngles, CUserCmd* pCmd )
	{
		Vec3D vMovements( pCmd->forwardmove, pCmd->sidemove, 0.f );

		if ( vMovements.m_flLength2D( ) == 0 )
			return;

		Vec3D vRealF, vRealR;
		Vec3D aRealDir = pCmd->viewangles;
		aRealDir.Clamp( );

		Math::AngleVectors( aRealDir, &vRealF, &vRealR, nullptr );
		vRealF[ 2 ] = 0;
		vRealR[ 2 ] = 0;

		vRealF.Normalize();
		vRealR.Normalize( );

		Vec3D aWishDir = oldAngles;
		aWishDir.Clamp( );

		Vec3D vWishF, vWishR;
		Math::AngleVectors( aWishDir, &vWishF, &vWishR, nullptr );

		vWishF[ 2 ] = 0;
		vWishR[ 2 ] = 0;

		vWishF.Normalize( );
		vWishR.Normalize( );

		Vec3D vWishVel;
		vWishVel[ 0 ] = vWishF[ 0 ] * pCmd->forwardmove + vWishR[ 0 ] * pCmd->sidemove;
		vWishVel[ 1 ] = vWishF[ 1 ] * pCmd->forwardmove + vWishR[ 1 ] * pCmd->sidemove;
		vWishVel[ 2 ] = 0;

		float a = vRealF[ 0 ], b = vRealR[ 0 ], c = vRealF[ 1 ], d = vRealR[ 1 ];
		float v = vWishVel[ 0 ], w = vWishVel[ 1 ];

		float flDivide = ( a * d - b * c );
		float x = ( d * v - b * w ) / flDivide;
		float y = ( a * w - c * v ) / flDivide;

		pCmd->forwardmove = x;
		pCmd->sidemove = y;
	}

	static void Stop( CUserCmd* pCmd, CBaseCombatWeapon* pWeapon = nullptr )
	{
		if ( !Menu::Config.autostop )
			return;

		if ( pCmd->buttons & IN_JUMP )
			return;

		auto pLocalEnt = Interfaces::User( );

		if ( !pLocalEnt )
			return;

		if ( !pLocalEnt->m_bIsAlive( ) )
			return;

		Vec3D Velocity = pLocalEnt->m_vecVelocity( );

		if ( !pWeapon )
			pWeapon = pLocalEnt->GetActiveWeapon( );

		if ( !pWeapon )
			return;

		float MaxVel = ( 200.f * .34f );

		if ( Velocity.m_flLength( ) >= (MaxVel - 25.f) )
		{
			Vec3D Direction, RealView, Forward;
			Math::VectorAngles( Velocity, Direction );
			Interfaces::Engine->GetViewAngles( RealView );
			Direction.y = RealView.y - Direction.y;
			Math::AngleVectors( Direction, &Forward );
			Vec3D NegativeDirection = Forward * -450.f;

			pCmd->forwardmove = NegativeDirection.x;
			pCmd->sidemove = NegativeDirection.y;
		}
	}

	void AutoRevolver( CUserCmd* pCmd );
	void AntiAim( CUserCmd* pCmd );

	// engine pred shit
	extern float flCurTime;
	extern float flFrameTime;
	extern int iTickCount;

	void PreEnginePred( CUserCmd* pCmd );
	void PostEnginePred(CUserCmd* pCmd);

	// fakelag
	void FakeLag( );
}

class Antiaim
{
public:
	void Do(CUserCmd* cmd);
	void UpdateLBY();

	/* note: who says i cant do my math here lmao */
	static float GetCurtime()
	{
		auto local_player = Interfaces::User();
		if (!local_player)
			return 0.f;

		return local_player->m_iTickBase() * Interfaces::GlobalVars->interval_per_tick;
	}

	static float GetLBYRotatedYaw(float lby, float yaw)
	{
		float delta = Math::m_flNormalizeYaw(yaw - lby);
		if (fabs(delta) < 25.f)
			return lby;

		if (delta > 0.f)
			return yaw + 25.f;

		return yaw;
	}

	float TimeUntilNextLBYUpdate()
	{
		return flNextLBYUpdateTime - GetCurtime();
	}
private:
	float flNextLBYUpdateTime = 0.f, flLastMoveTime = 0.f, flLastAttemptedLBY = 0.f;
	bool  bWillLBYUpdate = false;
private:
	float DoYaw();
	float DoPitch();

	float BreakLBY(float real_yaw, float desired_lby, bool double_flick = false);
};

extern Antiaim antiaim;