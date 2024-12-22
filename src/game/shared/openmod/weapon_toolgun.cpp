//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#include "cbase.h"
#include "npcevent.h"
#include "in_buttons.h"

#ifdef CLIENT_DLL
#define CWeaponMultitool C_WeaponMultitool
#endif

#include "weapon_toolgun.h"

// effects stuff
#include "beam_shared.h"
#include "IEffects.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

// defines.. wow
#define BEAM_SPRITE1	"sprites/toolgunbeam.vmt"

// rest of the weapon
IMPLEMENT_NETWORKCLASS_ALIASED(WeaponMultitool, DT_WeaponMultitool)

BEGIN_NETWORK_TABLE(CWeaponMultitool, DT_WeaponMultitool)
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA(CWeaponMultitool)
END_PREDICTION_DATA()

LINK_ENTITY_TO_CLASS(weapon_multitool, CWeaponMultitool);
PRECACHE_WEAPON_REGISTER(weapon_multitool);

#ifndef CLIENT_DLL
ConVar toolmode("toolmode", "0");
ConVar red("ttt_red", "0");
ConVar green("ttt_green", "0");
ConVar blue("ttt_blue", "0");
ConVar duration("ttt_duration", "0");
ConVar exp_magnitude("ttt_exp_magnitude", "0");
ConVar exp_radius("ttt_exp_radius", "0");
ConVar tool_create("ttt_tool_create", "");

acttable_t CWeaponMultitool::m_acttable[] =
{
    { ACT_HL2MP_IDLE,                    ACT_HL2MP_IDLE_PISTOL,                    false },
    { ACT_HL2MP_RUN,                     ACT_HL2MP_RUN_PISTOL,                     false },
    { ACT_HL2MP_IDLE_CROUCH,             ACT_HL2MP_IDLE_CROUCH_PISTOL,             false },
    { ACT_HL2MP_WALK_CROUCH,             ACT_HL2MP_WALK_CROUCH_PISTOL,             false },
    { ACT_HL2MP_GESTURE_RANGE_ATTACK,    ACT_HL2MP_GESTURE_RANGE_ATTACK_PISTOL,    false },
    { ACT_HL2MP_GESTURE_RELOAD,          ACT_HL2MP_GESTURE_RELOAD_PISTOL,          false },
    { ACT_HL2MP_JUMP,                    ACT_HL2MP_JUMP_PISTOL,                    false },
    { ACT_RANGE_ATTACK1,                 ACT_RANGE_ATTACK_PISTOL,                  false },
};

IMPLEMENT_ACTTABLE(CWeaponMultitool);
#endif // CLIENT_DLL

CWeaponMultitool::CWeaponMultitool(void)
{
#ifndef CLIENT_DLL
	m_igniter = nullptr;
#endif // CLIENT_DLL
    m_bReloadsSingly = false;
    m_bFiresUnderwater = true;
}

void CWeaponMultitool::Precache(void)
{
    BaseClass::Precache();

    PrecacheModel(BEAM_SPRITE1);
    PrecacheMaterial(BEAM_SPRITE1);
}

bool CWeaponMultitool::Deploy(void)
{
    BaseClass::Deploy();
    return true;
}

bool CWeaponMultitool::Holster(CBaseCombatWeapon* pSwitchingTo)
{
    BaseClass::Holster(pSwitchingTo);
    return true;
}

void CWeaponMultitool::PrimaryAttack(void)
{
    CBasePlayer* pPlayer = ToBasePlayer(GetOwner());
    if (!pPlayer)
        return;

	QAngle vecAngles(0, GetAbsAngles().y - 90, 0);

    // trace
    Vector vecStart = pPlayer->EyePosition();
    Vector vecForward;
    pPlayer->EyeVectors(&vecForward);

    Vector vecEnd = vecStart + (vecForward * MAX_TRACE_LENGTH);
    trace_t tr;
    UTIL_TraceLine(vecStart, vecEnd, MASK_SHOT, pPlayer, COLLISION_GROUP_NONE, &tr);

    // sparks..
    g_pEffects->Sparks(tr.endpos);

	if (gpGlobals->maxClients == 1)
	{
		// spawn beam
		CBeam* pBeam = CBeam::BeamCreate(BEAM_SPRITE1, 2.0f);
		pBeam->PointEntInit(tr.endpos, this);
		pBeam->SetEndAttachment(1);

		// beam color
		pBeam->SetColor(48, 179, 215);

		pBeam->SetBrightness(255);
		pBeam->SetNoise(0);
		pBeam->LiveForTime(0.1f);
	};

    // other stuff
    WeaponSound(SINGLE);
    pPlayer->DoMuzzleFlash();

    SendWeaponAnim(ACT_VM_PRIMARYATTACK);
    pPlayer->SetAnimation(PLAYER_ATTACK1);

#ifndef CLIENT_DLL
	m_iMode = toolmode.GetInt();

	switch (m_iMode)
	{
	case 0:
	{
		if (tr.m_pEnt->IsNPC() || tr.m_pEnt->VPhysicsGetObject())
		{
			if (tr.m_pEnt->IsPlayer())
			{
				ClientPrint(pPlayer, HUD_PRINTCONSOLE, "No.\n");
				break;
			}

			UTIL_Remove(tr.m_pEnt);
		}
	}
	break;

	case 1:
		if (tr.m_pEnt->IsNPC() || tr.m_pEnt->VPhysicsGetObject())
		{
			tr.m_pEnt->SetRenderColor(red.GetInt(), green.GetInt(), blue.GetInt(), 255);
		}
		break;

	case 2:
		if (tr.m_pEnt->IsNPC() || tr.m_pEnt->VPhysicsGetObject())
		{
			float a = duration.GetFloat();
			dynamic_cast<CBaseAnimating*>(tr.m_pEnt)->SetModelScale(a, 0.0f);
		}
		break;

	case 3:
		if (tr.m_pEnt->IsNPC() || tr.m_pEnt->VPhysicsGetObject())
		{
			if (m_igniter)
				UTIL_Remove(m_igniter);

			m_igniter = CEntityFlame::Create(tr.m_pEnt, true);
		}
		break;
	case 4: {}
	break;
	case 5:
	{
		CBaseEntity* pEntity = CreateEntityByName(tool_create.GetString());
		if (pEntity)
		{
			pEntity->SetAbsOrigin(tr.endpos);
			pEntity->SetAbsAngles(vecAngles);
			DispatchSpawn(pEntity);
			pEntity->Activate();
		}
	}
	break;
	case 6: {}
	}
#endif // CLIENT_DLL

    m_flNextPrimaryAttack = gpGlobals->curtime + 0.15;
    m_flNextSecondaryAttack = gpGlobals->curtime + 0.15;
}
