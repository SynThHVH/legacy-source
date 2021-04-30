#include "../../features/features.hpp"

float __fastcall Hooked::ViewModelFov( void* ecx, void* edx )
{
	if ( Menu::Config.viewmodelfov != 0.0 )
		return Menu::Config.viewmodelfov;
	else
		return 60.f;
}

/* note: you shouldn't need the runcommand fix for 2018 if i remember correctly but then again i wasn't coding back then only hvhing */
void __fastcall Hooked::RunCommand( void* ecx, void* edx, CBaseEntity* pEntity, CUserCmd* pCmd, IMoveHelper* helper )
{
	//Original::oPrediction->Get< decltype( &Hooked::RunCommand ) >( 19 )( ecx, edx, pEntity, pCmd, helper );
}

void __stdcall Hooked::FrameStageNotify( int iStage )
{
	LagComp::FrameStage( iStage );

	Original::oFrameStageNotify( iStage);
}

void __stdcall CMCall(  int sequence, float frametime, bool active, bool& bSendPacket)
{
	Original::oCreateMove( sequence, frametime, active);

	CVerifiedUserCmd* pVerif = &(*(CVerifiedUserCmd**)((DWORD)Interfaces::Input + 0xF0))[sequence % 150];
	CUserCmd* pCmd = &(*(CUserCmd**)((DWORD)Interfaces::Input + 0xEC))[sequence % 150];

	if ( !pCmd || !pCmd->command_number )
		return;

	if ( !GetKeyState( VK_INSERT ) )
		pCmd->buttons &= ~IN_ATTACK;

	static ConVar *sv_cheats = Interfaces::Cvar->FindVar( "sv_cheats" );

	if ( sv_cheats->ReadInt( ) == 0 )
		sv_cheats->SetInt( 1 );

	Globals::bSendPacket = true;

	Vec3D vOldang = pCmd->viewangles;

	auto pLocalEnt = Interfaces::User( );

	if (!pLocalEnt )
		return;

	if (!pLocalEnt->m_bIsAlive())
		return;

	Globals::AimPunchAng = Rage::DecayAimPunchAngleReversed( pLocalEnt );

	static int nSinceUse = 0;

	Rage::FakeLag( );

	Movement::Bunnyhop( pCmd );
	Movement::AutoStrafe( pCmd );

	/* note: need to add an actual fakewalk soon lmao */
	if ( GetAsyncKeyState( Menu::Config.slwwlk ) )
		Rage::Stop( pCmd );

	Rage::PreEnginePred( pCmd );
	{
		Resolver::ResolverPoints();
		antiaim.Do(pCmd);
		Legit::Backtrack(pCmd);
		Rage::Aimbot(pCmd);
	}
	Rage::PostEnginePred(pCmd);

	pCmd->viewangles.Clamp( );

	if ( !Menu::Config.legitback )
	{
		bSendPacket = Globals::bSendPacket;

		Rage::CorrectMovement( vOldang, pCmd );
	}

	pVerif->pCmd = *pCmd;
	pVerif->Crc = pCmd->GetChecksum();
}

__declspec(naked) void __stdcall Hooked::CreateMove(int sequence, float frametime, bool active)
{
	__asm
	{
		PUSH	EBP
		MOV		EBP, ESP
		PUSH	EBX
		LEA		ECX, [ESP]
		PUSH	ECX
		PUSH	active
		PUSH	frametime
		PUSH	sequence
		CALL	CMCall
		POP		EBX
		POP		EBP
		RETN	0xC
	}
}

void __fastcall Hooked::PaintTraverse( void* pPanels, int edx, unsigned int vguiPanel, bool forceRepaint, bool allowForce )
{
	static unsigned int vguiPanelID = NULL;
	Original::oPaintTraverse( pPanels, edx, vguiPanel, forceRepaint, allowForce );

	if ( !vguiPanelID && !strcmp( "MatSystemTopPanel", Interfaces::Panel->GetName( vguiPanel ) ) ) vguiPanelID = vguiPanel;
	else if ( vguiPanel != vguiPanelID )
		return;

	Visuals::Esp( );

	Menu::Render( );

	Menu::PopUpHandle( );
}

