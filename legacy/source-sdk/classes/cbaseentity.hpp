#pragma once

struct PlayerInfo_t
{
	char pad[ 0x8 ]; //8
	int m_iXUID; //0x9
	int m_iXOID; //0xD
	char m_cName[ 128 ]; //0x11
	int m_iUserID; //0x91
	char m_cGUID[ 33 ]; //0x95
	char pad1[ 0x17B ]; //0xB6
};

class CPlayerAnimstate
{
public:
	char pad[ 0x3 ]; //3
	char bUnknown; //0x4
	char pad2[ 0x4E ]; //74
	float m_flAnimUpdateDelta; //0x54 not atually the delta idfk what it is i was just sent this class but im p sure its always 0
	char pad3[ 0x8 ]; //8
	void* pBaseEntity; //0x60
	void* pActiveWeapon; //0x64
	void* pLastActiveWeapon; //0x68
	float m_flLastClientSideAnimationUpdateTime; //0x6C
	int m_iLastClientSideAnimationUpdateFramecount; //0x70
	float m_flDelta; //0x74 im guessing this is the real update time delta
	float m_flEyeYaw; //0x78
	float m_flPitch; //0x7C
	float m_flGoalFeetYaw; //0x80
	float m_flCurrentFeetYaw; //0x84
	float m_flCurrentTorsoYaw; //0x88
	float m_flUnknownVelocityLean; //0x8C
	float m_flLeanAmount; //0x90
	char pad4[ 4 ]; //NaN
	float m_flFeetCycle; //0x98 0 to 1
	float m_flFeetYawRate; //0x9C 0 to 1
	float m_fUnknown2;
	float m_fDuckAmount; //0xA4
	float m_fLandingDuckAdditiveSomething; //0xA8
	float m_fUnknown3; //0xAC
	Vec3D m_vOrigin; //0xB0, 0xB4, 0xB8
	Vec3D m_vLastOrigin; //0xBC, 0xC0, 0xC4
	float m_vVelocityX; //0xC8
	float m_vVelocityY; //0xCC
	char pad5[ 0x4 ]; //4
	float m_flUnknownFloat1; //0xD4
	char pad6[ 0x8 ]; //8
	float m_flUnknownFloat2; //0xE0
	float m_flUnknownFloat3; //0xE4
	float m_unknown; //0xE8
	float speed_2d; //0xEC
	float flUpVelocity; //0xF0
	float m_flSpeedNormalized; //0xF4
	float m_flFeetSpeedForwardsOrSideWays; //0xF8
	float m_flFeetSpeedUnknownForwardOrSideways; //0xFC
	float m_flTimeSinceStartedMoving; //0x100
	float m_flTimeSinceStoppedMoving; //0x104
	bool m_bOnGround; //0x108
	char pad7[ 0x7 ]; //7
	float m_flTimeSinceInAir; //0x110
	float m_flLastOriginZ; //0x114
	float m_flHeadHeightOrOffsetFromHittingGroundAnimation; //0x118
	float m_flStopToFullRunningFraction; //0x11C
	char pad8[ 0x4 ]; //4
	float m_flMagicFraction; //0x0124
	char pad9[ 0x3C ]; //60
	float m_flWorldForce; //0x0164
};

class CAnimationLayer
{
public:
	char pad[ 0x14 ]; 
	unsigned int m_iOrder; //0x014
	unsigned int m_iSequence; //0x18
	float m_flPrevCycle; //0x1C
	float m_flWeight; //0x20
	float m_flWeightDeltaRate; //0x24
	float m_flPlaybackRate; //0x28
	float m_flCycle; //0x2C
	void *m_pOwner; //0x30
	char pad2[ 0x4 ];
};

class CBaseCombatWeapon;
class CBaseEntity : public IClientUnknown, public IClientRenderable, public IClientNetworkable
{
public:
	// Treating this as a function tbh
	template <typename T>
	T& Get( uintptr_t offset ) 
	{
		return *reinterpret_cast< T* >( reinterpret_cast< uintptr_t >( this ) + offset );
	}

	// To get value from the pointer itself
	template<class T>
	T Read( uintptr_t offset )
	{
		return *reinterpret_cast< T* >( reinterpret_cast< uintptr_t >( this ) + offset );
	}

