#pragma once

class CBaseEntity;

enum Contents 
{
	CONTENTS_EMPTY = 0x0,
	CONTENTS_SOLID = 0x1,
	CONTENTS_WINDOW = 0x2,
	CONTENTS_AUX = 0x4,
	CONTENTS_GRATE = 0x8,
	CONTENTS_SLIME = 0x10,
	CONTENTS_WATER = 0x20,
	CONTENTS_BLOCKLOS = 0x40,
	CONTENTS_OPAQUE = 0x80,
	CONTENTS_TESTFOGVOLUME = 0x100,
	CONTENTS_UNUSED = 0x200,
	CONTENTS_BLOCKLIGHT = 0x400,
	CONTENTS_TEAM1 = 0x800,
	CONTENTS_TEAM2 = 0x1000,
	CONTENTS_IGNORE_NODRAW_OPAQUE = 0x2000,
	CONTENTS_MOVEABLE = 0x4000,
	CONTENTS_AREAPORTAL = 0x8000,
	CONTENTS_PLAYERCLIP = 0x10000,
	CONTENTS_MONSTERCLIP = 0x20000,
	CONTENTS_CURRENT0 = 0x40000,
	CONTENTS_CURRENT90 = 0x80000,
	CONTENTS_CURRENT180 = 0x100000,
	CONTENTS_CURRENT270 = 0x200000,
	CONTENTS_CURRENT_UP = 0x400000,
	CONTENTS_CURRENT_DOWN = 0x800000,
	CONTENTS_ORIGIN = 0x1000000,
	CONTENTS_MONSTER = 0x2000000,
	CONTENTS_DEBRIS = 0x4000000,
	CONTENTS_DETAIL = 0x8000000,
	CONTENTS_TRANSLUCENT = 0x10000000,
	CONTENTS_LADDER = 0x20000000,
	CONTENTS_HITBOX = 0x40000000,

	LastVisibleContents = CONTENTS_OPAQUE,
	AllVisibleContents = LastVisibleContents | LastVisibleContents - 1
};

enum Surf 
{
	SURF_LIGHT = 0x1,
	SURF_SKY2D = 0x2,
	SURF_SKY = 0x4,
	SURF_WARP = 0x8,
	SURF_TRANS = 0x10,
	SURF_NOPORTAL = 0x20,
	SURF_TRIGGER = 0x40,
	SURF_NODRAW = 0x80,
	SURF_HINT = 0x100,
	SURF_SKIP = 0x200,
	SURF_NOLIGHT = 0x400,
	SURF_BUMPLIGHT = 0x800,
	SURF_NOSHADOWS = 0x1000,
	SURF_NODECALS = 0x2000,
	SURF_NOPAINT = SURF_NODECALS,
	SURF_NOCHOP = 0x4000,
	SURF_HITBOX = 0x8000
};