void __fastcall Hooked::DrawModelExecute( void* ecx, void* edx, IMatRenderContext* context, const DrawModelState_t& state, const ModelRenderInfo_t& renderInfo, matrix3x4_t* matrix )
{
	if ( !Interfaces::Engine->IsInGame( ) || !Interfaces::Engine->IsConnected( ) )
		return Original::oDrawModelExecute( ecx, edx, context, state, renderInfo, matrix );

	const char* ModelName = Interfaces::ModelInfo->GetModelName( ( model_t* ) renderInfo.pModel );
	CBaseEntity* pPlayerEntity = Interfaces::EntityList->m_pGetClientEntity( renderInfo.index );

	if ( strstr( ModelName, "models/player/contactshadow" ) )
		return;

	auto pLocalEnt = Interfaces::User( );

	if ( !pLocalEnt )
		return Original::oDrawModelExecute(ecx, edx, context, state, renderInfo, matrix);

	if ( !pPlayerEntity
		|| !pPlayerEntity->m_bIsAlive( )	
		|| pPlayerEntity->IsDormant( )
		|| !strstr( ModelName, "models/player" ) )
		return Original::oDrawModelExecute(ecx, edx, context, state, renderInfo, matrix);

	static matrix3x4_t mBackup[ 128 ];
	matrix3x4_t* pMatrix = LagComp::pMatrix[ renderInfo.index ];

	/* note: ma nigga i got no clue what this does lmao */
	if ( pPlayerEntity->entindex( ) == pLocalEnt->entindex( ) )
	{
		memcpy( mBackup, pMatrix, ( sizeof( matrix3x4_t ) * 128 ) );
		for ( int i = 0; i < 128; i++ )
			mBackup[ i ].SetOrigin( ( mBackup[ i ].GetOrigin( ) - LagComp::pOrigin[ renderInfo.index ] ) + pPlayerEntity->GetAbsOrigin( ) );

		return Original::oDrawModelExecute( ecx, edx, context, state, renderInfo, mBackup );
	}
	/* note: start of chams */
	else if (Menu::Config.chams && pPlayerEntity->GetTeam() != pLocalEnt->GetTeam())
	{
		/* note: removed before release */
	}

	return Original::oDrawModelExecute( ecx, edx, context, state, renderInfo, pMatrix );
}

void __fastcall Hooked::LockCursor( ISurface* ecx, void* edx )
{
	if ( GetKeyState( VK_INSERT ) )
	{
		return Original::oLockCursor( ecx, edx );
	}

	Interfaces::Surface->UnLockCursor( );
}

CBaseEntity* UTIL_PlayerByIndex( int index )
{
	typedef CBaseEntity*( __fastcall* PlayerByIndex )( int );
	static PlayerByIndex UTIL_PlayerByIndex = reinterpret_cast< PlayerByIndex >( Memory::Signature<uintptr_t>( GetModuleHandleA( "server.dll" ), "85 C9 7E 2A A1" ) );

	if ( !UTIL_PlayerByIndex )
		return nullptr;

	return UTIL_PlayerByIndex( index );
}

void __fastcall Hooked::BeginFrame(void* ecx)
{
	Original::oBeginFrame(ecx);

	if (!Interfaces::Engine->IsInGame() || !Interfaces::Engine->IsConnected())
		return;

	if (Menu::Config.bullettrails && Globals::Bullet.size() > 0)
	{
		for (int i = 0; i < Globals::Bullet.size(); i++)
		{
			BulletHit Bullet = Globals::Bullet.at(i);

			if (Bullet.HitPlayer)
			{
				Interfaces::DebugOverlay->AddLineOverlayAlpha(Bullet.ShootPos, Bullet.Hit, 0, 255, 0, 255, true, 1.f);
			}
			else
			{
				Interfaces::DebugOverlay->AddLineOverlayAlpha(Bullet.ShootPos, Bullet.Hit, 255, 0, 0, 255, true, 1.f);
			}

			Globals::Bullet.erase(Globals::Bullet.begin() + i);
		}
	}

	auto pLocalEnt = Interfaces::User();

	if (!pLocalEnt)
		return;

	for (int index = 1; index < MAX_NETWORKID_LENGTH; ++index)
	{
		CBaseEntity* pPlayerEntity = Interfaces::EntityList->m_pGetClientEntity(index);

		if (!pPlayerEntity
			|| !pPlayerEntity->m_bIsAlive()
			|| pPlayerEntity->IsDormant()
			|| (pLocalEnt->GetTeam() == pPlayerEntity->GetTeam() && index != pLocalEnt->entindex()))
		{
			continue;
		}

		if (Menu::Config.shotcham && Rage::iTargetMatrix.second == index)
		{
			pPlayerEntity->DrawHitboxes(Rage::iTargetMatrix.first, Color(Menu::Config.shotchamscolor.red, Menu::Config.shotchamscolor.green, Menu::Config.shotchamscolor.blue, 100), Color(Menu::Config.shotchamscolor.red, Menu::Config.shotchamscolor.green, Menu::Config.shotchamscolor.blue, 255), .5f);
			Rage::iTargetMatrix.second = -1;
		}
	}
}

