#pragma once

//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//
//===========================================================================//

struct kbutton_t;

enum
{
	GAME_AXIS_NONE = 0,
	GAME_AXIS_FORWARD,
	GAME_AXIS_PITCH,
	GAME_AXIS_SIDE,
	GAME_AXIS_YAW,
	MAX_GAME_AXES
};

enum
{
	CAM_COMMAND_NONE = 0,
	CAM_COMMAND_TOTHIRDPERSON = 1,
	CAM_COMMAND_TOFIRSTPERSON = 2
};

enum
{
	MOUSE_ACCEL_THRESHHOLD1 = 0,	// if mouse moves > this many mickey's double it
	MOUSE_ACCEL_THRESHHOLD2,		// if mouse moves > this many mickey's double it a second time
	MOUSE_SPEED_FACTOR,				// 0 = disabled, 1 = threshold 1 enabled, 2 = threshold 2 enabled

	NUM_MOUSE_PARAMS,
};

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
class CKeyboardKey
{
public:
	// Name for key
	char				name[ 32 ];
	// Pointer to the underlying structure
	kbutton_t			*pkey;
	// Next key in key list.
	CKeyboardKey		*next;
};

// thx monarch
class CInput {
public:
	char pad_0x0000[0xA4]; //0x0000
	unsigned char N0000002A; //0x00A4 
	bool m_bCameraInThirdperson; //0x00A5 
	char pad_0x00A6[0x2]; //0x00A6
	Vec3D m_vecCameraOffset; //0x00A8 
	char pad_0x00B4[0x18]; //0x00B4
	Vec3D m_angPreviousViewAngles; //0x00CC 
	char pad_0x00D8[0x14]; //0x00D8
	CUserCmd* m_pCommands; //0x00EC 
	CVerifiedUserCmd* m_pVerifiedCommands; //0x00F0 

//	VF( GetUserCmd, CUserCmd*, 8, int slot, int seq );
};
