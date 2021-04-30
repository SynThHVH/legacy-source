#include "../../utilities/include.hpp"

// standard interface system
C_InterfaceScanner Scn;
namespace Interfaces
{
	// Interfaces to be grabbed
	IClient* Client;
	CGlobalVarsBase* GlobalVars;
	IVEngineClient* Engine;
	IClientEntityList* EntityList;
	IClientMode* ClientMode;
	IPanel* Panel;
	ISurface* Surface;
	ICvar* Cvar;
	IEngineTrace* Trace;
	CDebugOverlay* DebugOverlay;
	IGameTypes* GameTypes;
	IMaterialSystem* MaterialSystem;
	IVModelInfo* ModelInfo;
	IVModelRender* ModelRender;
	IPhysicsSurfaceProps* SurfaceProps;
	IGameMovement* GameMovement;
	IMoveHelper* MoveHelper;
	CPrediction* Prediction;
	IStudioRender* StudioRender;
	IGameEventManager2* GameEventManager;
	CInput* Input;
	IEngineSound* EngineSound;
	CClientState* ClientState;

	// Predefined interfaces
	CBaseEntity* User( )
	{
		int iIndex = Interfaces::Engine->GetLocalPlayer( );

		CBaseEntity* Player = Interfaces::EntityList->m_pGetClientEntity( iIndex );

		if ( !Player )
			return nullptr;

		return Player;
	}
}

/*
 * get_interface
 * Returns the pointer to the specifc interface given
 */
template <typename T>
T C_InterfaceScanner::get_interface(const char* chModule, const char* chInterface) const
{
	static_assert(std::is_pointer<T>::value, "Type T must be a pointer type!");

	char chBuffer[0x1000];
	HINSTANCE__* pModule;

	do
		pModule = GetModuleHandleA(chModule);
	while (pModule == nullptr);

	auto fnCreateInterface = reinterpret_cast<void* (*)(const char* p1, int* p2)>(GetProcAddress(pModule, "CreateInterface"));

	for (auto i = 100; i > 0; i--)
	{
		void* pInterface;
		sprintf_s(chBuffer, "%s%03i", chInterface, i);

		if ((pInterface = fnCreateInterface(chBuffer, nullptr)) != nullptr)
		{
			return static_cast<T>(pInterface);
		}
	}

	return nullptr;
}

/*
* find_interface
* Returns the pointer to the specifc interface given using the linked list
*/
template <typename T>
T C_InterfaceScanner::find_interface(std::string chInterface)
{
	static_assert(std::is_pointer<T>::value, "Type T must be a pointer type!");
	void* Object = nullptr;

	auto version_highest = -1;

	for (auto entry : m_InterfaceList)
	{
		/*
		> Copys the cleaned interface name into a buffer
		> ex. VClient001 -> VClient
		*/
		auto tmp(entry.first);
		tmp.resize(tmp.size() - 3);

		if (chInterface != tmp)
			continue;

		/*
		> Extracts the interface version number
		*/
		auto interface_version_buffer(entry.first.substr(entry.first.length() - 3));
		auto interface_version = atoi(interface_version_buffer.c_str());

		if (interface_version > version_highest)
		{
			Object = entry.second;
			version_highest = interface_version;
		}
	}

//	G::console.print("%s", chInterface.c_str());

	if (!Object)
	{
	//	G::console.print(" = ");
	//	G::console.print(eColour::RED, "nullptr\n");
	}
	else
	{
	//	G::console.print("%i = ", version_highest);
	//	G::console.print(eColour::GREEN, "0x%X\n", Object);
	}

	return static_cast<T>(Object);
}

/*
* push_back_interfaces
* Adds every interface into a map
*/
void C_InterfaceScanner::push_back_interfaces()
{
	for (auto module : Globals::modules.get_loaded_modules())
	{
		auto fnCreateInterface = Globals::modules.get_export(module.second, "CreateInterface");

		if (!fnCreateInterface)
			continue;

		m_iModules++;

		auto fnCreateInterfaceLocation = fnCreateInterface + 0x4;

		auto fnCreateInterfaceInternal = fnCreateInterfaceLocation + *reinterpret_cast<size_t*>(fnCreateInterfaceLocation + 0x1) + 0x5;
		interface_first = **reinterpret_cast<InterfaceReg***>(fnCreateInterfaceInternal + 0x6);

		if (!interface_first)
			continue;

		while (interface_first)
		{
			if (!interface_first)
				return;

			m_InterfaceList.insert_or_assign(interface_first->m_pName, interface_first->m_CreateFn());

			interface_first = interface_first->m_pNext;
		}
	}

	m_iInterfaces = m_InterfaceList.size();
}


void Interfaces::Initialize( )
{
	Scn.push_back_interfaces();

	Client = Scn.find_interface<IClient* >( "VClient"  );
	ClientMode = **reinterpret_cast< IClientMode*** >( ( *reinterpret_cast< uintptr_t** >( Client ) )[ 10 ] + 5 );
	GlobalVars = **reinterpret_cast< CGlobalVarsBase*** >( ( *reinterpret_cast< uintptr_t** >( Client ) )[ 0 ] + 27 );
	Engine = Scn.find_interface< IVEngineClient* >( "VEngineClient" );
	EntityList = Scn.find_interface< IClientEntityList* >( "VClientEntityList" );
	Panel = Scn.find_interface< IPanel* >( "VGUI_Panel" );
	Surface = Scn.get_interface< ISurface* >( "vguimatsurface.dll", "VGUI_Surface" );
	Cvar = Scn.find_interface< ICvar* >( "VEngineCvar" );
	Trace = Scn.find_interface< IEngineTrace* >( "EngineTraceClient" );
	DebugOverlay = Scn.find_interface< CDebugOverlay* >( "VDebugOverlay" );
//	GameTypes = reinterpret_cast< IGameTypes* >( GrabInterface( "matchmaking.dll", "VENGINE_GAMETYPES_VERSION002", true ) );
	MaterialSystem = Scn.get_interface< IMaterialSystem* >(  "materialsystem.dll", "VMaterialSystem" );
	ModelInfo = Scn.find_interface< IVModelInfo* >( "VModelInfoClient"  );
	ModelRender = Scn.find_interface< IVModelRender* >( "VEngineModel" );
	SurfaceProps = Scn.get_interface< IPhysicsSurfaceProps* >("vphysics.dll", "VPhysicsSurfaceProps");
	GameMovement = Scn.get_interface< IGameMovement* >( "client.dll", "GameMovement" );
	MoveHelper = **reinterpret_cast< IMoveHelper*** >( ( Memory::Signature<uintptr_t>( GetModuleHandleA( "client.dll"), "8B 0D ? ? ? ? 8B 46 08 68" )+ 2) );
	Prediction = Scn.get_interface< CPrediction* >( "client.dll", "VClientPrediction" );
	StudioRender = Scn.get_interface< IStudioRender* >( "studiorender.dll", "VStudioRender" );
	GameEventManager = Scn.find_interface< IGameEventManager2* >( "GAMEEVENTSMANAGER" );
	Input = *reinterpret_cast< CInput** >((*reinterpret_cast<uintptr_t**>(Client))[15] + 1);
	EngineSound = Scn.get_interface< IEngineSound* >(  "engine.dll", "IEngineSoundClient"  );
//	ClientState = **reinterpret_cast< CClientState*** >((Memory::Signature<uintptr_t>(GetModuleHandleA("client.dll"), "A1 ? ? ? ? 8B 80 ? ? ? ? C3") + 1) );   0x57C844
}