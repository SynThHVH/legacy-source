#pragma once
/*
* C_HookManager
* For hooking virtual functionsand memory to other memory / functions
*/

class C_HookManager
{
public:
	template <typename T>
	static T hook_vtable(void* pOriginal, void* pNew, int iIndex);
	static void start();
};

extern C_HookManager* Hook;