#include "../features.hpp"

/* note: i had plans to redo the autowall at some point but i never did lmao */

void Autowall::TraceLine( Vec3D& start, Vec3D& end, unsigned int mask, CBaseEntity* ignore, trace_t* ptr ) {

	CTraceFilter filter( ignore );
	Interfaces::Trace->TraceRay( Ray_t( start, end ), mask, &filter, ptr );
}

bool Autowall::VectortoVectorVisible( Vec3D src, Vec3D point, CBaseEntity* pEntity = nullptr )
{
	trace_t TraceInit;
	TraceInit.m_pEntity = nullptr;
	CTraceFilter filter1( Interfaces::User( ) );
	Interfaces::Trace->TraceRay( Ray_t( src, point ), MASK_SOLID, &filter1, &TraceInit );

	if ( TraceInit.m_flFraction == 1.0f )
		return true;

	if ( pEntity != nullptr && TraceInit.m_pEntity == pEntity )
		return true;

	return false;
}

// idk monarch sent me the autowall the floating point is mine tho :)

// i will never be able to make my own autowall

bool Autowall::Breakable( CBaseEntity* pEntity ) {
	static auto	ptr_instruction = Memory::Signature<uintptr_t>(GetModuleHandleA( "client.dll" ),"55 8B EC 51 56 8B F1 85 F6 74 68 83 BE");

	using Fn = bool( __fastcall* )( CBaseEntity* );
	static auto fn = reinterpret_cast< Fn >( ptr_instruction );

	if ( !pEntity || !pEntity->entindex( ) ) {
		return false;
	}

	//  m_takeDamage isn't properly set when using the signature.
	//  back it up, set it to true, then restore.
	auto take_damage{ ( char* ) ( ( uintptr_t ) pEntity + *( size_t* ) ( ( uintptr_t ) fn + 38 ) ) };
	auto take_damage_backup{ *take_damage };

	auto* cclass = Interfaces::Client->GetAllClasses( );

	//				 '       ''     '      '   '
	//			    01234567890123456     012345678
	//  check against CBreakableSurface and CBaseDoor

	//  windows etc. are CBrekableSurface
	//  large garage door in office is CBaseDoor and it get's reported as a breakable when it is not one
	//  this is seperate from "CPropDoorRotating", which is a normal door.
	//  normally you would also check for "CFuncBrush" but it was acting oddly
	//  ((cclass->m_pNetworkName[1]) != 'F' || (cclass->m_pNetworkName[4]) != 'c' || (cclass->m_pNetworkName[5]) != 'B' || (cclass->m_pNetworkName[9]) != 'h')

	if ( ( cclass->m_pNetworkName[ 1 ] == 'B' && cclass->m_pNetworkName[ 9 ] == 'e' && cclass->m_pNetworkName[ 10 ] == 'S' && cclass->m_pNetworkName[ 16 ] == 'e' )
		|| ( cclass->m_pNetworkName[ 1 ] != 'B' || cclass->m_pNetworkName[ 5 ] != 'D' ) )
		*take_damage = 2;

	bool breakable = fn( pEntity );
	*take_damage = take_damage_backup;

	return breakable;
}

