#pragma once

namespace Resolver
{
	extern float flOldGoalFeetYaw[ MAX_NETWORKID_LENGTH ];
	extern float flGoalFeetYaw[ MAX_NETWORKID_LENGTH ];
	extern Vec3D vOldEyeAng[ MAX_NETWORKID_LENGTH ];
	extern bool bUseFreestandAngle[ MAX_NETWORKID_LENGTH ];
	extern float flFreestandAngle[ MAX_NETWORKID_LENGTH ];
	extern float flLastFreestandAngle[ MAX_NETWORKID_LENGTH ];
	extern float flLastFreestandAngle[ MAX_NETWORKID_LENGTH ];
	extern float flGoalFeetYawB[ MAX_NETWORKID_LENGTH ];
	extern float flLbyB[ MAX_NETWORKID_LENGTH ];
	extern int iMode[MAX_NETWORKID_LENGTH];
	extern int iHitCountMovingMode[MAX_NETWORKID_LENGTH][4];
	extern int iHitCountStandingMode[MAX_NETWORKID_LENGTH][4];
	extern int iBestMovingMode[MAX_NETWORKID_LENGTH];
	extern int iBestStandingMode[MAX_NETWORKID_LENGTH];
	extern bool bCanUpdate[MAX_NETWORKID_LENGTH];

	extern std::string ResolverMode[ MAX_NETWORKID_LENGTH ];

	void PreResolver( CPlayerAnimstate* AnimState, CBaseEntity* pEntity, bool bShot, bool& bLbyUpdated );

	void ResolverPoints( );
}