#pragma once

struct plane_t
{
	Vec3D m_vNormal;
	float m_flDistance;
	byte m_uType;
	byte m_uSignBytes;
	char pad[ 0x2 ];
};

struct surface_t 
{
	const char* m_csName;
	short m_uName;
	unsigned short m_uFlags;
};