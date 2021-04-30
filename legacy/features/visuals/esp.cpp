#include "../features.hpp"

/*
note: 
	revamped esp, will recode and add more features to make it up to date with the new esp meta standards
	also for now, why should these bitch ass niggas be able to change the esp colors, these are fine until i recode our esp
	well that never happened either, lot of unfinished projects with this source
*/

RECT Visuals::BoundingBox = { NULL, NULL, NULL, NULL };
CBaseEntity* Visuals::pLocalEnt = nullptr;
int Visuals::YOffset = 0;
bool Visuals::bContinue = false;

void Visuals::Box()
{
	Draw::OutlinedRect(BoundingBox.left - 1, BoundingBox.top - 1, BoundingBox.right - BoundingBox.left + 2, BoundingBox.bottom - BoundingBox.top + 2, Color(0, 0, 0, 150));
	Draw::OutlinedRect(BoundingBox.left + 1, BoundingBox.top + 1, BoundingBox.right - BoundingBox.left - 2, BoundingBox.bottom - BoundingBox.top - 2, Color(0, 0, 0, 150));
	Draw::OutlinedRect(BoundingBox.left, BoundingBox.top, BoundingBox.right - BoundingBox.left, BoundingBox.bottom - BoundingBox.top, Color(255, 255, 255, 150));
}

void Visuals::Name(CBaseEntity* pEntity)
{
	int iIndex = pEntity->entindex();
	PlayerInfo_t pInfo;
	Interfaces::Engine->GetPlayerInfo(iIndex, &pInfo);

	if (!pInfo.m_cName)
		return;

	Draw::String(BoundingBox.left, BoundingBox.top - 16, Draw::Font.ESP, Color(255, 255, 255, 150), std::string(pInfo.m_cName));

	YOffset += 10;
}

void Visuals::HealthBar(CBaseEntity* pEntity)
{
	float Delta = float(BoundingBox.bottom - BoundingBox.top) / 100.f;
	int Position = (100 - pEntity->m_iHealth()) * Delta;

	if (pEntity->m_iHealth() >= 100)
		Draw::Rect(BoundingBox.left - 6, BoundingBox.top, 3, (BoundingBox.bottom - BoundingBox.top), Color(0, 255, 0, 150));
	else
	{
		Draw::Rect(BoundingBox.left - 6, BoundingBox.top, 3, Position, Color(255, 0, 0, 150));
		Draw::Rect(BoundingBox.left - 6, BoundingBox.top + Position, 3, (BoundingBox.bottom - BoundingBox.top) - Position, Color(0, 255, 0, 150));
	}

	Draw::OutlinedRect(BoundingBox.left - 7, BoundingBox.top - 1, 5, (BoundingBox.bottom - BoundingBox.top) + 2, Color(0, 0, 0, 150));
}

void Visuals::ResolverInfo(CBaseEntity* pEntity)
{
	int iIndex = pEntity->entindex();
	Draw::String(BoundingBox.right + 2, BoundingBox.top + YOffset, Draw::Font.ESP, Color(255, 255, 255, 150), Resolver::ResolverMode[iIndex]);

	YOffset += 10;

	float XOffset2 = 0.f;
	float YOffset2 = 0.f;
}

void Visuals::Weapon(CBaseEntity* pEntity)
{
	auto pWeapon = pEntity->GetActiveWeapon();

	if (!pWeapon)
		return;

	auto pWeaponData = pWeapon->m_pWeaponData();

	if (!pWeaponData)
		return;

	auto strWeaponName = std::string(pWeaponData->m_WeaponName);

	strWeaponName.erase(0, 7);

	Draw::String(BoundingBox.left, BoundingBox.top + 32, Draw::Font.ESP, Color(255, 255, 255, 150), strWeaponName);
	YOffset += 10;
}

