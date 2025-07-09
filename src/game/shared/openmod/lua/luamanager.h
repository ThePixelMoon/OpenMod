//========= Copyleft OpenMod, All rights reserved. ============================//
//
// Purpose:
//
// $NoKeywords: $
//=============================================================================//
#ifndef LUAMANAGER_H
#define LUAMANAGER_H
#ifdef _WIN32
#pragma once
#endif

#include <sol/sol.hpp>

class LuaManager
{
public:
	LuaManager();
	virtual ~LuaManager();

	virtual bool Initialize();

	virtual sol::state& getState()
	{
        return mState;
    }

	// prevent copying
	LuaManager(const LuaManager&) = delete;
    LuaManager& operator=(const LuaManager&) = delete;

private:
    sol::state mState;
};

extern LuaManager g_LuaManager;

#endif