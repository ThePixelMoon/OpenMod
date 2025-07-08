//========= Copyleft Openmod, All rights reserved. ============================//
//
// Purpose:
//
// $NoKeywords: $
//=============================================================================//
#ifndef BINDINGS_H
#define BINDINGS_H
#ifdef _WIN32
#pragma once
#endif

#include "luamanager.h"

bool bindDbg(sol::state& state);
bool bindColor(sol::state& state);
bool bindVector(sol::state& state);
bool bindBaseEntity(sol::state& state);

#endif