void Autowall::ScaleDamage( CBaseEntity* pEntity, int hitgroup, WeaponInfo_t* weapon_data, float& current_damage ) {
	bool   has_heavy_armor = pEntity->m_bHasHeavyArmor( );
	int    armor = pEntity->m_ArmorValue( );
	int    hit_group = hitgroup;

	auto armored = [ & ] ( ) -> bool {
		CBaseEntity* target = pEntity;

		switch ( hitgroup ) {
		case HITGROUP_HEAD:
			return target->m_bHasHelmet( );
		case HITGROUP_GENERIC:
		case HITGROUP_CHEST:
		case HITGROUP_STOMACH:
		case HITGROUP_LEFTARM:
		case HITGROUP_RIGHTARM:
			return true;
		default:
			return false;
		}
	};

	switch ( hit_group ) {
	case HITGROUP_HEAD:
		current_damage *= has_heavy_armor ? 2.f : 4.f;
		break;
	case HITGROUP_STOMACH:
		current_damage *= 1.25f;
		break;
	case HITGROUP_LEFTLEG:
	case HITGROUP_RIGHTLEG:
		current_damage *= 0.75f;
		break;
	default:
		break;
	}

	if ( armor > 0 && armored( ) ) {
		float bonus_value = 1.f, armor_bonus_ratio = 0.5f, armor_ratio = weapon_data->m_flArmorRatio / 2.f;

		if ( has_heavy_armor ) {
			armor_bonus_ratio = 0.33f;
			armor_ratio *= 0.5f;
			bonus_value = 0.33f;
		}

		auto new_damage = current_damage * armor_ratio;

		if ( has_heavy_armor ) {
			new_damage *= 0.85f;
		}

		if ( ( ( current_damage - ( current_damage * armor_ratio ) ) * ( bonus_value * armor_bonus_ratio ) ) > armor ) {
			new_damage = current_damage - ( armor / armor_bonus_ratio );
		}

		current_damage = new_damage;
	}
}

bool Autowall::TraceToExit( trace_t& enter_trace, trace_t& exit_trace, Vec3D start_position, Vec3D dir ) {
	Vec3D   start, end;
	float  max_distance = 90.f,
		ray_extension = 4.f,
		current_distance = 0;
	int    first_contents = 0;

	while ( current_distance <= max_distance ) {
		//  add extra distance to our ray
		current_distance += ray_extension;

		//  multiply the direction vector to the distance so we go outwards, add our position to it.
		start = start_position + dir * current_distance;

		if ( !first_contents ) {
			first_contents = Interfaces::Trace->m_iGetPointContents( start, MASK_SHOT_HULL | CONTENTS_HITBOX, nullptr ); /*0x4600400B*/
		}

		int point_contents = Interfaces::Trace->m_iGetPointContents( start, MASK_SHOT_HULL | CONTENTS_HITBOX, nullptr );

		if ( !( point_contents & MASK_SHOT_HULL ) || point_contents & CONTENTS_HITBOX && point_contents != first_contents ) /*0x600400B, *0x40000000*/ {

			//  let's setup our end position by deducting the direction by the extra added distance
			end = start - ( dir * ray_extension );

			//  let's cast a ray from our start pos to the end pos
			TraceLine( start, end, MASK_SHOT_HULL | CONTENTS_HITBOX, nullptr, &exit_trace );

			//  let's check if a hitbox is in-front of our enemy and if they are behind of a solid wall
			if ( exit_trace.m_bStartSolid && exit_trace.m_uSurface.m_uFlags & SURF_HITBOX ) {
				TraceLine( start, start_position, MASK_SHOT_HULL, exit_trace.m_pEntity, &exit_trace );

				if ( exit_trace.m_bDidHit( ) && !exit_trace.m_bStartSolid ) {
					start = exit_trace.m_vEndPos;

					return true;
				}

				continue;
			}

			//  can we hit? is the wall solid?
			if ( exit_trace.m_bDidHit( ) && !exit_trace.m_bStartSolid ) {

				// is the wall a breakable? if so, let's shoot through it.
				if ( Breakable( enter_trace.m_pEntity ) && Breakable( exit_trace.m_pEntity ) ) {
					return true;
				}

				if ( enter_trace.m_uSurface.m_uFlags & SURF_NODRAW || !( exit_trace.m_uSurface.m_uFlags & SURF_NODRAW ) && ( exit_trace.m_uPlane.m_vNormal.m_flDot( dir ) <= 1.f ) ) {
					float mult_amount = exit_trace.m_flFraction * 4.f;

					start -= dir * mult_amount;

					return true;
				}

				continue;
			}

			if ( !exit_trace.m_bDidHit( ) || exit_trace.m_bStartSolid ) {
				if ( ( enter_trace.m_pEntity != nullptr && enter_trace.m_pEntity->entindex( ) != 0 ) && Breakable( enter_trace.m_pEntity ) ) {
					exit_trace = enter_trace;

					exit_trace.m_vEndPos = start + dir;

					return true;
				}

				continue;
			}
		}
	}

	return false;
}

