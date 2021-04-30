#pragma once

class CEvent : public IGameEventListener2
{
public:
	void FireGameEvent( IGameEvent *event );
	int  GetEventDebugID = 42;
	void Initialize( )
	{
		Interfaces::GameEventManager->AddListener( this, "player_hurt", false );
		Interfaces::GameEventManager->AddListener( this, "weapon_fire", false );
		Interfaces::GameEventManager->AddListener( this, "bullet_impact", false );
	}
};

extern CEvent Event;