#pragma once
//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//===========================================================================//


//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------

struct cplane_t;

struct mstudiobbox_t
{
	int		bone;				// 0x0000 
	int		group;				// 0x0004 
	Vec3D	min;				// 0x0008 
	Vec3D	max;				// 0x0014 
	int		nameidx;			// 0x0020 
	char	pad1[ 0xC ];	// 0x0024
	float	radius;				// 0x0030 
	char	pad2[ 0x10 ];	// 0x0034
};

struct mstudiohitboxset_t
{
	int                                             sznameindex;
	inline char* const              GetName( void ) const { return ( ( char* ) this ) + sznameindex; }
	int                                             numhitboxes;
	int                                             hitboxindex;
	inline mstudiobbox_t*   GetHitbox( int i ) const { return ( mstudiobbox_t* ) ( ( ( BYTE* ) this ) + hitboxindex ) + i; };
};

struct mstudiobone_t
{
	int                                     sznameindex;
	inline char * const GetName( void ) const { return ( ( char * ) this ) + sznameindex; }
	int                                     parent;
	int                                     bonecontroller[ 6 ];

	Vec3D                          pos;
	float                           quat[ 4 ];
	Vec3D                          rot;
	Vec3D                          posscale;
	Vec3D                          rotscale;

	matrix3x4_t                     poseToBone;
	float                           qAlignment[ 4 ];
	int                                     flags;
	int                                     proctype;
	int                                     procindex;              // procedural rule
	mutable int                     physicsbone;    // index into physically simulated bone
	inline void *           GetProcedure( ) const { if ( procindex == 0 ) return NULL; else return  ( void * ) ( ( ( BYTE * ) this ) + procindex ); };
	int                                     surfacepropidx; // index into string tablefor property name
	inline char * const GetSurfaceProps( void ) const { return ( ( char * ) this ) + surfacepropidx; }
	int                                     contents;               // See BSPFlags.h for the contents flags

	int                                     unused[ 8 ];              // remove as appropriate
};

struct studiohdr_t
{
	int                                     id;
	int                                     version;

	int                                     checksum;

	char                            name[ 64 ];
	int                                     length;


	Vec3D                          eyeposition;

	Vec3D                          illumposition;

	Vec3D                          hull_min;
	Vec3D                          hull_max;

	Vec3D                          view_bbmin;
	Vec3D                          view_bbmax;

	int                                     flags;

	int                                     numbones;
	int                                     boneindex;

	inline mstudiobone_t *GetBone( int i ) const { return ( mstudiobone_t * ) ( ( ( BYTE * ) this ) + boneindex ) + i; };
	//	inline mstudiobone_t *pBone(int i) const { Assert(i >= 0 && i < numbones); return (mstudiobone_t *)(((byte *)this) + boneindex) + i; };

	int                                     numbonecontrollers;
	int                                     bonecontrollerindex;

	int                                     numhitboxsets;
	int                                     hitboxsetindex;

	mstudiohitboxset_t* GetHitboxSet( int i ) const
	{
		return ( mstudiohitboxset_t* ) ( ( ( BYTE* ) this ) + hitboxsetindex ) + i;
	}

	inline mstudiobbox_t* GetHitbox( int i, int set ) const
	{
		mstudiohitboxset_t const* s = GetHitboxSet( set );

		if ( !s )
			return NULL;

		return s->GetHitbox( i );
	}

	inline int GetHitboxCount( int set ) const
	{
		mstudiohitboxset_t const* s = GetHitboxSet( set );

		if ( !s )
			return 0;

		return s->numhitboxes;
	}

	int                                     numlocalanim;
	int                                     localanimindex;

	int                                     numlocalseq;
	int                                     localseqindex;

	mutable int                     activitylistversion;
	mutable int                     eventsindexed;

	int                                     numtextures;
	int                                     textureindex;

	int                                     numcdtextures;
	int                                     cdtextureindex;

	int                                     numskinref;
	int                                     numskinfamilies;
	int                                     skinindex;

	int                                     numbodyparts;
	int                                     bodypartindex;

	int                                     numlocalattachments;
	int                                     localattachmentindex;

	int                                     numlocalnodes;
	int                                     localnodeindex;
	int                                     localnodenameindex;

	int                                     numflexdesc;
	int                                     flexdescindex;

	int                                     numflexcontrollers;
	int                                     flexcontrollerindex;

	int                                     numflexrules;
	int                                     flexruleindex;

	int                                     numikchains;
	int                                     ikchainindex;

	int                                     nummouths;
	int                                     mouthindex;

	int                                     numlocalposeparameters;
	int                                     localposeparamindex;

	int                                     surfacepropindex;

	int                                     keyvalueindex;
	int                                     keyvaluesize;


	int                                     numlocalikautoplaylocks;
	int                                     localikautoplaylockindex;

	float                           mass;
	int                                     contents;

	int                                     numincludemodels;
	int                                     includemodelindex;

	mutable void            *virtualModel;

	int                                     szanimblocknameindex;
	int                                     numanimblocks;
	int                                     animblockindex;

	mutable void            *animblockModel;

