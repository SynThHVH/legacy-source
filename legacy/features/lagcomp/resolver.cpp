#include "../features.hpp"

/*
note:
	so, i revamped the fuck out of the resolver, added more resolver modes, and optimized it a lot
	overall this resolver is a lot better than the one that was in this before, i can say that for a fact
*/

/* identifier: resolver variables */
namespace Resolver
{
	float flOldGoalFeetYaw[MAX_NETWORKID_LENGTH];
	float flGoalFeetYaw[MAX_NETWORKID_LENGTH];
	Vec3D vOldEyeAng[MAX_NETWORKID_LENGTH];
	bool bUseFreestandAngle[MAX_NETWORKID_LENGTH];
	float flFreestandAngle[MAX_NETWORKID_LENGTH];
	float flLastFreestandAngle[MAX_NETWORKID_LENGTH];
	float flGoalFeetYawB[MAX_NETWORKID_LENGTH];
	float flLbyB[MAX_NETWORKID_LENGTH];
	std::string ResolverMode[MAX_NETWORKID_LENGTH];

	int iMode[MAX_NETWORKID_LENGTH]; /* identifiers: 0 = undecided | 1 = inverse side | 2 = extend angle | 3 = eye angles */
	bool bCanUpdate[MAX_NETWORKID_LENGTH];
}

/* note: store freestanding points for resolving later */
void Resolver::ResolverPoints()
{
	/* note: reset freestanding points */
	if (Globals::FreeStandPoints.size() > 0) Globals::FreeStandPoints.clear();

	/* identifier: sanity check */
	if (!Menu::Config.ragebot) return;

	/* note: get our local entity */
	auto pLocalEnt = Interfaces::User();

	/* note: local sanity checks */
	if (!pLocalEnt) return;
	/* note: are we alive? */
	if (!pLocalEnt->m_bIsAlive()) return;
	/* note: are we holding a weapon? */
	if (!pLocalEnt->GetActiveWeapon()) return;

	/* note: get all entities */
	for (int index = 1; index < MAX_NETWORKID_LENGTH; ++index)
	{
		/* note: identity which entity we are targeting */
		CBaseEntity* pPlayerEntity = Interfaces::EntityList->m_pGetClientEntity(index);

		/* note: targeted entity sanity checks */
		if (!pPlayerEntity
			|| !pPlayerEntity->m_bIsAlive()

			|| pLocalEnt->entindex() == index
			|| pPlayerEntity->IsDormant()
			|| pLocalEnt->GetTeam() == pPlayerEntity->GetTeam()
			|| !LagComp::pMatrix[index])
		{
			continue;
		}

		/* note: note that our targeted entity isn't using freestanding */
		bUseFreestandAngle[index] = false;

		//	if ( !bUseFreestandAngle[ index ] )
		{
			/* identifier: freestanding variables */
			bool Autowalled = false, HitSide1 = false, HitSide2 = false;

			/* identifier: angle math */
			float angToLocal = Math::CalcAngle(pLocalEnt->m_vecOrigin(), pPlayerEntity->m_vecOrigin()).y;
			Vec3D ViewPoint = pLocalEnt->m_vecOrigin() + Vec3D(0, 0, 80);

			Vec2D Side1 = Vec2D((40 * sin(Math::m_flDegToRad(angToLocal))), (40 * cos(Math::m_flDegToRad(angToLocal))));
			Vec2D Side2 = Vec2D((40 * sin(Math::m_flDegToRad(angToLocal + 180))), (40 * cos(Math::m_flDegToRad(angToLocal + 180))));

			Vec2D Side3 = Vec2D((50 * sin(Math::m_flDegToRad(angToLocal))), (50 * cos(Math::m_flDegToRad(angToLocal))));
			Vec2D Side4 = Vec2D((50 * sin(Math::m_flDegToRad(angToLocal + 180))), (50 * cos(Math::m_flDegToRad(angToLocal + 180))));

			Vec3D Origin = pPlayerEntity->m_vecOrigin();

			Vec2D OriginLeftRight[] = { Vec2D(Side1.x, Side1.y), Vec2D(Side2.x, Side2.y) };

			Vec2D OriginLeftRightLocal[] = { Vec2D(Side3.x, Side3.y), Vec2D(Side4.x, Side4.y) };

			if (!Autowall::CanHitFloatingPoint(LagComp::pMatrix[index][8].GetOrigin(), pLocalEnt->GetEyePosition(), pPlayerEntity))
			{
				for (int side = 0; side < 2; side++)
				{
					Vec3D OriginAutowall = Vec3D(Origin.x + OriginLeftRight[side].x, Origin.y - OriginLeftRight[side].y, LagComp::pMatrix[index][8].GetOrigin().z);
					Vec3D OriginAutowall2 = Vec3D(ViewPoint.x + OriginLeftRightLocal[side].x, ViewPoint.y - OriginLeftRightLocal[side].y, ViewPoint.z);

					Globals::FreeStandPoints.push_back(OriginAutowall);

					Globals::FreeStandPoints.push_back(OriginAutowall2);

					if (Autowall::CanHitFloatingPoint(OriginAutowall, ViewPoint, pPlayerEntity))
					{
						if (side == 0)
						{
							HitSide1 = true;
							flFreestandAngle[index] = 90;
						}
						else if (side == 1)
						{
							HitSide2 = true;
							flFreestandAngle[index] = -90;
						}

						Autowalled = true;
					}
					else
					{
						for (int side222 = 0; side222 < 2; side222++)
						{
							Vec3D OriginAutowall222 = Vec3D(Origin.x + OriginLeftRight[side222].x, Origin.y - OriginLeftRight[side222].y, LagComp::pMatrix[index][8].GetOrigin().z);

							Globals::FreeStandPoints.push_back(OriginAutowall222);

							if (Autowall::CanHitFloatingPoint(OriginAutowall222, OriginAutowall2, pPlayerEntity))
							{
								if (side222 == 0)
								{
									HitSide1 = true;
									flFreestandAngle[index] = 90;
								}
								else if (side222 == 1)
								{
									HitSide2 = true;
									flFreestandAngle[index] = -90;
								}

								Autowalled = true;
							}
						}
					}
				}
			}

			if (Autowalled)
			{
				if (HitSide1 && HitSide2)
					bUseFreestandAngle[index] = false;
				else
				{
					bUseFreestandAngle[index] = true;
					flLastFreestandAngle[index] = flFreestandAngle[index];
				}
			}
		}
	}
}