	template <typename t>
	static t GetVFunc( void* class_pointer, size_t index ) 
	{
		return ( *( t** ) class_pointer )[ index ];
	}

	// Offsets
	OF( m_pNetworkable, void*, 0x8 );
	OF( m_pRenderable, void*, 0x4 );
	OF( m_pAnimOverlay, CAnimationLayer*, 0x2970);
	OF( m_pAnimState, CPlayerAnimstate*, 0x3874);
	OF( GetBoneCache, CUtlVector< matrix3x4_t >, 0x2900 ); //memmove_0(a3, *(const void **)(v7 + 0x290C), 48 * v90);
	OF( m_iBoneCount, int, 0x2908); // v90 = *(_DWORD *)(v7 + 0x2918);
	OF( m_iGetEffects, int, 0xEC );
	OF( m_iMoveType, int, 0x258);
	OF( GetViewOffset, Vec3D, 0x104);

	// Virtual functions
	//VF( m_bIsPlayer, bool, 157 );
	VF( m_bIsWeapon, bool, 163 );
	VF( UpdateClientSideAnimations, void, 218 );
	VF( GetAbsOrigin, Vec3D, 10 );

	// Netvars
	OF( m_iTickBase, int, gOffsets->m_nTickBase );
	OF( m_flC4Blow, float, gOffsets->m_flC4Blow );
	OF( m_angEyeAngles, Vec3D, gOffsets->m_angEyeAngles );
	OF( m_iHealth, int, gOffsets->m_iHealth );
	OF( m_lifeState, bool, gOffsets->m_lifeState );
	OF( m_fFlags, int, gOffsets->m_fFlags );
	OF( m_vecVelocity, Vec3D, gOffsets->m_vecVelocity );
	OF( m_flSimulationTime, float, gOffsets->m_flSimulationTime );
	OF( m_bClientSideAnimation, bool, gOffsets->m_bClientSideAnimation );
	OF( m_aimPunchAngle, Vec3D, gOffsets->m_aimPunchAngle );
	OF( m_viewPunchAngle, Vec3D, gOffsets->m_viewPunchAngle );
	OF( m_aimPunchAngleVel, Vec3D, gOffsets->m_aimPunchAngleVel );
	OF( m_vecOrigin, Vec3D, gOffsets->m_vecOrigin );
	OF( m_bHasHeavyArmor, bool, gOffsets->m_bHasHeavyArmor );
	OF( m_ArmorValue, int, gOffsets->m_ArmorValue );
	OF( m_bHasHelmet, bool, gOffsets->m_bHasHelmet );
//	OF( m_flPoseParameter, float*, gOffsets->m_flPoseParameter );
	OF( m_flModelScale, float, gOffsets->m_flModelScale );
	OF( m_flLowerBodyYaw, float, gOffsets->m_flLowerBodyYawTarget );
	OF( m_flNextAttack, float, gOffsets->m_flNextAttack );
	OF( m_bClientSideFrameReset, bool, gOffsets->m_bClientSideFrameReset);
	OF( m_bDrawViewmodel, bool, gOffsets->m_bDrawViewmodel );
	OF( m_angRotation, Vec3D, gOffsets->m_angRotation );

	std::array<float, 24>& m_flPoseParameter()
	{
		return *(std::array<float, 24>*)((uintptr_t)this + gOffsets->m_flPoseParameter);
	}

	bool m_bIsAlive( ) 
	{
		return m_iHealth( ) > 0 && m_lifeState( ) == 0;
	}

	CBaseCombatWeapon* GetActiveWeapon( )
	{
		return ( CBaseCombatWeapon* ) ( Interfaces::EntityList->m_pGetClientEntityHandle( Get<CBaseHandle>( gOffsets->m_hActiveWeapon ) ) );
	}

	Vec3D GetEyePosition( )
	{
		return GetViewOffset() + m_vecOrigin();
	}

