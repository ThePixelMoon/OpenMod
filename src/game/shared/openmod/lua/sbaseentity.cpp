//========= Copyleft OpenMod, All rights reserved. ============================//
//
// Purpose:
//
// $NoKeywords: $
//=============================================================================//
#include "cbase.h"
#include "bindings.h"
#include "baseentity_shared.h"
#ifdef CLIENT_DLL
#include "cdll_client_int.h"
#else
#include "gameinterface.h"
#endif

class LModel
{
public:
    LModel(const model_t* model) : m_model(model) {}

    std::string GetName() const
    {
        return modelinfo->GetModelName(m_model);
    }

    const void* GetPointer() const { return m_model; }

private:
    const model_t* m_model;
};


bool bindBaseEntity(sol::state& state)
{
	state.new_usertype<LModel>("model_t",
        sol::constructors<LModel(const model_t*)>(),
        "GetName",    &LModel::GetName,
        "GetPointer", &LModel::GetPointer
    );

    state.new_usertype<CBaseEntity>("CBaseEntity",
        sol::constructors<CBaseEntity()>(),

        "entindex", &CBaseEntity::entindex,
        "GetClassname", &CBaseEntity::GetClassname,
        "IsAlive", &CBaseEntity::IsAlive,
        "IsDormant", &CBaseEntity::IsDormant,
        "IsPlayer", &CBaseEntity::IsPlayer,

        "GetAbsOrigin", &CBaseEntity::GetAbsOrigin,
        "SetAbsOrigin", &CBaseEntity::SetAbsOrigin,
        "GetAbsAngles", &CBaseEntity::GetAbsAngles, 
        "SetAbsAngles", &CBaseEntity::SetAbsAngles,

		"GetModel", [](CBaseEntity* ent) {
			return LModel(ent->GetModel());
		},
        "SetModel", &CBaseEntity::SetModel,
        "GetModelIndex", &CBaseEntity::GetModelIndex,
        "SetModelIndex", &CBaseEntity::SetModelIndex,

        "GetFlags", &CBaseEntity::GetFlags,
        "AddFlag", &CBaseEntity::AddFlag,
        "RemoveFlag", &CBaseEntity::RemoveFlag,

        "GetTeamNumber", &CBaseEntity::GetTeamNumber,
        "ChangeTeam", &CBaseEntity::ChangeTeam,

        "GetHealth", &CBaseEntity::GetHealth,
        "SetHealth", &CBaseEntity::SetHealth,
        "GetMaxHealth", &CBaseEntity::GetMaxHealth,
#ifndef CLIENT_DLL
		"SetMaxHealth", &CBaseEntity::SetMaxHealth,
#endif

        "GetCollisionGroup", &CBaseEntity::GetCollisionGroup,
        "SetCollisionGroup", &CBaseEntity::SetCollisionGroup,

        "GetEffects", &CBaseEntity::GetEffects,
        "SetEffects", &CBaseEntity::SetEffects,
        "AddEffects", &CBaseEntity::AddEffects,
        "RemoveEffects", &CBaseEntity::RemoveEffects,

        "GetRenderMode", &CBaseEntity::GetRenderMode,
        "SetRenderMode", &CBaseEntity::SetRenderMode,

        "GetOwnerEntity", &CBaseEntity::GetOwnerEntity,
        "SetOwnerEntity", &CBaseEntity::SetOwnerEntity,

        "GetSimulationTime", &CBaseEntity::GetSimulationTime,
        "SetSimulationTime", &CBaseEntity::SetSimulationTime,

        "IsWorld", &CBaseEntity::IsWorld,
        "IsBaseCombatWeapon", &CBaseEntity::IsBaseCombatWeapon,
        "GetLocalOrigin", &CBaseEntity::GetLocalOrigin,
        "GetLocalAngles", &CBaseEntity::GetLocalAngles
    );

	state.set_function("CreateEntityByName", &CreateEntityByName);

    return true;
}