void Visuals::BoundBox(CBaseEntity* pEntity)
{
	int iIndex = pEntity->entindex();

	if (!LagComp::pMatrix[iIndex])
		return;

	int LeftInt = 999999, TopInt = 999999, RightInt = -999999, BottomInt = -999999;
	static Vec3D w2s;

	int BestHitboxes[4];

	bContinue = false;

	for (int i = 0; i < 19; i++)
	{
		Vec3D vHitBoxPos = pEntity->GetHitboxPosition(i, LagComp::pMatrix[iIndex]);
		if (Interfaces::DebugOverlay->m_bWorldToScreen(vHitBoxPos, w2s))
		{
			if (w2s.x < LeftInt)
			{
				BestHitboxes[0] = i;
				LeftInt = w2s.x;
			}

			if (w2s.x > RightInt)
			{
				BestHitboxes[1] = i;
				RightInt = w2s.x;
			}

			if (w2s.y < TopInt)
			{
				BestHitboxes[2] = i;
				TopInt = w2s.y;
			}

			if (w2s.y > BottomInt)
			{
				BestHitboxes[3] = i;
				BottomInt = w2s.y;
			}
		}
		else
		{
			bContinue = true;
		}
	}

	Vec3D flAngToLocal = Globals::ViewSetup.angles;

	static Vec3D Bruh[4];

	Math::AngleVectors(Vec3D(0.f, Math::m_flNormalizeYaw(flAngToLocal.y + 90.f), 0.f), &Bruh[0]);
	Math::NormalizeAngles(Bruh[0]);
	Math::AngleVectors(Vec3D(0.f, Math::m_flNormalizeYaw(flAngToLocal.y - 90.f), 0.f), &Bruh[1]);
	Math::NormalizeAngles(Bruh[1]);
	Math::AngleVectors(Vec3D(flAngToLocal.x - 90.f, Math::m_flNormalizeYaw(flAngToLocal.y), 0.f), &Bruh[2]);
	Math::NormalizeAngles(Bruh[2]);
	Math::AngleVectors(Vec3D(flAngToLocal.x + 90.f, Math::m_flNormalizeYaw(flAngToLocal.y), 0.f), &Bruh[3]);
	Math::NormalizeAngles(Bruh[3]);

	static Vec2D OK[4];

	for (int i = 0; i < 4; i++)
	{
		Vec3D vHitBoxPos = pEntity->GetHitboxPosition(BestHitboxes[i], LagComp::pMatrix[iIndex]);

		vHitBoxPos += (Bruh[i] * 20.f);
		if (Interfaces::DebugOverlay->m_bWorldToScreen(vHitBoxPos, w2s))
		{
			OK[i] = Vec2D(w2s.x, w2s.y);
		}
	}

	BoundingBox.left = OK[0].x;
	BoundingBox.right = OK[1].x;
	BoundingBox.top = OK[2].y;
	BoundingBox.bottom = OK[3].y;
}

void Visuals::Esp( )
{
	if ( !Menu::Config.visuals )
		return;

	pLocalEnt = Interfaces::User( );

	if ( !pLocalEnt )
		return;

	for ( int index = 1; index < MAX_NETWORKID_LENGTH; ++index )
	{
		CBaseEntity* pPlayerEntity = Interfaces::EntityList->m_pGetClientEntity( index );

		if ( !pPlayerEntity
			|| !pPlayerEntity->m_bIsAlive( )
			
			|| pLocalEnt->entindex( ) == index
			|| pPlayerEntity->IsDormant( )
			|| pLocalEnt->GetTeam( ) == pPlayerEntity->GetTeam( ) )
		{
			continue;
		}

		YOffset = 0;

		BoundBox( pPlayerEntity );

		if ( bContinue )
			continue;

		if ( Menu::Config.box )
			Box( );

		if ( Menu::Config.name )
			Name( pPlayerEntity );

		if ( Menu::Config.weaponname )
			Weapon( pPlayerEntity );

		if ( Menu::Config.resolvermode )
			ResolverInfo( pPlayerEntity );

		if ( Menu::Config.healthbar )
			HealthBar( pPlayerEntity );
	}
}