bool Autowall::HandleBulletPen( WeaponInfo_t* wpn_data, FireBulletData& data, bool extracheck = false, Vec3D point = Vec3D( 0, 0, 0 ), CBaseEntity* pEntity = nullptr ) {
	trace_t       trace_exit;
	surfacedata_t* enter_surface_data = Interfaces::SurfaceProps->GetSurfaceData( data.enter_trace.m_uSurface.m_uName );
	int           enter_material = enter_surface_data->game.material;

	CBaseEntity* entity = data.enter_trace.m_pEntity;

	float         enter_surf_penetration_modifier = enter_surface_data->game.flPenetrationModifier;
	float         final_damage_modifier = 0.f;
	float         combined_penetration_modifier = 0.f;
	bool          solid_surf = ( ( data.enter_trace.m_iContents >> 3 )& CONTENTS_SOLID );
	bool          light_surf = ( ( data.enter_trace.m_uSurface.m_uFlags >> 7 )& SURF_LIGHT );

	if ( data.penetrate_count <= 0
		|| ( !data.penetrate_count && !light_surf && !solid_surf && enter_material != CHAR_TEX_GLASS && enter_material != CHAR_TEX_GRATE )
		|| wpn_data->m_flPenetration <= 0.f
		|| !TraceToExit( data.enter_trace, trace_exit, data.enter_trace.m_vEndPos, data.direction )
		&& !( Interfaces::Trace->m_iGetPointContents( data.enter_trace.m_vEndPos, MASK_SHOT_HULL, nullptr ) & ( MASK_SHOT_HULL ) ) )
		return false;

	surfacedata_t* exit_surface_data = Interfaces::SurfaceProps->GetSurfaceData( trace_exit.m_uSurface.m_uName );
	int           exit_material = exit_surface_data->game.material;
	float         exit_surf_penetration_modifier = exit_surface_data->game.flPenetrationModifier;

	static ConVar* damage_reduction_bullets = Interfaces::Cvar->FindVar( "ff_damage_reduction_bullets" );
	static ConVar* damage_bullet_penetration = Interfaces::Cvar->FindVar( "ff_damage_bullet_penetration" );

	if ( enter_material == CHAR_TEX_GLASS || enter_material == CHAR_TEX_GRATE ) {
		combined_penetration_modifier = 3.f;
		final_damage_modifier = 0.05f;
	}
	else if ( light_surf || solid_surf ) {
		combined_penetration_modifier = 1.f;
		final_damage_modifier = 0.16f;
	}
	else if ( data.enter_trace.m_pEntity && enter_material == CHAR_TEX_FLESH && ( Interfaces::User()->GetTeam( ) == entity->GetTeam( ) && damage_reduction_bullets->ReadFloat( ) == 0.f ) ) {
		//  look's like you aren't shooting through your teammate today
		if ( damage_bullet_penetration->ReadFloat( ) == 0.f ) {
			return false;
		}

		combined_penetration_modifier = damage_bullet_penetration->ReadFloat( );
		final_damage_modifier = 0.16f;
	}

	else {
		combined_penetration_modifier = ( enter_surf_penetration_modifier + exit_surf_penetration_modifier ) * 0.5f;
		final_damage_modifier = 0.16f;
	}

	//  do our materials line up?
	if ( enter_material == exit_material ) {
		if ( exit_material == CHAR_TEX_CARDBOARD || exit_material == CHAR_TEX_WOOD ) {
			combined_penetration_modifier = 3.f;
		}

		else if ( exit_material == CHAR_TEX_PLASTIC ) {
			combined_penetration_modifier = 2.f;
		}
	}

	//  calculate thickness of the wall by getting the length of the range of the trace and squaring
	float thickness = ( trace_exit.m_vEndPos - data.enter_trace.m_vEndPos ).m_flLengthSqr( );

	if ( extracheck )
		if ( !VectortoVectorVisible( trace_exit.m_vEndPos, point, pEntity ) )
			return false;

	float modifier = fmaxf( 1.f / combined_penetration_modifier, 0.f );

	//  this calculates how much damage we've lost depending on thickness of the wall, our penetration, damage, and the modifiers set earlier
	float lost_damage = fmaxf(
		( ( modifier * thickness ) / 24.f ) //* 0.041666668
		+ ( ( data.current_damage * final_damage_modifier )
			+ ( fmaxf( 3.75 / wpn_data->m_flPenetration, 0.f ) * 3.f * modifier ) ), 0.f );

	//  did we loose too much damage?
	if ( lost_damage > data.current_damage ) {
		return false;
	}

	//  we can't use any of the damage that we've lost
	if ( lost_damage > 0.f ) {
		data.current_damage -= lost_damage;
	}

	//  do we still have enough damage to deal?
	if ( data.current_damage < 1.f ) {
		return false;
	}

	data.src = trace_exit.m_vEndPos;
	--data.penetrate_count;

	return true;
}

