//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//
#ifndef WEAPON_TOOLGUN_H
#define WEAPON_TOOLGUN_H
#ifdef _WIN32
#pragma once
#endif

#include "weapon_hl2mpbasehlmpcombatweapon.h"

#ifndef CLIENT_DLL
#include "ehandle.h"
#include "util.h"
#include "EntityFlame.h"
#endif // CLIENT_DLL

class CWeaponMultitool : public CBaseHL2MPCombatWeapon
{
	DECLARE_CLASS(CWeaponMultitool, CBaseHL2MPCombatWeapon);
public:

	CWeaponMultitool(void);

	void	PrimaryAttack(void);
	void	Precache(void);

    bool    Deploy(void);
    bool    Holster(CBaseCombatWeapon* pSwitchingTo);
	bool HasAnyAmmo() { return true; }
	bool HasPrimaryAmmo() { return true; }
	bool HasSecondaryAmmo() { return true; }

	DECLARE_NETWORKCLASS();
	DECLARE_PREDICTABLE();

#ifndef CLIENT_DLL
	DECLARE_ACTTABLE();
#endif // CLIENT_DLL

private:
#ifndef CLIENT_DLL
	CHandle<CEntityFlame> m_igniter;
#endif // CLIENT_DLL
	CNetworkVar(int, m_iMode);

	CWeaponMultitool(const CWeaponMultitool&);
};

#endif