	int GetTeam( )
	{
	//	if ( !Interfaces::GameTypes )
	//		return *reinterpret_cast< int* >( reinterpret_cast< uintptr_t >( this ) + gOffsets->m_iTeamNum );
	//	if ( Interfaces::GameTypes->GetCurrentGameType( ) == 6 )
	//		return *reinterpret_cast< int* >( reinterpret_cast< uintptr_t >( this ) + gOffsets->m_nSurvivalTeam );
	//	else
			return *reinterpret_cast< int* >( reinterpret_cast< uintptr_t >( this ) + gOffsets->m_iTeamNum );
	}

	void InvalidateBoneCache( ) // deadcell
	{
		*reinterpret_cast<unsigned int*>(reinterpret_cast<DWORD> (this) + gOffsets->m_nForceBone + 0x20) = 0;
		*reinterpret_cast<unsigned int*>(reinterpret_cast<DWORD> (this) + gOffsets->m_nForceBone + 0x4) = -1;

		static uintptr_t InvalidateBoneCache = Memory::Signature<uintptr_t>(GetModuleHandleA("client.dll"), "80 3D ? ? ? ? ? 74 16 A1 ? ? ? ? 48 C7 81");
		static uintptr_t MDLBoneCounter = **reinterpret_cast<uintptr_t**> (InvalidateBoneCache + 10);
		*reinterpret_cast<unsigned int*>(reinterpret_cast<DWORD> (this) + 0x2914) = 0xFF7FFFFF; //bone setup time
		*reinterpret_cast<unsigned int*>(reinterpret_cast<DWORD> (this) + 0x2680) = (MDLBoneCounter - 1); //bone model counter
	}

	void SetAbsAngles( Vec3D angles )
	{
		using Fn = void( __thiscall* )( CBaseEntity*, const Vec3D& angles );
		static Fn AbsAngles = ( Fn ) ( Memory::Signature<uintptr_t>( GetModuleHandleA( "client.dll" ), "55 8B EC 83 E4 F8 83 EC 64 53 56 57 8B F1 E8" ) );

		AbsAngles( this, angles );
	}

	void SetAbsOrigin( Vec3D origin )
	{
		using Fn = void( __thiscall* )( CBaseEntity*, const Vec3D& origin );
		static Fn AbsOrigin = ( Fn ) ( Memory::Signature<uintptr_t>( GetModuleHandleA( "client.dll" ), "55 8B EC 83 E4 F8 51 53 56 57 8B F1 E8 ? ? ? ? 8B 7D" ) );

		AbsOrigin( this, origin );
	}

	void SetAbsVelocity( Vec3D velocity )
	{
		using Fn = void( __thiscall* )( CBaseEntity*, const Vec3D& velocity );
		static Fn AbsVelocity = ( Fn ) ( Memory::Signature<uintptr_t>( GetModuleHandleA( "client.dll" ), "55 8B EC 83 E4 F8 83 EC 0C 53 56 57 8B 7D 08 8B F1 F3" ) );

		AbsVelocity( this, velocity );
	}

	void SetUserCmd( CUserCmd *pCmd )
	{
		*reinterpret_cast< CUserCmd** >( reinterpret_cast< uintptr_t >( this ) + 0x3314 ) = pCmd;
	}

	int GetSequenceActivity( int sequence )
	{
		studiohdr_t* hdr = Interfaces::ModelInfo->GetStudiomodel( this->GetModel( ) );
		
		if ( !hdr )
			return -1;

		static auto GetSequenceActivity = reinterpret_cast< int( __fastcall* )( void*, studiohdr_t*, int ) >( Memory::Signature<uintptr_t>( GetModuleHandleA( "client.dll" ), "55 8B EC 53 8B 5D 08 56 8B F1 83" ) );

		return GetSequenceActivity( this, hdr, sequence );
	}

