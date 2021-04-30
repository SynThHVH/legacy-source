#include "../../utilities/include.hpp"
#include "../netvars/netvarhks.hpp"

C_HookManager* Hook;

Offset * gOffsets = nullptr;

template <typename T>
T C_HookManager::hook_vtable(void* pOriginal, void* pNew, int iIndex)
{
	if (pOriginal == nullptr || pNew == nullptr)
		return nullptr;

	DWORD pBackupProtection;
	auto pOriginalVirtual = *static_cast<DWORD*>(pOriginal) + 4 * iIndex;
	auto pBackupFunc = reinterpret_cast<void*>(*reinterpret_cast<DWORD*>(pOriginalVirtual));

	VirtualProtect(reinterpret_cast<void*>(pOriginalVirtual), 4, 0x40, &pBackupProtection);
	*reinterpret_cast<DWORD*>(pOriginalVirtual) = reinterpret_cast<DWORD>(pNew);
	VirtualProtect(reinterpret_cast<void*>(pOriginalVirtual), 4, pBackupProtection, &pBackupProtection);

	return static_cast<T>(pBackupFunc);
}

/* note: never used lol */
void Hooks::InitNetvarHooks()
{
	netvarHookManager.Hook("DT_CSPlayer", "m_angEyeAngles[0]", EyeAnglesPitchHook);
	netvarHookManager.Hook("DT_CSPlayer", "m_angEyeAngles[1]", EyeAnglesYawHook);
}

/*
 * start
 * Hooks all functions to what ever memory we are going to flash them to.
 */
void C_HookManager::start()
{
	Original::oFrameStageNotify = Hook->hook_vtable<Original::fnFrameStageNotify>(Interfaces::Client, Hooked::FrameStageNotify, 36);
	Original::oCreateMove = Hook->hook_vtable<Original::fnCreateMove>(Interfaces::Client, Hooked::CreateMove, 21);
	Original::oViewModelFov = Hook->hook_vtable<Original::fnViewModelFov>(Interfaces::ClientMode, Hooked::ViewModelFov, 35);
	Original::oOverrideView = Hook->hook_vtable<Original::fnOverrideView>(Interfaces::ClientMode, Hooked::OverrideView, 18);
	Original::oPaintTraverse = Hook->hook_vtable<Original::fnPaintTraverse>(Interfaces::Panel, Hooked::PaintTraverse, 41);
	Original::oLockCursor = Hook->hook_vtable<Original::fnLockCursor>(Interfaces::Surface, Hooked::LockCursor, 67);
	Original::oDrawModelExecute = Hook->hook_vtable<Original::fnDrawModelExecute>(Interfaces::ModelRender, Hooked::DrawModelExecute, 21);
	Original::oBeginFrame = Hook->hook_vtable<Original::fnBeginFrame>(Interfaces::StudioRender, Hooked::BeginFrame, 9);
}

void Hooks::Initialize( ) 
{
	g_pNetvars = std::make_unique<NetvarTree>( );

	gOffsets = new Offset;

	gOffsets->Init( ); //three types in one space lol

	Draw::Initialize( );

	Event.Initialize( );

	Hook->start( );

	Hooks::InitNetvarHooks( );
}

void Hooks::Release( ) 
{
	// VMTs
}