enum Masks
{
	MASK_ALL = 0xFFFFFFFF,
	MASK_SOLID = ( CONTENTS_SOLID | CONTENTS_MOVEABLE | CONTENTS_WINDOW | CONTENTS_MONSTER | CONTENTS_GRATE ),
	MASK_PLAYERSOLID = ( CONTENTS_SOLID | CONTENTS_MOVEABLE | CONTENTS_PLAYERCLIP | CONTENTS_WINDOW | CONTENTS_MONSTER | CONTENTS_GRATE ),
	MASK_NPCSOLID = ( CONTENTS_SOLID | CONTENTS_MOVEABLE | CONTENTS_MONSTERCLIP | CONTENTS_WINDOW | CONTENTS_MONSTER | CONTENTS_GRATE ),
	MASK_NPCFLUID = ( CONTENTS_SOLID | CONTENTS_MOVEABLE | CONTENTS_MONSTERCLIP | CONTENTS_WINDOW | CONTENTS_MONSTER ),
	MASK_WATER = ( CONTENTS_WATER | CONTENTS_MOVEABLE | CONTENTS_SLIME ),
	MASK_OPAQUE = ( CONTENTS_WATER | CONTENTS_MOVEABLE | CONTENTS_OPAQUE ),
	MASK_OPAQUE_NPC = ( MASK_OPAQUE | CONTENTS_MONSTER ),
	MASK_BLOCKLOS = ( CONTENTS_SOLID | CONTENTS_MOVEABLE | CONTENTS_SLIME ),
	MASK_BLOCKLOS_NPC = ( MASK_BLOCKLOS | CONTENTS_MONSTER ),
	MASK_VISIBLE = ( MASK_OPAQUE | CONTENTS_IGNORE_NODRAW_OPAQUE ),
	MASK_VISIBLE_NPC = ( MASK_OPAQUE_NPC | CONTENTS_IGNORE_NODRAW_OPAQUE ),
	MASK_SHOT = ( CONTENTS_SOLID | CONTENTS_MOVEABLE | CONTENTS_MONSTER | CONTENTS_WINDOW | CONTENTS_DEBRIS | CONTENTS_HITBOX ),
	MASK_SHOT_BRUSHONLY = ( CONTENTS_SOLID | CONTENTS_MOVEABLE | CONTENTS_WINDOW | CONTENTS_DEBRIS ),
	MASK_SHOT_HULL = ( CONTENTS_SOLID | CONTENTS_MOVEABLE | CONTENTS_MONSTER | CONTENTS_WINDOW | CONTENTS_DEBRIS | CONTENTS_GRATE ),
	MASK_SHOT_PORTAL = ( CONTENTS_SOLID | CONTENTS_MOVEABLE | CONTENTS_WINDOW | CONTENTS_MONSTER ),
	MASK_SOLID_BRUSHONLY = ( CONTENTS_SOLID | CONTENTS_MOVEABLE | CONTENTS_WINDOW | CONTENTS_GRATE ),
	MASK_PLAYERSOLID_BRUSHONLY = ( CONTENTS_SOLID | CONTENTS_MOVEABLE | CONTENTS_WINDOW | CONTENTS_PLAYERCLIP | CONTENTS_GRATE ),
	MASK_NPCSOLID_BRUSHONLY = ( CONTENTS_SOLID | CONTENTS_MOVEABLE | CONTENTS_WINDOW | CONTENTS_MONSTERCLIP | CONTENTS_GRATE ),
	MASK_NPCWORLDSTATIC = ( CONTENTS_SOLID | CONTENTS_WINDOW | CONTENTS_MONSTERCLIP | CONTENTS_GRATE ),
	MASK_NPCWORLDSTATIC_FLUID = ( CONTENTS_SOLID | CONTENTS_WINDOW | CONTENTS_MONSTERCLIP ),
	MASK_SPLITAREAPORTAL = ( CONTENTS_WATER | CONTENTS_SLIME ),
	MASK_CURRENT = ( CONTENTS_CURRENT0 | CONTENTS_CURRENT90 | CONTENTS_CURRENT180 | CONTENTS_CURRENT270 | CONTENTS_CURRENT_UP | CONTENTS_CURRENT_DOWN ),
	MASK_DEADSOLID = ( CONTENTS_SOLID | CONTENTS_PLAYERCLIP | CONTENTS_WINDOW | CONTENTS_GRATE )
};

enum TraceType_t
{
	TRACE_EVERYTHING = 0,
	TRACE_WORLD_ONLY,
	TRACE_ENTITIES_ONLY,
	TRACE_EVERYTHING_FILTER_PROPS,
};

struct Ray_t 
{
public:
	VectorAligned m_Start; // starting point, centered within the extents
	VectorAligned m_Delta; // direction + length of the ray
	VectorAligned m_StartOffset; // Add this to m_Start to get the actual ray start
	VectorAligned m_Extents; // Describes an axis aligned box extruded along a ray
	const matrix3x4_t* m_pWorldAxisTransform;
	//const matrix3x4_t *m_pWorldAxisTransform;
	bool m_IsRay; // are the extents zero?
	bool m_IsSwept; // is delta != 0?


	Ray_t( Vec3D _start, Vec3D _end )
	{
		Init( _start, _end );
	}

	Ray_t( Vec3D _start, Vec3D _end, Vec3D _mins, Vec3D _maxs )
	{
		Init( _start, _end, _mins, _maxs );
	}

