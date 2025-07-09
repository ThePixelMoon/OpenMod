//========= Copyleft OpenMod, All rights reserved. ============================//
//
// Purpose:
//
// $NoKeywords: $
//=============================================================================//
#include "cbase.h"
#include "bindings.h"
#include "convar.h"
#include <vector>
#include <memory>

static std::vector<std::unique_ptr<ConVar>> g_CreatedConVars;

bool bindCVar(sol::state& state)
{
    state.new_usertype<ConVar>(
        "ConVar",
        sol::no_constructor,
        "GetName", &ConVar::GetName,
        "GetString", &ConVar::GetString,
        "GetInt", &ConVar::GetInt,
        "GetFloat", &ConVar::GetFloat,
        "SetValue", sol::overload(
            [](ConVar& cv, int v) { cv.SetValue(v); },
            [](ConVar& cv, float v) { cv.SetValue(v); },
            [](ConVar& cv, const char* v) { cv.SetValue(v); }
        )
    );

    state.set_function("CreateCVar",
        [&](const std::string& name, const std::string& defaultValue, int flags, const std::string& help) -> ConVar* {
            ConVar* existing = g_pCVar->FindVar(name.c_str());
            if (existing) {
                return existing;
            }

            auto var = std::make_unique<ConVar>(name.c_str(), defaultValue.c_str(), flags, help.c_str());
            ConVar* rawPtr = var.get();

            g_pCVar->RegisterConCommand(rawPtr);

            g_CreatedConVars.push_back(std::move(var));
            return rawPtr;
        }
    );

    state.set_function("FindCVar", [&](const std::string& name) -> sol::optional<ConVar*> {
        ConVar* var = g_pCVar->FindVar(name.c_str());
        if (var)
            return var;
        return sol::nullopt;
    });

	return true;
}