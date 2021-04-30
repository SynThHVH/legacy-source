#pragma once

#define CHAR_TEX_ANTLION		'A'
#define CHAR_TEX_BLOODYFLESH	'B'
#define	CHAR_TEX_CONCRETE		'C'
#define CHAR_TEX_DIRT			'D'
#define CHAR_TEX_EGGSHELL		'E' ///< the egg sacs in the tunnels in ep2.
#define CHAR_TEX_FLESH			'F'
#define CHAR_TEX_GRATE			'G'
#define CHAR_TEX_ALIENFLESH		'H'
#define CHAR_TEX_CLIP			'I'
//#define CHAR_TEX_UNUSED		'J'
#define CHAR_TEX_SNOW			'K'
#define CHAR_TEX_PLASTIC		'L'
#define CHAR_TEX_METAL			'M'
#define CHAR_TEX_SAND			'N'
#define CHAR_TEX_FOLIAGE		'O'
#define CHAR_TEX_COMPUTER		'P'
//#define CHAR_TEX_UNUSED		'Q'
#define CHAR_TEX_REFLECTIVE		'R'
#define CHAR_TEX_SLOSH			'S'
#define CHAR_TEX_TILE			'T'
#define CHAR_TEX_CARDBOARD		'U'
#define CHAR_TEX_VENT			'V'
#define CHAR_TEX_WOOD			'W'
//#define CHAR_TEX_UNUSED		'X' ///< do not use - "fake" materials use this (ladders, wading, clips, etc)
#define CHAR_TEX_GLASS			'Y'
#define CHAR_TEX_WARPSHIELD		'Z' ///< wierd-looking jello effect for advisor shield.

#define CHAR_TEX_STEAM_PIPE		11


struct FireBulletData
{
	FireBulletData( const Vec3D &eyePos, CBaseEntity* entity ) : src( eyePos ), filter( entity )
	{
	}

	Vec3D          src;
	trace_t        enter_trace;
	Vec3D          direction;
	CTraceFilter   filter;
	float          trace_length;
	float          trace_length_remaining;
	float          current_damage;
	int            penetrate_count;
	float          max_range;
};

namespace Autowall
{
	extern void ClipTrace( const Vec3D &src, Vec3D &end, trace_t *tr, CBaseEntity* target );
	extern void TraceLine( Vec3D& start, Vec3D& end, unsigned int mask, CBaseEntity* ignore, trace_t* ptr );
	extern bool VectortoVectorVisible( Vec3D src, Vec3D point, CBaseEntity* pEntity );
	extern bool Breakable( CBaseEntity* pEntity );
	extern void ScaleDamage( CBaseEntity* pEntity, int , WeaponInfo_t* weapon_data, float& current_damage );
	extern bool TraceToExit( trace_t& enter_trace, trace_t& exit_trace, Vec3D start_position, Vec3D dir );
	extern bool HandleBulletPen( WeaponInfo_t* wpn_data, FireBulletData& data, bool extracheck, Vec3D point, CBaseEntity* pEntity );
	extern bool FireBullet( CBaseCombatWeapon* weapon, FireBulletData& data, CBaseEntity* pTarget = nullptr );
	extern float GetDamage( Vec3D point, CBaseEntity* pTarget = nullptr );
	extern bool CanHitFloatingPoint( const Vec3D &point, const Vec3D &source, CBaseEntity* pEntity = nullptr );
}