	int                                     bonetablebynameindex;

	void                            *pVertexBase;
	void                            *pIndexBase;

	BYTE                            constdirectionallightdot;

	BYTE                            rootLOD;

	BYTE                            numAllowedRootLODs;

	BYTE                            unused[ 1 ];

	int                                     unused4;

	int                                     numflexcontrollerui;
	int                                     flexcontrolleruiindex;
	float                           flVertAnimFixedPointScale;
	int                                     unused3[ 1 ];
	int                                     studiohdr2index;
	int                                     unused2[ 1 ];
};

struct virtualmodel_t;

//-----------------------------------------------------------------------------
// Purpose: a callback class that is notified when a model has finished loading
//-----------------------------------------------------------------------------
class IModelLoadCallback
{
public:
	virtual void OnModelLoadComplete( const model_t* pModel ) = 0;

protected:
	// Protected destructor so that nobody tries to delete via this interface.
	// Automatically unregisters if the callback is destroyed while still pending.
	~IModelLoadCallback( );
};


//-----------------------------------------------------------------------------
// Purpose: Automate refcount tracking on a model index
//-----------------------------------------------------------------------------
class CRefCountedModelIndex
{
private:
	int m_nIndex;
public:
	CRefCountedModelIndex( ) : m_nIndex( -1 ) { }
	~CRefCountedModelIndex( ) { Set( -1 ); }

	CRefCountedModelIndex( const CRefCountedModelIndex& src ) : m_nIndex( -1 ) { Set( src.m_nIndex ); }
	CRefCountedModelIndex& operator=( const CRefCountedModelIndex& src ) { Set( src.m_nIndex ); return *this; }

	explicit CRefCountedModelIndex( int i ) : m_nIndex( -1 ) { Set( i ); }
	CRefCountedModelIndex& operator=( int i ) { Set( i ); return *this; }

	int Get( ) const { return m_nIndex; }
	void Set( int i );
	void Clear( ) { Set( -1 ); }

	operator int( ) const { return m_nIndex; }
};


//-----------------------------------------------------------------------------
// Model info interface
//-----------------------------------------------------------------------------

// change this when the new version is incompatable with the old
#define VMODELINFO_CLIENT_INTERFACE_VERSION		"VModelInfoClient006"
#define VMODELINFO_SERVER_INTERFACE_VERSION_3	"VModelInfoServer003"
#define VMODELINFO_SERVER_INTERFACE_VERSION		"VModelInfoServer004"

// MODEL INDEX RULES
// If index >= 0, then index references the precached model string table
// If index == -1, then the model is invalid
// If index < -1, then the model is DYNAMIC and has a DYNAMIC INDEX of (-2 - index)
// - if the dynamic index is ODD, then the model is CLIENT ONLY
//   and has a m_LocalDynamicModels lookup index of (dynamic index)>>1
// - if the dynamic index is EVEN, then the model is NETWORKED
//   and has a dynamic model string table index of (dynamic index)>>1

inline bool IsDynamicModelIndex( int modelindex ) { return modelindex < -1; }
inline bool IsClientOnlyModelIndex( int modelindex ) { return modelindex < -1 && ( modelindex & 1 ); }

class IVModelInfo
{
public:

	VF( GetModel, inline void*, 1, int Index );

	VF( GetModelIndex, int, 2, const char *name );

	VF( GetModelName, const char*, 3, const model_t *mod );

	VF( GetModelMaterials, void, 17, const model_t *model, int count, IMaterial** ppMaterial );

	VF( GetStudiomodel, studiohdr_t	*, 30, const model_t *mod );
};

typedef IVModelInfo IVModelInfo003;

class IVModelInfoClient : public IVModelInfo
{
public:
	virtual void OnDynamicModelsStringTableChange( int nStringIndex, const char *pString, const void *pData ) = 0;

	// For tools only!
	virtual const model_t *FindOrLoadModel( const char *name ) = 0;
};

struct virtualterrainparams_t
{
	// UNDONE: Add grouping here, specified in BSP file? (test grouping to see if this is necessary)
	int index;
};

//-----------------------------------------------------------------------------
// Force removal from callback list on destruction to avoid crashes.
//-----------------------------------------------------------------------------
inline IModelLoadCallback::~IModelLoadCallback( )
{
#ifdef CLIENT_DLL
	extern IVModelInfoClient *modelinfo;
#else
	extern IVModelInfo *modelinfo;
#endif
	if ( modelinfo )
	{
	//	modelinfo->UnregisterModelLoadCallback( -1, this );
	}
}

//-----------------------------------------------------------------------------
// Purpose: Automate refcount tracking on a model index
//-----------------------------------------------------------------------------
inline void CRefCountedModelIndex::Set( int i )
{
#ifdef CLIENT_DLL
	extern IVModelInfoClient *modelinfo;
#else
	extern IVModelInfo *modelinfo;
#endif
	if ( i == m_nIndex )
		return;
//	modelinfo->AddRefDynamicModel( i );
//	modelinfo->ReleaseDynamicModel( m_nIndex );
	m_nIndex = i;
}