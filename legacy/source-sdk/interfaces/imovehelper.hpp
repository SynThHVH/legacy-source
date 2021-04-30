#pragma once

enum
{
	WL_NotInWater = 0,
	WL_Feet,
	WL_Waist,
	WL_Eyes
};

class IMoveHelper
{
public:

	VF( SetHost, void, 1, CBaseEntity* host );
};