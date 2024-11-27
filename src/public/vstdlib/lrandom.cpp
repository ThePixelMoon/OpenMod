//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: Random number generator
//
// $Workfile: $
// $NoKeywords: $
//===========================================================================//

#include "cbase.h"
#include "lua.hpp"
#include "luasrclib.h"
#include "mathlib\lvector.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"



static int random_RandomFloat (lua_State *L) {
  lua_pushnumber(L, random->RandomFloat(luaL_optnumber(L, 1, 0.0f), luaL_optnumber(L, 2, 0.0f)));
  return 1;
}

static int random_RandomFloatExp (lua_State *L) {
  lua_pushnumber(L, random->RandomFloatExp(luaL_optnumber(L, 1, 0.0f), luaL_optnumber(L, 2, 0.0f), luaL_optnumber(L, 3, 1.0f)));
  return 1;
}

static int random_RandomInt (lua_State *L) {
  lua_pushinteger(L, random->RandomInt(luaL_checkint(L, 1), luaL_checkint(L, 2)));
  return 1;
}

static int random_SetSeed (lua_State *L) {
  random->SetSeed(luaL_checkint(L, 1));
  return 0;
}

static int random_RandomVector(lua_State* L) {
    int dimensions = luaL_optint(L, 1, 3);
    if (dimensions < 2 || dimensions > 3) {
        return luaL_error(L, "Invalid dimensions, must be 2 or 3.");
    }

    Vector result;
    result.x = random->RandomFloat(-1.0f, 1.0f);
    result.y = random->RandomFloat(-1.0f, 1.0f);
    if (dimensions == 3) {
        result.z = random->RandomFloat(-1.0f, 1.0f);
    }
    else {
        result.z = 0.0f;
    }

    lua_pushvector(L, result);
    return 1;
}

static int random_WeightedRandom(lua_State* L) {
    luaL_checktype(L, 1, LUA_TTABLE);
    luaL_checktype(L, 2, LUA_TTABLE);
    int count = lua_objlen(L, 1);
    if (count != lua_objlen(L, 2)) {
        return luaL_error(L, "Input and weight tables must have the same length.");
    }
    float totalWeight = 0.0f;
    for (int i = 1; i <= count; ++i) {
        lua_rawgeti(L, 2, i);
        totalWeight += lua_tonumber(L, -1);
        lua_pop(L, 1);
    }
    float rnd = random->RandomFloat(0.0f, totalWeight);
    float cumulativeWeight = 0.0f;
    for (int i = 1; i <= count; ++i) {
        lua_rawgeti(L, 2, i);
        cumulativeWeight += lua_tonumber(L, -1);
        lua_pop(L, 1);
        if (rnd <= cumulativeWeight) {
            lua_rawgeti(L, 1, i);
            return 1;
        }
    }
    return luaL_error(L, "Failed to find a valid random choice.");
}

static int random_Shuffle(lua_State* L) {
    luaL_checktype(L, 1, LUA_TTABLE);
    int n = lua_objlen(L, 1);
    for (int i = n; i > 1; --i) {
        int j = random->RandomInt(1, i);
        lua_rawgeti(L, 1, i);
        lua_rawgeti(L, 1, j);
        lua_rawseti(L, 1, i);
        lua_rawseti(L, 1, j);
    }
    lua_pushvalue(L, 1);
    return 1;
}

static const luaL_Reg randomlib[] = {
  {"RandomFloat",   random_RandomFloat},
  {"RandomFloatExp",   random_RandomFloatExp},
  {"RandomInt",   random_RandomInt},
  {"SetSeed",   random_SetSeed},
  {"RandomVector", random_RandomVector},
  {"WeightedRandom", random_WeightedRandom},
  {"Shuffle", random_Shuffle},
  {NULL, NULL}
};

/*
** Open random library
*/
LUALIB_API int luaopen_random (lua_State *L) {
  luaL_register(L, LUA_RANDOMLIBNAME, randomlib);
  // UNDONE: this has always been redundant.
  // luaL_register(L, "_G", random_funcs);
  return 1;
}