void Autowall::ClipTrace( const Vec3D &src, Vec3D &end, trace_t *tr, CBaseEntity* target )
{
	if ( !target )
		return;

	const Vec3D mins = target->GetCollideable( )->OBBMins( );
	const Vec3D maxs = target->GetCollideable( )->OBBMins( );

	Vec3D dir( end - src );
	dir.Normalize( );

	const Vec3D center = ( maxs + mins ) / 2;
	const Vec3D pos( center + target->m_vecOrigin( ) );

	Vec3D to = pos - src;
	const float range_along = dir.m_flDot( to );

	float range;
	if ( range_along < 0.f ) {
		range = -to.m_flLength( );
	}
	else if ( range_along > dir.m_flLength( ) ) {
		range = -( pos - end ).m_flLength( );
	}
	else {
		auto ray( pos - ( dir * range_along + src ) );
		range = ray.m_flLength( );
	}

	if ( range <= 60.f ) {
		trace_t trace;
		Interfaces::Trace->ClipRayToEntity( Ray_t( src, end ), MASK_SHOT_HULL | CONTENTS_HITBOX, target, &trace );
		if ( tr->m_flFraction > trace.m_flFraction )
			*tr = trace;
	}
}

bool Autowall::FireBullet( CBaseCombatWeapon* weapon, FireBulletData& data, CBaseEntity* pTarget ) {
	auto local = Interfaces::User();

	trace_t     exit_trace;
	WeaponInfo_t* weapon_info = weapon->m_pWeaponData( );

	data.penetrate_count = 4;
	data.trace_length = 0.f;

	if ( weapon_info == nullptr ) {
		return false;
	}

	data.current_damage = weapon_info->m_iDamage;
	data.max_range = weapon_info->m_flRange;

	//  if our damage is greater than (or equal to) 1, and we can shoot, let's shoot.
	while ( data.penetrate_count > 0 && data.current_damage >= 1.f ) {
		data.max_range -= data.trace_length;

		//  create endpoint of bullet
		auto end = ( data.direction * data.max_range ) + data.src;

		TraceLine( data.src, end, MASK_SHOT_HULL | CONTENTS_HITBOX, local, &data.enter_trace );
		if ( pTarget )
			ClipTrace( end + data.direction * 40.f, data.src, &data.enter_trace, pTarget );
	//	UtilClipTraceToPlayers( data.src, end + data.direction * 40.f, MASK_SHOT_HULL | CONTENTS_HITBOX, &data.filter, &data.enter_trace );

		//  "fraction == 1" means that we didn't hit anything. we don't want that- so let's break on it.
		if ( data.enter_trace.m_flFraction == 1.f ) {
			break;
		}

		//  calculate the damage based on the distance the bullet traveled.
		data.trace_length += data.enter_trace.m_flFraction * data.max_range;

		//  let's make our damage drops off the further away the bullet is.
		data.current_damage *= pow( weapon_info->m_flRangeModifier, ( data.trace_length / 500.f ) );

		//  get surface data
		auto enter_surface_data = Interfaces::SurfaceProps->GetSurfaceData( data.enter_trace.m_uSurface.m_uName );
		auto      enter_pen_mod = enter_surface_data->game.flPenetrationModifier;
		int      enter_material = enter_surface_data->game.material;

		if ( data.trace_length > 3000.f || enter_pen_mod < 0.1f ) {
			break;
		}

		//  this looks gay as fuck if we put it in to 1 long line of code
		bool can_do_damage = ( data.enter_trace.m_iHitGroup != HITGROUP_GEAR && data.enter_trace.m_iHitGroup != HITGROUP_GENERIC );
		bool      is_enemy = ( local->GetTeam( ) != data.enter_trace.m_pEntity->GetTeam( ) );

		//  check to see if we hit an entity, if so scale the damage
		if ( ( can_do_damage && is_enemy ) ) {

			//  scale our damage
			ScaleDamage( data.enter_trace.m_pEntity, data.enter_trace.m_iHitGroup, weapon_info, data.current_damage );

			return true;
		}

		//  if the bullet can't penetrate wall we don't want to continue looping
		if ( !HandleBulletPen( weapon_info, data ) ) {
			break;
		}
	}

	return false;
}

