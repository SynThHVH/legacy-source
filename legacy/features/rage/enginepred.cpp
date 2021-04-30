#include "../features.hpp"

/*
note:
	revamped it so far, want to do a full recode at some point
	never did before release, i don't really care though
*/

/* identifier: global variables */
float Rage::flCurTime = 0.f;
float Rage::flFrameTime = 0.f;
int Rage::iTickCount = 0;

/* identifier: pattern variables */
int* m_pPredictionRandomSeed;
int* m_pSetPredictionPlayer;

/* note: get move data */
CMoveData m_MoveData;
static void* cMoveData = nullptr;

/* identifier: variable to see if our prediction has been ran */
bool EnginePredRan = false;

void Rage::PreEnginePred( CUserCmd* pCmd )
{
	/* note: we haven't ran our prediction yet */
	EnginePredRan = false;

	/* identifier: sanity checks */
	if ( Menu::Config.legitback ) return;
	if ( !Interfaces::MoveHelper ) return;

	/* note: get the local entity */
	auto pLocalEnt = Interfaces::User();

	/* identifier: local entity sanity checks */
	/* note: are we alive? */
	if (!pLocalEnt || !pLocalEnt->m_bIsAlive()) return;
	/* note: do we have a weapon? */
	if (!pLocalEnt->GetActiveWeapon()) return;

	/* note: store our accuracy penalty */
	pLocalEnt->GetActiveWeapon( )->m_UpdateAccuracyPenalty( );

	/* identifier: variable to see if our prediction has been initialized */
	static bool bInit = false;

	/* note: initialize our prediction */
	if ( !bInit ) 
	{
		/* note: define our patterns */
		m_pPredictionRandomSeed = *reinterpret_cast< int** >( Memory::Signature<uintptr_t>( GetModuleHandleA( "client.dll"), "A3 ? ? ? ? 66 0F 6E 86" ) + 1 );
		m_pSetPredictionPlayer = *reinterpret_cast< int** >( Memory::Signature<uintptr_t>( GetModuleHandleA( "client.dll"), "89 35 ? ? ? ? F3 0F 10 48 20" ) + 2 );

		/* note: run our move data */
		cMoveData = malloc( 184 );

		/* note: we are initialized! */
		bInit = true;
	}

	*m_pPredictionRandomSeed = MD5_PseudoRandom( pCmd->command_number ) & 0x7FFFFFFF;
	*m_pSetPredictionPlayer = reinterpret_cast< int >( pLocalEnt );
	
	/* note: backup our globals */
	flCurTime = Interfaces::GlobalVars->curtime;
	flFrameTime = Interfaces::GlobalVars->frametime;

	/* note: set globals appropriately */
	Interfaces::GlobalVars->curtime = float( pLocalEnt->m_iTickBase( ) ) * Interfaces::GlobalVars->interval_per_tick;
	Interfaces::GlobalVars->frametime = Interfaces::GlobalVars->interval_per_tick;

	/* note: start predicting */
	Interfaces::GameMovement->StartTrackPredictionErrors( pLocalEnt );

	/* note: set local entity */
	Interfaces::MoveHelper->SetHost( pLocalEnt );

	/* note: setup input */
	Interfaces::Prediction->SetupMove( pLocalEnt, pCmd, Interfaces::MoveHelper, reinterpret_cast< CMoveData* >( cMoveData ) );

	/* note: run movement */
	Interfaces::GameMovement->ProcessMovement( pLocalEnt, reinterpret_cast< CMoveData* >( cMoveData ) );

	/* note: input is setup, so now tell the game that it is */
	Interfaces::Prediction->FinishMove( pLocalEnt, pCmd, reinterpret_cast< CMoveData* >( cMoveData ) );

	/* note: state that our prediction has ran */
	EnginePredRan = true;
}

void Rage::PostEnginePred(CUserCmd* pCmd)
{
	/* identifier: sanity check */
	if (!EnginePredRan) return;

	/* note: get local entity */
	auto pLocalEnt = Interfaces::User();

	/* identifier: local sanity check */
	if (!pLocalEnt || !pLocalEnt->m_bIsAlive()) return;

	/* note: restore our prediction */
	Interfaces::GameMovement->FinishTrackPredictionErrors(pLocalEnt);

	/* note: restore set entity */
	Interfaces::MoveHelper->SetHost(nullptr);

	/* note: reset globals */
	Interfaces::GlobalVars->curtime = flCurTime;
	Interfaces::GlobalVars->frametime = flFrameTime;

	/* note: reset pattern variables */
	*m_pPredictionRandomSeed = -1;
	*m_pSetPredictionPlayer = 0;
}