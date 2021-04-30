#pragma once

namespace Visuals
{
	extern CBaseEntity* pLocalEnt;

	// esp
	extern RECT BoundingBox;
	extern int YOffset;
	extern bool bContinue;

	void Box( );
	void Name( CBaseEntity* pEntity );
	void HealthBar( CBaseEntity* pEntity );
	void ResolverInfo( CBaseEntity* pEntity );
	void Weapon( CBaseEntity* pEntity );
	void BoundBox( CBaseEntity* pEntity );

	void Esp( );
}