float Autowall::GetDamage( Vec3D point, CBaseEntity* pTarget ) {
	//  define local
	auto   local = Interfaces::User();

	//  make sure local and weapon is not nil
	if ( !local ) {
		return -1.f;
	}

	// setup fire bullet data
	FireBulletData data = FireBulletData( point, local );

	data.src = local->GetEyePosition( );

	//  find the direction from source to point
	Vec3D angles = Math::CalcAngle( data.src, point );
	Math::AngleVectors( angles, &data.direction );
	data.direction.NormalizeInPlace( );

	//  simulate a bullet being fired
	if ( FireBullet( local->GetActiveWeapon( ), data, pTarget ) ) {
		return data.current_damage;
	}

	return -1.f;
}

bool Autowall::CanHitFloatingPoint( const Vec3D &point, const Vec3D &source, CBaseEntity* pEntity ) // ez
{
	auto pLocalEnt = Interfaces::User( );

	if ( !pLocalEnt )
		return false;

	if ( !pLocalEnt->m_bIsAlive( ) )
		return false;

	FireBulletData data = FireBulletData( source, pLocalEnt );

	Vec3D angles = Math::CalcAngle( data.src, point );
	Math::AngleVectors( angles, &data.direction );
	data.direction.Normalize( );

	CBaseCombatWeapon *pWeapon = pLocalEnt->GetActiveWeapon( );

	if ( !pWeapon )
		return false;

	data.penetrate_count = 1;
	data.trace_length = 0.0f;

	WeaponInfo_t *pWeaponData = pWeapon->m_pWeaponData( );

	if ( !pWeaponData )
		return false;

	data.current_damage = ( float ) pWeaponData->m_iDamage;
	data.trace_length_remaining = pWeaponData->m_flRange - data.trace_length;
	Vec3D end = data.src + ( data.direction * data.trace_length_remaining );
	TraceLine( data.src, end, MASK_SHOT | CONTENTS_HITBOX, pLocalEnt, &data.enter_trace );

	if ( VectortoVectorVisible( data.src, point, pEntity ) || HandleBulletPen( pWeaponData, data, true, point, pEntity ) )
		return true;

	return false;
}