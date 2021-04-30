#pragma once

struct CMoveData { byte data[ 184 ]; };

class IGameMovement
{
public:
	virtual			~IGameMovement( void ) {}

	virtual void	ProcessMovement( CBaseEntity *pPlayer, CMoveData *pMove ) = 0;
	virtual void	Reset( void ) = 0;
	virtual void	StartTrackPredictionErrors( CBaseEntity *pPlayer ) = 0;
	virtual void	FinishTrackPredictionErrors( CBaseEntity *pPlayer ) = 0;
	virtual void	DiffPrint( char const *fmt, ... ) = 0;

	virtual Vec3D const&	GetPlayerMins( bool ducked ) const = 0;
	virtual Vec3D const&	GetPlayerMaxs( bool ducked ) const = 0;
	virtual Vec3D const&   GetPlayer
	( bool ducked ) const = 0;

	virtual bool			IsMovingPlayerStuck( void ) const = 0;
	virtual CBaseEntity*	GetMovingPlayer( void ) const = 0;
	virtual void			UnblockPusher( CBaseEntity* pPlayer, CBaseEntity *pPusher ) = 0;

	virtual void    SetupMovementBounds( CMoveData *pMove ) = 0;
};