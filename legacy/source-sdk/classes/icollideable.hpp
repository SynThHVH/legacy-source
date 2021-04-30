#pragma once
//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

class IClientUnknown;

class Ray_t;
class trace_t;

enum SolidType_t;

class ICollideable
{
public:
	// Gets at the entity handle associated with the collideable
	virtual IHandleEntity	*GetEntityHandle( ) = 0;

	// These methods return the bounds of an OBB measured in "collision" space
	// which can be retreived through the CollisionToWorldTransform or
	// GetCollisionOrigin/GetCollisionAngles methods
//	virtual const Vec3D&	OBBMinsPreScaled( ) const = 0;
//	virtual const Vec3D&	OBBMaxsPreScaled( ) const = 0;
	virtual Vec3D&	OBBMins( ) const;
	virtual Vec3D&	OBBMaxs( ) const;
};
