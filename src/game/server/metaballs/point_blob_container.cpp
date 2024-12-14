//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

#include "cbase.h"
#include "point_blob_element.h"
#include "point_blob_container.h"

// memdbgon must be the last include file in a .cpp file!!! :apple_advertisement:
#include "tier0/memdbgon.h"

LINK_ENTITY_TO_CLASS(point_blob_container, CPointBlobContainer);

BEGIN_DATADESC(CPointBlobContainer)
	DEFINE_KEYFIELD(GridSize, FIELD_INTEGER, "resolution"),
	DEFINE_KEYFIELD(GridBounds, FIELD_VECTOR, "bounds"),
	DEFINE_KEYFIELD(color, FIELD_COLOR32, "color"),
	DEFINE_KEYFIELD(Ambcolor, FIELD_COLOR32, "ambientcolor"),
	DEFINE_KEYFIELD(colorBoost, FIELD_FLOAT, "brightness"),
	DEFINE_KEYFIELD(BlobMaterialName, FIELD_STRING, "materialpath"),
END_DATADESC()

IMPLEMENT_SERVERCLASS_ST(CPointBlobContainer, DT_PointBlobContainer)
	SendPropInt(SENDINFO(GridSize)),
#ifdef HL2MP
	SendPropVector(SENDINFO(GridBounds), -1, SPROP_COORD_MP),
#else
	SendPropVector(SENDINFO(GridBounds), -1, SPROP_COORD),
#endif
	SendPropInt( SENDINFO(color), 32, SPROP_UNSIGNED, SendProxy_Color32ToInt),
	SendPropInt( SENDINFO(Ambcolor), 32, SPROP_UNSIGNED, SendProxy_Color32ToInt),
	SendPropFloat(SENDINFO(colorBoost)),
	SendPropStringT(SENDINFO(BlobMaterialName)),
END_SEND_TABLE()