bool __fastcall Hooked::WriteUserCmdDeltaToBuffer( void* ecx, void* edx, int iSlot, bf_write* wBuffer, int iFrom, int iTo, bool bIsNewCommand )
{
	return false;
	//	return Original::Client->Get< decltype( &Hooked::WriteUserCmdDeltaToBuffer ) >( 24 )( ecx, edx, iSlot, wBuffer, iFrom, iTo, bIsNewCommand );
}

void __fastcall Hooked::OverrideView( void* ecx, void* edx, CViewSetup* pSetup )
{
	Original::oOverrideView( ecx, edx, pSetup );

	if ( !Interfaces::Engine->IsInGame( ) || !Interfaces::Engine->IsConnected( ) )
		return;

	auto pLocalEnt = Interfaces::User( );
	static bool died = false;
	if ( pLocalEnt && pLocalEnt->m_bIsAlive( ) )
	{
		died = false;

		if ( GetKeyState( Menu::Config.tpkey ) )
			Interfaces::Input->m_bCameraInThirdperson = true;
		else
			Interfaces::Input->m_bCameraInThirdperson = false;

		if ( Menu::Config.novisrecoil && !GetKeyState( Menu::Config.tpkey ) )
		{
			Vec3D viewPunch = pLocalEnt->m_viewPunchAngle( );
			Vec3D aimPunch = pLocalEnt->m_aimPunchAngle( );

			float RecoilScale = Interfaces::Cvar->FindVar( "weapon_recoil_scale" )->ReadFloat( );

			pSetup->angles -= ( viewPunch + ( aimPunch * RecoilScale * 0.4499999f ) );
		}

		if ( Menu::Config.fov != 0.0 )
		{
			pSetup->fov = Menu::Config.fov;
		}

		if ( GetKeyState( Menu::Config.tpkey ) )
		{
			Vec3D ViewAngles, ViewForward;
			Interfaces::Engine->GetViewAngles( ViewAngles );

			pSetup->angles = ViewAngles;

			ViewAngles.x = -ViewAngles.x;
			ViewAngles.y = Math::m_flNormalizeYaw( ViewAngles.y + 180 );

			Math::AngleVectors( ViewAngles, &ViewForward );
			Math::NormalizeAngles( ViewForward );

			Vec3D Origin = pLocalEnt->GetAbsOrigin( ) + pLocalEnt->GetViewOffset( );

			trace_t TraceInit;
			CTraceFilter filter1( pLocalEnt, TRACE_WORLD_ONLY );
			Interfaces::Trace->TraceRay( Ray_t( Origin, ( Origin + ( ViewForward * Menu::Config.tpdistance ) ) ), MASK_SOLID, &filter1, &TraceInit );

			ViewForward = Origin + ( ViewForward * ( TraceInit.m_flFraction *  (Menu::Config.tpdistance - 15) ) );

			pSetup->origin = ViewForward;
		}
	}
	else if ( !died )
	{
		died = true;
		Interfaces::Input->m_bCameraInThirdperson = false;
	}

	Globals::ViewSetup = *pSetup;
}

void __fastcall Hooked::EmitSound( void* ecx, void* edx, void* filter, int iEntIndex, int iChannel, const char *pSoundEntry, unsigned int nSoundEntryHash, const char *pSample, float flVolume, float flAttenuation, int nSeed, int iFlags, int iPitch, const Vec3D *pOrigin, const Vec3D *pDirection, Vec3D * pUtlVecOrigins, bool bUpdatePositions, float soundtime, int speakerentity, StartSoundParams_t& params )
{
	if (strstr( pSample, "weapon" ) )
	{
		if ( iEntIndex == Interfaces::Engine->GetLocalPlayer( ) )
		{
			if ( Menu::Config.ragebot && !Menu::Config.legitback )
				return;
		}
	}

	//Original::EngineSound->Get< decltype( &Hooked::EmitSound ) >( 5 )( ecx, edx, filter, iEntIndex, iChannel, pSoundEntry, nSoundEntryHash, pSample, flVolume, flAttenuation, nSeed, iFlags, iPitch, pOrigin, pDirection, pUtlVecOrigins, bUpdatePositions, soundtime, speakerentity, params );
}