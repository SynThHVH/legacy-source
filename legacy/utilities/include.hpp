#pragma once

// Windows based includes
#include <windows.h>
#include <shlobj.h>
#include <winuser.h>
#include <process.h>
#include <stdlib.h>
#include <assert.h>
#include <wincon.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstdint>
#include <iostream>
#include <ostream>
#include <fstream>
#include <algorithm>
#include <numeric>
#include <array>
#include <string>
#include <ctime>
#include <map>
#include <unordered_map>
#include <vector>
#include <deque>
#include <random>
#include <chrono>
#include <functional>
#include <future>
#include <tchar.h>
#include <utility>
#include "TlHelp32.h"

// Microsoft detours
#include "../hooking/detours/library/detours.hpp"
#pragma comment(lib, "detours.lib")

// Cheat based functions
#define VF(y, x, z, ...) x y##(__VA_ARGS__)\
	{ \
	    _asm mov eax, dword ptr[ecx]\
	    _asm mov edx, z\
	    _asm imul edx, 4\
	    _asm mov eax, dword ptr[eax + edx]\
	    _asm pop ebp\
	    _asm pop ebp\
	    _asm jmp eax\
	}

#define OF( n, t, o )\
	t &n( )\
	{\
		return *( t * )( (uintptr_t)this + o );\
	}

#define getStackBase() (reinterpret_cast<char*>(_AddressOfReturnAddress()) - sizeof(void*))

// Cheat based includes
#include "../source-sdk/valve/recv.hpp"

#include "fnv1a.hpp"
#include "memory.hpp"
#include "hitboxes.hpp"

#include "../math/vector.hpp"
#include "../math/matrix.hpp"
#include "../math/math.hpp"

#include "../source-sdk/valve/in_buttons.hpp"
#include "../source-sdk/valve/color.h"
#include "../source-sdk/valve/cutlvector.hpp"
#include "../source-sdk/valve/const.hpp"
#include "../source-sdk/valve/checksum_crc.hpp"
#include "../source-sdk/valve/checksum_md5.hpp"

#include "../source-sdk/classes/usercmd.hpp"

#include "../source-sdk/interfaces/cglobalvarsbase.hpp"

#include "../source-sdk/classes/view_shared.h"
#include "../source-sdk/interfaces/input.h"

#include "../source-sdk/classes/iconvar.hpp"
#include "../source-sdk/classes/convar.hpp"

#include "../source-sdk/classes/keyvalues.hpp"

#include "../source-sdk/classes/iappsystem.hpp"
#include "../source-sdk/interfaces/icvar.hpp"

#include "../source-sdk/interfaces/cdll_int.hpp"

#include "../source-sdk/classes/ihandleentity.hpp"

#include "../source-sdk/classes/basehandle.hpp"
#include "../source-sdk/classes/icollideable.hpp"
#include "../source-sdk/classes/iclientnetworkable.hpp"
#include "../source-sdk/classes/client_class.hpp"
#include "../source-sdk/classes/iclientrenderable.hpp"
#include "../source-sdk/classes/iclientunknown.hpp"
#include "../source-sdk/classes/iclientthinkable.hpp"
#include "../source-sdk/classes/icliententity.hpp"
#include "../source-sdk/interfaces/iclientmode.hpp" 

#include "../source-sdk/interfaces/debugoverlay.hpp"
#include "../source-sdk/interfaces/surface.hpp"
#include "../source-sdk/interfaces/trace.hpp"

#include "../source-sdk/interfaces/ipanel.hpp"
#include "../source-sdk/interfaces/isurfaceV30.hpp"

#include "../source-sdk/interfaces/client.hpp"
#include "../source-sdk/interfaces/icliententitylist.hpp"

#include "../source-sdk/classes/imaterial.hpp"
#include "../source-sdk/interfaces/imaterialsystem.hpp"
#include "../source-sdk/interfaces/igametypes.hpp"

#include "../source-sdk/interfaces/ivmodelinfo.hpp"
#include "../source-sdk/interfaces/ivmodelrender.hpp"

#include "../source-sdk/interfaces/vphysics_interfaceV30.hpp"

#include "../source-sdk/interfaces/imovehelper.hpp"
#include "../source-sdk/interfaces/igamemovement.hpp"
#include "../source-sdk/interfaces/prediction.hpp"

#include "../source-sdk/interfaces/istudiorender.hpp"

#include "../source-sdk/interfaces/igameevent.hpp"

#include "../source-sdk/interfaces/ienginesound.hpp"

#include "globals.hpp"

#include "../source-sdk/handling/interfaces.hpp"

#include "drawmanager.hpp"
#include "netvar.hpp"

#include "offsets.hpp"

#include "../source-sdk/classes/cbaseentity.hpp"
#include "../source-sdk/classes/cbasecombatweapon.hpp"

#include "../hooking/vmts/vmts.hpp"
#include "../hooking/handling/hooks.hpp"
#include "../hooking/events/event.hpp"