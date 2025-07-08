//========= Copyleft Openmod, All rights reserved. ============================//
//
// Purpose:
//
// $NoKeywords: $
//=============================================================================//
#include "cbase.h"
#include "luamanager.h"
#include "bindings.h"

LuaManager g_LuaManager;

static ConVar sv_allow_clientside_lua("sv_allow_clientside_lua", "1", FCVAR_REPLICATED | FCVAR_NOTIFY | FCVAR_ARCHIVE, "Allow clients to run clientside lua commands");

LuaManager::LuaManager()
{
}

LuaManager::~LuaManager()
{
}

bool LuaManager::Initialize()
{
	mState.open_libraries(
		sol::lib::base,
		sol::lib::coroutine,
		sol::lib::string,
		sol::lib::math,
		sol::lib::table,
		sol::lib::utf8
	);

	// this is gonna be messy but i don't care
	bindDbg(mState);
	bindColor(mState);
	bindVector(mState);

	return true;
}

#ifdef GAME_DLL
CON_COMMAND_F(lua_dostr, "Run Lua string", FCVAR_NONE)
{
	if ( !UTIL_IsCommandIssuedByServerAdmin() )
		return;
	
    if (args.ArgC() < 2)
    {
        Msg("Usage: lua_dostr <code>\n");
        return;
    }

    std::string luaCode;
	for (int i = 1; i < args.ArgC(); i++)
    {
        if (i > 1)
            luaCode += " ";
        luaCode += args[i];
    }

    try
    {
        g_LuaManager.getState().script(luaCode);
    }
    catch (const sol::error& e)
    {
        Warning("Lua error: %s\n", e.what());
    }
}
#else
CON_COMMAND_F(lua_dostr_cl, "Run Lua string (clientside)", FCVAR_NONE)
{
	if (!sv_allow_clientside_lua.GetBool())
		return;

    if (args.ArgC() < 2)
    {
        Msg("Usage: lua_dostr_cl <code>\n");
        return;
    }

    std::string luaCode;
	for (int i = 1; i < args.ArgC(); i++)
    {
        if (i > 1)
            luaCode += " ";
        luaCode += args[i];
    }

    try
    {
        g_LuaManager.getState().script(luaCode);
    }
    catch (const sol::error& e)
    {
        Warning("Lua error: %s\n", e.what());
    }
}
#endif