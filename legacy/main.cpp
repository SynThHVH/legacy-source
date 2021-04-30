#include "utilities/include.hpp"

/*
credits:
	as you can obviously tell, this is stickrpghook legacy
	i thought this was an excellent cheat to sort of base off of because its a really good source that just needs some improvements
	so credits to stickrpg
	next, credits to bolbi
	i took his antiaims from bameware and added them into this
	after that, resolver credits
	i took shit from supremacy, fatality, kiduahook, stackhack, and my own brain to make this premium ass resolver
	shit hits huh
	the esp i redid, still dogshit though
	i would't recommend using this shit, supremacy pastes and basically any other paste almost hits better than this because they have an animfix lmao
*/

DWORD WINAPI entry( LPVOID lpThreadParameter )
{
	while ( !FindWindowA( "Valve001", nullptr ) )
		std::this_thread::sleep_for( std::chrono::milliseconds( 300 ) );

	Globals::modules.dump_module_list();
	Interfaces::Initialize( );
	Hooks::Initialize( );

	while ( !GetAsyncKeyState( VK_END ) )
		std::this_thread::sleep_for( std::chrono::milliseconds( 1000 ) );

	Hooks::Release( );

	FreeLibraryAndExitThread( static_cast< HMODULE >( lpThreadParameter ), EXIT_SUCCESS );
}

BOOL APIENTRY DllMain( HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch ( ul_reason_for_call )
	{
	case DLL_PROCESS_ATTACH:
		DisableThreadLibraryCalls( hModule );
		CreateThread( 0, 0, ( LPTHREAD_START_ROUTINE ) entry, 0, 0, 0 );
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}