/* note: start resolving */
void Resolver::PreResolver(CPlayerAnimstate* AnimState, CBaseEntity* pEntity, bool bShot, bool& bLbyUpdated)
{
	/* note: sanity checks */
	if (!Menu::Config.ragebot) return;
	if (!AnimState) return;
	if (!pEntity) return;

	/* note: get player index */
	int iIndex = pEntity->entindex();

	/* note: get our local entity */
	auto pLocalEnt = Interfaces::User();

	/* identifier: local sanity checks */
	if (!pLocalEnt->m_bIsAlive()) return;
	if (pLocalEnt->entindex() == iIndex) return;
	if (pLocalEnt->GetTeam() == pEntity->GetTeam()) return;

	/* identifier: moving variables */
	float flVelocity = pEntity->m_vecVelocity().m_flLength2D();
	bool bMoving = (flVelocity > 0.1f);
	static bool bBreakingLBY = false;

	/* note: state that your entity isn't being resolved */
	/* note: i also put this here so it's easier to tell if you fucked up something in your resolver in some scenarios */
	ResolverMode[iIndex] = "Unresolved";

	/* identifier: resolver variables */
	static int ResolveInt = 0;
	static int LastResolveMode[MAX_NETWORKID_LENGTH];
	static float LastShotTime[MAX_NETWORKID_LENGTH];
	float flLastResolveYaw[MAX_NETWORKID_LENGTH];

	flGoalFeetYawB[iIndex] = 0.f;
	flLbyB[iIndex] = pEntity->m_flLowerBodyYaw();

	bool ResetBF = false;

	static float flLowerBodyYawRec[MAX_NETWORKID_LENGTH];
	static float flFlickTime[MAX_NETWORKID_LENGTH];
	static float flLastMovingLby[MAX_NETWORKID_LENGTH];

	int ShotDelta = *Globals::iMissedShots[iIndex];

	bLbyUpdated = false;

	/* note: already done math so you can make a resolver easier :) */
	float angToLocal = Math::m_flNormalizeYaw(Math::CalcAngle(pLocalEnt->m_vecOrigin(), pEntity->m_vecOrigin()).y);
	float Back = Math::m_flNormalizeYaw(angToLocal);
	float Brute = pEntity->m_angEyeAngles().y;
	float AntiSide = 0.f;
	float EyeDelta = fabs(Math::m_flNormalizeYaw(vOldEyeAng[iIndex].y - pEntity->m_angEyeAngles().y));

	/* note: used for getting the players velocity in the air resolver */
	Record PushPlayer;

	if (pEntity->m_flLowerBodyYaw() != flLowerBodyYawRec[iIndex])
	{
		if (!LagComp::bWasDormant[iIndex])
		{
			flLowerBodyYawRec[iIndex] = pEntity->m_flLowerBodyYaw();
			bLbyUpdated = true;
			flFlickTime[iIndex] = pEntity->m_flSimulationTime() + Interfaces::GlobalVars->interval_per_tick;
			bCanUpdate[iIndex] = true;
		}
	}

	if (pEntity->m_flSimulationTime() - flFlickTime[iIndex] > 1.1f && bCanUpdate[iIndex])
	{
		flFlickTime[iIndex] = pEntity->m_flSimulationTime() + Interfaces::GlobalVars->interval_per_tick;
		bLbyUpdated = true;
	}

	/* 
	note:
		always state what resolver mode you are using first for your resolver flag
		or else it will not have time to render if your targeted entity switches resolver modes quickly 
		for example, your enemy switches from standing to jumping back to standing
		if it is not stated first it will always show standing
	*/

	/* 
	note: 
		not hitting premium = cfg issue lmao
		also what kind of fuck nigga rebuilds animlayers lmao
	*/
	if (bLbyUpdated)
	{
		ResolverMode[iIndex] = "LBY Update";
		pEntity->m_angEyeAngles().y = pEntity->m_flLowerBodyYaw();
	}
	/* note: kiduahook lby prediction + my gigantic brain */
	else if (pEntity->m_flSimulationTime() - flFlickTime[iIndex] > 1.1)
	{
		ResolverMode[iIndex] = "LBY Prediction";
		flFlickTime[iIndex] = pEntity->m_flSimulationTime();
		pEntity->m_angEyeAngles().y = pEntity->m_flLowerBodyYaw();
		bLbyUpdated = true;
	}
	else if (flVelocity > 35.f)
	{
		ResolverMode[iIndex] = "LowerBodyYaw";
		pEntity->m_angEyeAngles().y = pEntity->m_flLowerBodyYaw();
		/* note: delay body update */
		bLbyUpdated = flFlickTime[iIndex] + 0.22f;
		/* note: store last moving */
		flLastMovingLby[iIndex] = pEntity->m_flLowerBodyYaw();
	}
	/* note: this nigga really think he can fakewalk my ass lmao fuck u bitch */
	else if (flVelocity <= 35.f && flVelocity > 3.f)
	{
		ResolverMode[iIndex] = "FakeWalk:";
		switch (ShotDelta % 7)
		{
		case 0:
		{
			ResolverMode[iIndex] += " LastMoving";
			pEntity->m_angEyeAngles().y = flLastMovingLby[iIndex];
			break;
		}
		case 1:
		{
			ResolverMode[iIndex] += " LBY";
			pEntity->m_angEyeAngles().y = pEntity->m_flLowerBodyYaw();
			break;
		}
		case 2:
		{
			ResolverMode[iIndex] += " LBY Inv";
			pEntity->m_angEyeAngles().y = pEntity->m_flLowerBodyYaw() - 90.f;
			break;
		}
		case 3:
		{
			ResolverMode[iIndex] += " Back";
			pEntity->m_angEyeAngles().y = 179.9f;
			break;
		}
		case 4:
		{
			ResolverMode[iIndex] += " Inv";
			pEntity->m_angEyeAngles().y = angToLocal;
			break;
		}
		case 5:
		{
			ResolverMode[iIndex] += " Right";
			pEntity->m_angEyeAngles().y = -89.9f;
			break;
		}
		case 6:
		{
			ResolverMode[iIndex] += " Left";
			pEntity->m_angEyeAngles().y = 89.9f;
			break;
		}
		}
	}
	/* note: supremacy air resolver and my huge brain */
	else if (!(pEntity->m_fFlags() & FL_ONGROUND))
	{
		ResolverMode[iIndex] = "In Air";

		/* note: this variable tries to predict the direction of the player based on his velocity */
		float velyaw = Math::m_flRadToDeg(std::atan2(PushPlayer.vVelocity.y, PushPlayer.vVelocity.x));

		switch (ShotDelta % 3)
		{
		case 0:
			pEntity->m_angEyeAngles().y = velyaw + 180.f;
			break;
		case 1:
			pEntity->m_angEyeAngles().y = velyaw - 90.f;
			break;
		case 2:
			pEntity->m_angEyeAngles().y = velyaw + 90.f;
			break;
		}
	}
	/* note: freestanding resolver from the 2020 resolver that was in this source lmao */
	else if (pEntity->m_fFlags() & FL_ONGROUND && bUseFreestandAngle[iIndex])
	{
		ResolverMode[iIndex] = "Freestanding:";
		switch (ShotDelta % 2)
		{
		case 0:
			if (Math::m_flNormalizeYaw(pEntity->m_angEyeAngles().y - Back) > 0.f)
			{
				ResolverMode[iIndex] += " R";
				pEntity->m_angEyeAngles().y = -90.f;
			}
			else if (Math::m_flNormalizeYaw(pEntity->m_angEyeAngles().y - Back) < 0.f)
			{
				ResolverMode[iIndex] += " L";
				pEntity->m_angEyeAngles().y = 90.f;
			}
			break;
		case 1:
			if (Math::m_flNormalizeYaw(pEntity->m_angEyeAngles().y - Back) > 0.f)
			{
				ResolverMode[iIndex] += " L";
				pEntity->m_angEyeAngles().y = 90.f;
			}
			else if (Math::m_flNormalizeYaw(pEntity->m_angEyeAngles().y - Back) < 0.f)
			{
				ResolverMode[iIndex] += " R";
				pEntity->m_angEyeAngles().y = -90.f;
			}
			break;
		}
	}
	/* note: the best my brain can think of for another resolving method + my p bruteforce resolver */
	else
	{
		ResolverMode[iIndex] = "Brute:";
		switch (ShotDelta % 5)
		{
		case 0:
		{
			ResolverMode[iIndex] += " LastMoving";
			pEntity->m_angEyeAngles().y = flLastMovingLby[iIndex];
			break;
		}
		case 1:
		{
			ResolverMode[iIndex] += " Back";
			pEntity->m_angEyeAngles().y = 179.9f;
			break;
		}
		case 2:
		{
			ResolverMode[iIndex] += " Inverse";
			pEntity->m_angEyeAngles().y = angToLocal;
			break;
		}
		case 3: /* note: it shouldn't even get to here if it reaches the bruteforce condition you're fucking dead */
		{
			ResolverMode[iIndex] += " LBY";
			pEntity->m_angEyeAngles().y = pEntity->m_flLowerBodyYaw();
			break;
		}
		case 4:
		{
			ResolverMode[iIndex] += " LBY Inv";
			pEntity->m_angEyeAngles().y = pEntity->m_flLowerBodyYaw() - 90.f;
			break;
		}
		}
	}

	if (!bShot)
		vOldEyeAng[iIndex] = pEntity->m_angEyeAngles();
}