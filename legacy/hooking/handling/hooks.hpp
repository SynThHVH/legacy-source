#pragma once

class CBoneBitList;
class CIKContext;

// Temporary
class CStudioHdr;
//class Vec3D;
//class Vec4D;
//class matrix3x4_t;

class IRecipientFilter;

namespace Hooked {
	// VMTs
	void __stdcall FrameStageNotify(  int iStage );
	void __stdcall CreateMove( int sequence, float frametime, bool active);
	void __fastcall PaintTraverse( void* pPanels, int edx, unsigned int vguiPanel, bool forceRepaint, bool allowForce );
	void __fastcall LockCursor( ISurface* ecx, void* edx );
	void __fastcall DrawModelExecute( void* ecx, void* edx, IMatRenderContext* context, const DrawModelState_t& state, const ModelRenderInfo_t& renderInfo, matrix3x4_t* matrix );
	void __fastcall BeginFrame( void* ecx );
	void __fastcall OverrideView( void* ecx, void* edx, CViewSetup* pSetup );
	void __fastcall RunCommand( void* ecx, void* edx, CBaseEntity* pEntity, CUserCmd* pCmd, IMoveHelper* helper );
	float __fastcall ViewModelFov( void* ecx, void* edx );
	void __fastcall EmitSound( void* ecx, void* edx, void* filter, int iEntIndex, int iChannel, const char *pSoundEntry, unsigned int nSoundEntryHash, const char *pSample, float flVolume, float flAttenuation, int nSeed, int iFlags, int iPitch, const Vec3D *pOrigin, const Vec3D *pDirection, Vec3D * pUtlVecOrigins, bool bUpdatePositions, float soundtime, int speakerentity, StartSoundParams_t& params );
	bool __fastcall WriteUserCmdDeltaToBuffer( void* ecx, void* edx, int iSlot, bf_write* wBuffer, int iFrom, int iTo, bool bIsNewCommand );

	// Detours
	void __fastcall DoExtraBoneProcessing( void* ecx, void* edx, CStudioHdr* pHdr, Vec3D* vPosition, Vec4D* vUnknown, matrix3x4_t* pMatrix, CBoneBitList& pBoneList, CIKContext* pContext );
	void __fastcall UpdateClientSideAnimations( void *ecx, void *edx );

	// Detour addresses
//	static uintptr_t dwDoExtraBoneProcessing = Memory::Signature< uintptr_t >( GetModuleHandleA( "client.dll" ), "55 8B EC 83 E4 F8 81 EC ? ? ? ? 53 56 8B F1 57 89 74 24 1C" );
// 	static uintptr_t dwUpdateClientSideAnimations = Memory::Signature< uintptr_t >( GetModuleHandleA( "client.dll" ), "55 8B EC 51 56 8B F1 80 BE ? ? ? ? ? 74 36" );
}

namespace Original {
	// VMTs
	typedef void(__stdcall* fnFrameStageNotify)(int iStage);
	inline fnFrameStageNotify oFrameStageNotify;

	typedef void(__stdcall* fnCreateMove)(int sequence, float frametime, bool active);
	inline fnCreateMove oCreateMove;

	typedef void(__fastcall* fnPaintTraverse)(void* pPanels, int edx, unsigned int vguiPanel, bool forceRepaint, bool allowForce);
	inline fnPaintTraverse oPaintTraverse;

	typedef void(__fastcall* fnLockCursor)(ISurface* ecx, void* edx);
	inline fnLockCursor oLockCursor;

	typedef void(__fastcall* fnDrawModelExecute)(void* ecx, void* edx, IMatRenderContext* context, const DrawModelState_t& state, const ModelRenderInfo_t& renderInfo, matrix3x4_t* matrix);
	inline fnDrawModelExecute oDrawModelExecute;

	typedef void(__fastcall* fnBeginFrame)(void* ecx);
	inline fnBeginFrame oBeginFrame;

	typedef void(__fastcall* fnOverrideView)(void* ecx, void* edx, CViewSetup* pSetup);
	inline fnOverrideView oOverrideView;

	typedef void(__fastcall* fnViewModelFov)(void* ecx, void* edx);
	inline fnViewModelFov oViewModelFov;
}

namespace Hooks {
	void Initialize( );
	void Release( );
	void InitNetvarHooks( );
};