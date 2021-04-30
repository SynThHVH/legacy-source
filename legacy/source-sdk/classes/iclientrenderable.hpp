#pragma once
//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

//-----------------------------------------------------------------------------
// Handles to a client shadow
//-----------------------------------------------------------------------------
typedef unsigned short ClientShadowHandle_t;

enum
{
	CLIENTSHADOW_INVALID_HANDLE = ( ClientShadowHandle_t ) ~0
};

//-----------------------------------------------------------------------------
// What kind of shadows to render?
//-----------------------------------------------------------------------------
enum ShadowType_t
{
	SHADOWS_NONE = 0,
	SHADOWS_SIMPLE,
	SHADOWS_RENDER_TO_TEXTURE,
	SHADOWS_RENDER_TO_TEXTURE_DYNAMIC,	// the shadow is always changing state
	SHADOWS_RENDER_TO_DEPTH_TEXTURE,
};


// This provides a way for entities to know when they've entered or left the PVS.
// Normally, server entities can use NotifyShouldTransmit to get this info, but client-only
// entities can use this. Store a CPVSNotifyInfo in your 
//
// When bInPVS=true, it's being called DURING rendering. It might be after rendering any
// number of views.
//
// If no views had the entity, then it is called with bInPVS=false after rendering.
class IPVSNotify
{
public:
	virtual void OnPVSStatusChanged( bool bInPVS ) = 0;
};


//-----------------------------------------------------------------------------
// Purpose: All client entities must implement this interface.
//-----------------------------------------------------------------------------
class IClientRenderable
{
public:
	virtual IClientUnknown*            GetIClientUnknown( ) = 0;
	virtual Vec3D const&               GetRenderOrigin( void ) = 0;
	virtual Vec3D const&               GetRenderAngles( void ) = 0;
	virtual bool                       ShouldDraw( void ) = 0;
	virtual int                        GetRenderFlags( void ) = 0; // ERENDERFLAGS_xxx
	virtual void                       Unused( void ) const {}
	virtual ClientShadowHandle_t       GetShadowHandle( ) const = 0;
	virtual ClientRenderHandle_t&      RenderHandle( ) = 0;
	virtual model_t*                   GetModel( ) const = 0;
	virtual int                        DrawModel( int flags, const int &instance ) = 0;
	virtual int                        GetBody( ) = 0;
	virtual void                       GetColorModulation( float* color ) = 0;
	virtual bool                       LODTest( ) = 0;
	virtual bool                       SetupBones( matrix3x4_t *pBoneToWorldOut, int nMaxBones, int boneMask, float currentTime ) = 0;
	virtual void                       SetupWeights( const matrix3x4_t *pBoneToWorld, int nFlexWeightCount, float *pFlexWeights, float *pFlexDelayedWeights ) = 0;
	virtual void                       DoAnimationEvents( void ) = 0;
	virtual void* /*IPVSNotify*/       GetPVSNotifyInterface( ) = 0;
	virtual void                       GetRenderBounds( Vec3D& mins, Vec3D& maxs ) = 0;
	virtual void                       GetRenderBoundsWorldspace( Vec3D& mins, Vec3D& maxs ) = 0;
	virtual void                       GetShadowRenderBounds( Vec3D &mins, Vec3D &maxs, int /*ShadowType_t*/ shadowType ) = 0;
	virtual bool                       ShouldReceiveProjectedTextures( int flags ) = 0;
	virtual bool                       GetShadowCastDistance( float *pDist, int /*ShadowType_t*/ shadowType ) const = 0;
	virtual bool                       GetShadowCastDirection( Vec3D *pDirection, int /*ShadowType_t*/ shadowType ) const = 0;
	virtual bool                       IsShadowDirty( ) = 0;
	virtual void                       MarkShadowDirty( bool bDirty ) = 0;
	virtual IClientRenderable*         GetShadowParent( ) = 0;
	virtual IClientRenderable*         FirstShadowChild( ) = 0;
	virtual IClientRenderable*         NextShadowPeer( ) = 0;
	virtual int /*ShadowType_t*/       ShadowCastType( ) = 0;
	virtual void                       CreateModelInstance( ) = 0;
	virtual ModelInstanceHandle_t      GetModelInstance( ) = 0;
	virtual const matrix3x4_t&         RenderableToWorldTransform( ) = 0;
	virtual int                        LookupAttachment( const char *pAttachmentName ) = 0;
	virtual   bool                     GetAttachment( int number, Vec3D &origin, Vec3D &angles ) = 0;
	virtual bool                       GetAttachment( int number, matrix3x4_t &matrix ) = 0;
	virtual float*                     GetRenderClipPlane( void ) = 0;
	virtual int                        GetSkin( ) = 0;
	virtual void                       OnThreadedDrawSetup( ) = 0;
	virtual bool                       UsesFlexDelayedWeights( ) = 0;
	virtual void                       RecordToolMessage( ) = 0;
	virtual bool                       ShouldDrawForSplitScreenUser( int nSlot ) = 0;
	virtual int                        OverrideAlphaModulation( int nAlpha ) = 0;
	virtual int                        OverrideShadowAlphaModulation( int nAlpha ) = 0;
};