	Vec3D GetHitboxPosition( int iHitbox, matrix3x4_t *pMatrix, float* flRadius = nullptr )
	{
		if ( iHitbox >= HITBOX_MAX || iHitbox < 0 || !pMatrix )
			return Vec3D( 0, 0, 0 );

		model_t* Model = GetModel( );

		if ( !Model )
			return Vec3D( 0, 0, 0 );

		studiohdr_t* hdr = Interfaces::ModelInfo->GetStudiomodel( Model );

		if ( !hdr )
			return Vec3D( 0, 0, 0 );

		mstudiohitboxset_t* set = hdr->GetHitboxSet( 0 );

		if ( !set )
			return Vec3D( 0, 0, 0 );

		mstudiobbox_t* hitbox = set->GetHitbox( iHitbox );

		if ( !hitbox )
			return Vec3D( 0, 0, 0 );

		if ( hitbox->bone >= 128 || hitbox->bone < 0)
			return Vec3D( 0, 0, 0 );

		Vec3D vMin, vMax;
		Math::VectorTransform( hitbox->min, pMatrix[ hitbox->bone ], vMin ); //  crashes here
		Math::VectorTransform( hitbox->max, pMatrix[ hitbox->bone ], vMax );

		if ( flRadius )
			*flRadius = hitbox->radius;

		return Vec3D( ( vMin + vMax ) * 0.5 );
	}

	void DrawHitboxes( matrix3x4_t *pMatrix, Color cFaceColor, Color cEdgeColor, float flDuration = -1.f )
	{
		model_t* Model = this->GetModel( );

		if ( !Model )
			return;

		studiohdr_t* hdr = Interfaces::ModelInfo->GetStudiomodel( Model );

		if ( !hdr )
			return;

		mstudiohitboxset_t* set = hdr->GetHitboxSet( 0 );

		if ( !set )
			return;

		for ( int i = 0; i < HITBOX_MAX; i++ )
		{
			mstudiobbox_t* hitbox = set->GetHitbox( i );

			if ( !hitbox )
				continue;

			if ( hitbox->bone >= 128 || hitbox->bone < 0 )
				continue;

			Vec3D Angle, Position;

			Math::MatrixAngles( pMatrix[ hitbox->bone ], Angle, Position );

			uint8_t secondaryclr[ ] = { cEdgeColor.r( ),cEdgeColor.g( ),cEdgeColor.b( ),cEdgeColor.a( ) };
			uint8_t primaryclr[ ] = { cFaceColor.r( ),cFaceColor.g( ),cFaceColor.b( ),cFaceColor.a( ) };

			if ( i == HITBOX_LEFT_FOOT || i == HITBOX_RIGHT_FOOT || i == HITBOX_RIGHT_HAND || i == HITBOX_LEFT_HAND )
			{
				if ( this->m_fFlags( ) & FL_ONGROUND )
				{
					if ( i == HITBOX_LEFT_FOOT )
						Angle.x += 25;
					else if ( i == HITBOX_RIGHT_FOOT )
						Angle.x -= 25;
				}

				Interfaces::DebugOverlay->AddBoxOverlaySecond( Position, hitbox->min, hitbox->max, Angle, primaryclr, secondaryclr, flDuration );
			}
			else
				Interfaces::DebugOverlay->AddBoxOverlaySecond( Position, Globals::MaxsMins[ i ][ 0 ], Globals::MaxsMins[ i ][ 1 ], Angle, primaryclr, secondaryclr, flDuration );
		}
	}

	void GetCollisionBounds( Vec3D& mins, Vec3D& maxs )
	{
		const static auto call = Memory::Signature< uintptr_t >( GetModuleHandleA( "client.dll" ), "55 8B EC 83 EC 48 53 56 57 8B D9" );
		reinterpret_cast< void( __thiscall* )( void*, const Vec3D&, const Vec3D& ) >( call )( this, mins, maxs );
	}

	void SetCollisionBounds( const Vec3D& mins, const Vec3D& maxs )
	{
		void* pCollideable = this + 800;
		const static auto call = Memory::Signature< uintptr_t >( GetModuleHandleA( "client.dll" ), "53 8B DC 83 EC 08 83 E4 F8 83 C4 04 55 8B 6B 04 89 6C 24 04 8B EC 83 EC 10 56 57 8B 7B 08" );
		reinterpret_cast< void( __thiscall* )( void*, const Vec3D&, const Vec3D& ) >( call )( pCollideable, mins, maxs );
	}

	CAnimationLayer& GetAnimOverlays(int Index)
	{
		return (*(CAnimationLayer**)((DWORD)this + 0x2970))[Index];
	}

	void SetEyeAngles(Vec3D angles)
	{
		*reinterpret_cast<Vec3D*>(uintptr_t(this) + 0xB23C) = angles;
	}
};