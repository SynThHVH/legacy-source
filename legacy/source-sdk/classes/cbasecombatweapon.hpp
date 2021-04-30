#pragma once

/*class N003D1189
{
public:
char _0x0000[236];
__int32 m_iDamage; //0x00EC
float m_flArmorRatio; //0x00F0
char _0x00F4[4];
float m_flPenetration; //0x00F8
char _0x00FC[8];
float m_flRange; //0x0104
float m_flRangeModifier; //0x0108
char _0x010C[52];

};//Size=0x0140*/

/*char pad[ 0x4 ]; 
	char* m_WeaponName; 
	char pad1[ 0xC ]; 
	__int32 m_iMaxClip1; 
	char pad2[ 0xC ]; 
	__int32 m_iMaxReservedAmmo;
	char pad3[ 0x4 ];
	char* m_WeaponMdlPath; 
	char pad4[ 0x4 ];
	char* m_DropWeaponMdlPath;
	char pad5[ 0x48 ];
	char* m_BulletType;
	char pad6[ 0x4 ];
	char* m_HudName; 
	char pad7[ 0x40 ];
	__int32 m_WeaponType;
	__int32 m_WeaponPrice;
	__int32 m_WeaponReward;
	char* m_WeaponGroupName; 
	char pad8[ 0x10 ];
	unsigned char m_bFullAuto;
	char pad9[ 0x3 ];
	__int32 m_iDamage;
	float m_flArmorRatio;
	__int32 m_iBullets;
	float m_flPenetration;
	char pad10[ 0x8 ];
	float m_flRange; 
	float m_flRangeModifier; 
	char pad11[ 0x10 ]; 
	unsigned char m_Silencer;
	char pad12[ 0xF ];
	float m_flMaxSpeed; 
	float m_flMaxSpeedAlt; 
	char pad13[ 0x4C ];
	__int32 m_iRecoilSeed;
	char pad14[ 0x68 ];
	char* m_WeaponTracesType;
	char pad15[ 0x638 ]; 
*/
class WeaponInfo_t
{
public:	
	char pad[0x4];
	char* m_WeaponName;
	char _0x0000[228];
	__int32 m_iDamage; //0x00EC
	float m_flArmorRatio; //0x00F0
	char _0x00F4[4];
	float m_flPenetration; //0x00F8
	char _0x00FC[8];
	float m_flRange; //0x0104
	float m_flRangeModifier; //0x0108
	char _0x010C[52];
};

class CBaseCombatWeapon : public CBaseEntity
{
public:
	template <typename T>
	T& Get( uintptr_t offset )
	{
		return *reinterpret_cast< T* >( reinterpret_cast< uintptr_t >( this ) + offset );
	}

	// Offsets

	// Virtual functions
	VF( m_flInaccuracy, float, 469 );
	VF( m_flSpread, float, 439 );
	VF( m_UpdateAccuracyPenalty, void, 470 );
	VF( m_pWeaponData, WeaponInfo_t*, 446 );

	// Netvars
	OF( m_fAccuracyPenalty, float, gOffsets->m_fAccuracyPenalty);
	OF( m_fLastShotTime, float, gOffsets->m_fLastShotTime );
	OF( m_flNextPrimaryAttack, float, gOffsets->m_flNextPrimaryAttack );
	OF( m_iClip1, int, 0x3204);

	bool IsKnifeorNade( )
	{
		if ( !this )
			return false;

		auto pWeapon = this->m_pWeaponData( );

		if ( !pWeapon )
			return false;

		std::string WeaponName = pWeapon->m_WeaponName;

		if ( strstr( WeaponName.c_str( ), "weapon_knife" ) ) // EZ
			return true;
		else if ( WeaponName == "weapon_incgrenade" )
			return true;
		else if ( WeaponName == "weapon_decoy" )
			return true;
		else if ( WeaponName == "weapon_flashbang" )
			return true;
		else if ( WeaponName == "weapon_hegrenade" )
			return true;
		else if ( WeaponName == "weapon_smokegrenade" )
			return true;
		else if ( WeaponName == "weapon_molotov" )
			return true;

		return false;
	}

	float FireRate( )
	{
		if ( !this )
			return 0.f;

		if ( this->IsKnifeorNade( ) )
			return 0.f;

		auto pWeapon = this->m_pWeaponData( );

		if ( !pWeapon )
			return false;

		std::string WeaponName = pWeapon->m_WeaponName;

		if ( WeaponName == "weapon_glock" )
			return 0.15f;
		else if ( WeaponName == "weapon_hkp2000" )
			return 0.169f;
		else if ( WeaponName == "weapon_usp_silencer" )
			return 0.169f;
		else if ( WeaponName == "weapon_p250" )
			return 0.15f;
		else if ( WeaponName == "weapon_tec9" )
			return 0.12f;
		else if ( WeaponName == "weapon_elite" )
			return 0.12f;
		else if ( WeaponName == "weapon_fiveseven" )
			return 0.15f;
		else if ( WeaponName == "weapon_deagle" )
			return 0.224f;
		else if ( WeaponName == "weapon_nova" )
			return 0.882f;
		else if ( WeaponName == "weapon_sawedoff" )
			return 0.845f;
		else if ( WeaponName == "weapon_mag7" )
			return 0.845f;
		else if ( WeaponName == "weapon_xm1014" )
			return 0.35f;
		else if ( WeaponName == "weapon_mac10" )
			return 0.075f;
		else if ( WeaponName == "weapon_ump45" )
			return 0.089f;
		else if ( WeaponName == "weapon_mp9" )
			return 0.070f;
		else if ( WeaponName == "weapon_bizon" )
			return 0.08f;
		else if ( WeaponName == "weapon_mp7" )
			return 0.08f;
		else if ( WeaponName == "weapon_p90" )
			return 0.070f;
		else if ( WeaponName == "weapon_galilar" )
			return 0.089f;
		else if ( WeaponName == "weapon_ak47" )
			return 0.1f;
		else if ( WeaponName == "weapon_sg556" )
			return 0.089f;
		else if ( WeaponName == "weapon_m4a1_silencer" )
			return 0.089f;
		else if ( WeaponName == "weapon_m4a1" )
			return 0.089f;
		else if ( WeaponName == "weapon_aug" )
			return 0.089f;
		else if ( WeaponName == "weapon_m249" )
			return 0.08f;
		else if ( WeaponName == "weapon_negev" )
			return 0.0008f;
		else if ( WeaponName == "weapon_ssg08" )
			return 1.25f;
		else if ( WeaponName == "weapon_awp" )
			return 1.463f;
		else if ( WeaponName == "weapon_g3sg1" )
			return 0.25f;
		else if ( WeaponName == "weapon_scar20" )
			return 0.25f;
		else if ( WeaponName == "weapon_mp5sd" )
			return 0.08f;
		else
			return .0f;

	}
};