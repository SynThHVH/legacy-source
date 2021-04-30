#pragma once

struct BulletHit
{
public:
	Vec3D ShootPos;
	Vec3D Hit;
	float timeShot;
	bool HitPlayer = false;
};

class C_Address
{
	uintptr_t uAddress;
public:
	template <typename T>
	explicit C_Address(T ObjectLocation)
	{
		uAddress = reinterpret_cast<uintptr_t>(ObjectLocation);
	}

	uintptr_t get_address() const;
	bool is_valid_address() const;
	uint8_t at(int iOffset) const;
};

class C_Module
{
	std::map<std::string, HINSTANCE__*> modules;

	static bool is_valid_export(C_Address Address);
public:
	void dump_module_list(DWORD ProccessID = GetCurrentProcessId());
	std::map<std::string, HINSTANCE__*> get_loaded_modules() const;
	HINSTANCE__* get_loaded_module_handle(std::string Module);
	uintptr_t get_export(std::string Module, std::string Export);
	static uintptr_t get_export(HINSTANCE__* Module, std::string Export);
};

namespace Globals
{
	extern bool UpdateAnimations;
	extern Vec3D MaxsMins[ 20 ][ 2 ];
	extern std::deque<Vec3D> AimPoints;
	extern std::deque<Vec3D> FreeStandPoints;
	extern bool bSendPacket;
	extern Vec3D AimPunchAng;
	extern CViewSetup ViewSetup;
	extern std::deque<BulletHit> Bullet;
	extern matrix3x4_t mLocalMatrix[ 128 ];
	extern int iTickBaseShift;
	extern int iMissedShots[ 65 ][3];
	extern int iMissedShotRslv[ 65 ];
	extern bool bShot;
	extern bool bHit;
	extern bool bIsFakeWalking;
	extern Vec3D vRealAngles;

	extern C_Module modules;

	typedef void( __cdecl* MsgFn )( const char* msg, va_list );
	static void Msg( const char* msg, ... )
	{
		/* note: do nothing if no string was passed or if it returned null */
		if ( msg == nullptr )
			return;

		/* note: this gets the address of the message export in 'tier0.dll', the static keyword means it is only called once but the variable remains there. */
		static MsgFn fn = ( MsgFn ) GetProcAddress( GetModuleHandle( "tier0.dll" ), "Msg" );
		char buffer[ 989 ];

		/* note: normal varargs shit - http://stackoverflow.com/questions/10482960/varargs-to-printf-all-arguments */
		va_list list;
		va_start( list, msg );
		vsprintf( buffer, msg, list );
		va_end( list );
		
		/* note: call the function we already got the address */
		fn( buffer, list );
	}
}