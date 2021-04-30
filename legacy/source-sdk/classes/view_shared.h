#pragma once
//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//


//-----------------------------------------------------------------------------
// Flags passed in with view setup
//-----------------------------------------------------------------------------
enum ClearFlags_t
{
	VIEW_CLEAR_COLOR = 0x1,
	VIEW_CLEAR_DEPTH = 0x2,
	VIEW_CLEAR_FULL_TARGET = 0x4,
	VIEW_NO_DRAW = 0x8,
	VIEW_CLEAR_OBEY_STENCIL = 0x10, // Draws a quad allowing stencil test to clear through portals
	VIEW_CLEAR_STENCIL = 0x20,
};

enum StereoEye_t
{
	STEREO_EYE_MONO = 0,
	STEREO_EYE_LEFT = 1,
	STEREO_EYE_RIGHT = 2,
	STEREO_EYE_MAX = 3,
};


class CViewSetup
{
public:
	__int32   x;                 
	__int32   m_nUnscaledX;          
	__int32   y;                
	__int32   m_nUnscaledY;           
	__int32   width;             
	__int32   m_nUnscaledWidth;   
	__int32   height;            
	__int32   m_nUnscaledHeight; 
	char      pad1[ 0x90 ];   
	float     fov;                
	float     fovViewmodel;      
	Vec3D    origin;             
	Vec3D    angles;           
	char      pad2[ 0x7C ]; 

};