	void Init( const Vec3D& vecStart, const Vec3D& vecEnd )
	{
		m_Delta = vecEnd - vecStart;

		m_IsSwept = ( m_Delta.m_flLengthSqr( ) != 0 );

		m_Extents.x = m_Extents.y = m_Extents.z = 0.0f;
		m_IsRay = true;

		m_StartOffset.x = m_StartOffset.y = m_StartOffset.z = 0.0f;

		m_Start = vecStart;
	}

	void Init( Vec3D& vecStart, Vec3D& vecEnd, Vec3D min, Vec3D max )
	{
		m_Delta = vecEnd - vecStart;

		m_IsSwept = ( m_Delta.m_flLengthSqr( ) != 0 );

		m_Extents.x = ( max.x - min.x );
		m_Extents.y = ( max.y - min.y );
		m_Extents.z = ( max.z - min.z );
		m_IsRay = false;

		m_StartOffset.x = m_StartOffset.y = m_StartOffset.z = 0.0f;

		m_Start = vecStart + ( ( max + min ) * 0.5f );
	}
};

class CTraceFilter 
{
public:
	CBaseEntity* m_pSkipEntity;
	CBaseEntity* m_pSkipEntityTwo;
	TraceType_t m_uType;

	CTraceFilter( CBaseEntity* pEntity, TraceType_t uTraceType = TRACE_EVERYTHING ) 
	{
		m_pSkipEntity = pEntity;
	}

	virtual bool m_bShouldHit( CBaseEntity* pEntity, int iMask )
	{
		return ( pEntity != m_pSkipEntity && pEntity != m_pSkipEntity );
	}

	virtual TraceType_t m_uTraceEverything( )
	{
		return TRACE_EVERYTHING;
	}

	virtual TraceType_t m_uGetTraceType( ) 
	{
		return m_uType;
	}
};

class trace_t 
{
public:
	bool m_bDispSurface( ) const 
	{
		return ( m_uDispFlags & 1 << 0 ) != 0;
	}

	bool m_bDispWalkable( ) const 
	{
		return ( m_uDispFlags & 1 << 1 ) != 0;
	}

	bool m_bDispBuildable( ) const 
	{
		return ( m_uDispFlags & 1 << 2 ) != 0;
	}

	bool m_bDispPropOne( ) const 
	{
		return ( m_uDispFlags & 1 << 3 ) != 0;
	}

	bool m_bDispPropTwo( ) const 
	{
		return ( m_uDispFlags & 1 << 4 ) != 0;
	}

	Vec3D m_vStartPos;
	Vec3D m_vEndPos;
	plane_t m_uPlane;
	float m_flFraction;
	int m_iContents;
	unsigned short m_uDispFlags;
	bool m_bAllSolid;
	bool m_bStartSolid;

	float m_flFractionLeftSolid;
	surface_t m_uSurface;
	int m_iHitGroup;
	short m_uPhysicsBone;
	unsigned short m_uWorldSurfaceIndex;
	CBaseEntity *m_pEntity;
	int m_iHitbox;

	bool m_bDidHit( ) const 
	{
		return m_flFraction < 1.f || m_bAllSolid || m_bStartSolid;
	}

	bool m_bDidHitNonWorldEntity( ) const 
	{
		return m_pEntity != nullptr;
	}
};

class IEngineTrace 
{
public:
	VF( m_iGetPointContents, int, 0, const Vec3D& vEnd, int iMask, CBaseEntity* pEntity );
	VF( ClipRayToEntity, void, 3, const Ray_t& uRay, int iMask, CBaseEntity* pEntity, trace_t* uTrace );
	VF( TraceRay, void, 5, const Ray_t& uRay, int iMask, CTraceFilter *uFilter, trace_t *uTrace );

	trace_t m_uTrace( Vec3D vStart, Vec3D vEnd, int iMask, CBaseEntity* pFilter = nullptr ) 
	{
		trace_t uTrace;

		CTraceFilter uFilter( pFilter );

		TraceRay( Ray_t( vStart, vEnd ), iMask, &uFilter, &uTrace );

		return uTrace;
	}
};