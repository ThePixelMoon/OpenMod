//========= Copyleft Openmod, All rights reserved. ============================//
//
// Purpose:
//
// $NoKeywords: $
//=============================================================================//
#include "cbase.h"
#include "bindings.h"
#include "baseentity_shared.h"

bool bindBaseEntity(sol::state& state)
{
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

        //"GetModel", &CBaseEntity::GetModel,
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