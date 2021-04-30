#include "../../features/features.hpp"

/* 
note:
	i was working on adding an animation fix at one point but i'm too lazy to do that when i can paste supremacy and add shit from pandora because their the same sources :)
*/

void __fastcall Hooked::DoExtraBoneProcessing( void* ecx, void* edx, CStudioHdr *hdr, Vec3D *pos, Vec4D *q, matrix3x4_t *matrix, CBoneBitList &bone_list, CIKContext *context ) 
{
	return;
}

void __fastcall Hooked::UpdateClientSideAnimations( void* ecx, void* edx ) 
{
	
}