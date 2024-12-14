//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
// $NoKeywords: $
//=============================================================================//

#include "cbase.h"
#include "METABALL.h"
#include "c_point_blob_element.h"

// memdbgon must be the last include file in a .cpp file!!! :apple_advertisement:
#include "tier0/memdbgon.h"

LINK_ENTITY_TO_CLASS(point_blob_element, C_PointBlobElement);

IMPLEMENT_NETWORKCLASS_ALIASED(PointBlobElement, DT_PointBlobElement)

BEGIN_NETWORK_TABLE(C_PointBlobElement, DT_PointBlobElement)
RecvPropFloat(RECVINFO(radius)),
RecvPropFloat(RECVINFO(radiusSquared)),
END_NETWORK_TABLE()

void C_PointBlobElement::Spawn()
{
	BaseClass::Spawn();

	VPhysicsInitNormal(SOLID_VPHYSICS, 0, false);
	SetMoveType(MOVETYPE_VPHYSICS);
	if (VPhysicsGetObject())
	{
		VPhysicsGetObject()->EnableCollisions(true);
		VPhysicsGetObject()->SetMass(100.0f);
		SetGravity(-1.0f);
	}
}

C_PointBlobElement::C_PointBlobElement()
{
	radius = 10;
	radiusSquared = radius * radius;
	metaball = METABALL();

	VPhysicsInitNormal(SOLID_VPHYSICS, 0, false);
	SetMoveType(MOVETYPE_VPHYSICS);
	if (VPhysicsGetObject())
	{
		VPhysicsGetObject()->EnableCollisions(true);
		VPhysicsGetObject()->SetMass(100.0f);
		SetGravity(-1.0f);
	}

}

C_PointBlobElement::C_PointBlobElement(int newRadius)
{
	radius = newRadius;
	radiusSquared = radius * radius;
	metaball = METABALL();

}

void C_PointBlobElement::VPhysicsUpdate(IPhysicsObject* pPhysics)
{
	Vector vecVelocity(0, 0, NULL);
	if (pPhysics)
	{
		pPhysics->AddVelocity(&vecVelocity, &vecVelocity);
	}

	BaseClass::VPhysicsUpdate(pPhysics);
}

void C_PointBlobElement::Simulate()
{
	BaseClass::Simulate();

	VPhysicsInitNormal(SOLID_VPHYSICS, 0, false);
	SetMoveType(MOVETYPE_VPHYSICS);
	if (VPhysicsGetObject())
	{
		VPhysicsGetObject()->EnableCollisions(true);
		VPhysicsGetObject()->SetMass(100.0f);
		SetGravity(-1.0f);
	}
	if(!First)
	{ 
		pos = GetAbsOrigin();
		SpawnTime = gpGlobals->curtime;
		First = true;
	}

	Vector eeee;
	eeee.x = pos.x + sin(gpGlobals->curtime - SpawnTime);
	eeee.y = pos.y + sin(gpGlobals->curtime - SpawnTime);
	eeee.z = pos.z + sin(gpGlobals->curtime - SpawnTime);
	SetAbsOrigin(eeee);

	metaball.squaredRadius = radius * radius;
	metaball.position = GetAbsOrigin();
#ifdef _DEBUG
	NDebugOverlay::Sphere(GetAbsOrigin(), sqrt(radius), 255, 0, 0, true, 0.0015f);
#endif
}