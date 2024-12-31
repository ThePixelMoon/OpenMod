//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//
#include "cbase.h"
#include "utlvector.h"
#include "materialsystem/imesh.h"
#include "materialsystem/imaterial.h"
#include "tier2/tier2.h"
#include "materialsystem/imaterialsystemhardwareconfig.h"
#include "tier0/vprof.h"
#include "view.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

#define SQRT3INV		(0.57735f)		// a little less than 1 / sqrt(3)

static IMaterial* skyboxMaterials[6] = { NULL, NULL, NULL, NULL, NULL, NULL };

// 1 = s, 2 = t, 3 = 2048
int	st_to_vec[6][3] =
{
	{3,-1,2},
	{-3,1,2},

	{1,3,2},
	{-1,-3,2},

	{-2,-1,3},		
	{2,-1,-3}		
};

int	skytexorder[6] = { 0,2,1,3,4,5 };
#define SIGN(d)	((d)<0?-1:1)
static int		gFakePlaneType[6] = { 1,-1,2,-2,3,-3 };

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void R_UnloadSkys(void)
{
	int i;

	for (i = 0; i < 6; i++)
	{
		if (skyboxMaterials[i])
		{
			skyboxMaterials[i]->DecrementReferenceCount();
			skyboxMaterials[i] = NULL;
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : *name - 
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool R_LoadNamedSkys(const char* skyname)
{
	char		name[512];
	IMaterial* skies[6];
	bool		success = true;
	char* skyboxsuffix[6] = { "rt", "bk", "lf", "ft", "up", "dn" };

	bool bUseDx8Skyboxes = (g_pMaterialSystemHardwareConfig->GetDXSupportLevel() < 90);
	for (int i = 0; i < 6; i++)
	{
		skies[i] = NULL;
		if (bUseDx8Skyboxes)
		{
			Q_snprintf(name, sizeof(name), "skybox/%s_dx80%s", skyname, skyboxsuffix[i]);
			skies[i] = materials->FindMaterial(name, TEXTURE_GROUP_SKYBOX, false);
			if (IsErrorMaterial(skies[i]))
			{
				skies[i] = NULL;
			}
		}

		if (skies[i] == NULL)
		{
			Q_snprintf(name, sizeof(name), "skybox/%s%s", skyname, skyboxsuffix[i]);
			skies[i] = materials->FindMaterial(name, TEXTURE_GROUP_SKYBOX);
		}
		if (!IsErrorMaterial(skies[i]))
			continue;

		success = false;
		break;
	}

	if (!success)
	{
		return false;
	}

	// Increment references
	for (int i = 0; i < 6; i++)
	{
		// Unload any old skybox
		if (skyboxMaterials[i])
		{
			skyboxMaterials[i]->DecrementReferenceCount();
			skyboxMaterials[i] = NULL;
		}

		// Use the new one
		assert(skies[i]);
		skyboxMaterials[i] = skies[i];
		skyboxMaterials[i]->IncrementReferenceCount();
	}

	return true;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void R_LoadSkys(void)
{
	bool success = true;

	char requestedsky[128];

	ConVarRef skyname("sv_skyname");
	if (skyname.IsValid())
	{
		Q_strncpy(requestedsky, skyname.GetString(), sizeof(requestedsky));
	}
	else
	{
		ConDMsg("Unable to find skyname ConVar!!!\n");
		return;
	}

	// See if user's sky will work
	if (!R_LoadNamedSkys(requestedsky))
	{
		// Assume failure
		success = false;

		// See if user requested other than the default
		if (Q_stricmp(requestedsky, "sky_urb01"))
		{
			// Try the default
			skyname.SetValue("sky_urb01");

			// See if we could load that one now
			if (R_LoadNamedSkys(skyname.GetString()))
			{
				ConDMsg("Unable to load sky %s, but successfully loaded %s\n", requestedsky, skyname.GetString());
				success = true;
			}
		}
	}

	if (!success)
	{
		ConDMsg("Unable to load sky %s\n", requestedsky);
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
#pragma warning (disable : 4701)
void MakeSkyVec(float s, float t, int axis, float zFar, Vector& position, Vector2D& texCoord)
{
	Vector		v, b;
	int			j, k;
	float		width;

	width = zFar * SQRT3INV;

	if (s < -1)
		s = -1;
	else if (s > 1)
		s = 1;
	if (t < -1)
		t = -1;
	else if (t > 1)
		t = 1;

	b[0] = s * width;
	b[1] = t * width;
	b[2] = width;

	for (j = 0; j < 3; j++)
	{
		k = st_to_vec[axis][j];
		if (k < 0)
			v[j] = -b[-k - 1];
		else
			v[j] = b[k - 1];
		v[j] += CurrentViewOrigin()[j];
	}

	// avoid bilerp seam
	s = (s + 1) * 0.5;
	t = (t + 1) * 0.5;

	if (s < 1.0 / 512)
		s = 1.0 / 512;
	else if (s > 511.0 / 512)
		s = 511.0 / 512;
	if (t < 1.0 / 512)
		t = 1.0 / 512;
	else if (t > 511.0 / 512)
		t = 511.0 / 512;

	t = 1.0 - t;
	VectorCopy(v, position);
	texCoord[0] = s;
	texCoord[1] = t;
}
#pragma warning (default : 4701)

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void R_DrawSkyBox(float zFar, int nDrawFlags /*= 0x3F*/)
{
	VPROF("R_DrawSkyBox");

	int		i;
	Vector	normal;

	static ConVarRef r_drawskybox("r_drawskybox");
	static ConVarRef mat_loadtextures("mat_loadtextures");
	if (!r_drawskybox.GetInt() || !mat_loadtextures.GetInt())
	{
		return;
	}

	CMatRenderContextPtr pRenderContext(materials);

	for (i = 0; i < 6; i++, nDrawFlags >>= 1)
	{
		// Don't draw this panel of the skybox if the flag isn't set:
		if (!(nDrawFlags & 1))
			continue;

		VectorCopy(vec3_origin, normal);
		switch (gFakePlaneType[i])
		{
		case 1:
			normal[0] = 1;
			break;

		case -1:
			normal[0] = -1;
			break;

		case 2:
			normal[1] = 1;
			break;

		case -2:
			normal[1] = -1;
			break;

		case 3:
			normal[2] = 1;
			break;

		case -3:
			normal[2] = -1;
			break;
		}

		if (DotProduct(CurrentViewForward(), normal) < -0.29289f)
			continue;

		Vector positionArray[4];
		Vector2D texCoordArray[4];
		if (skyboxMaterials[skytexorder[i]])
		{
			pRenderContext->Bind(skyboxMaterials[skytexorder[i]]);

			MakeSkyVec(-1.0f, -1.0f, i, zFar, positionArray[0], texCoordArray[0]);
			MakeSkyVec(-1.0f, 1.0f, i, zFar, positionArray[1], texCoordArray[1]);
			MakeSkyVec(1.0f, 1.0f, i, zFar, positionArray[2], texCoordArray[2]);
			MakeSkyVec(1.0f, -1.0f, i, zFar, positionArray[3], texCoordArray[3]);

			IMesh* pMesh = pRenderContext->GetDynamicMesh();
			CMeshBuilder meshBuilder;
			meshBuilder.Begin(pMesh, MATERIAL_QUADS, 1);

			for (int j = 0; j < 4; ++j)
			{
				meshBuilder.Position3fv(positionArray[j].Base());
				meshBuilder.TexCoord2fv(0, texCoordArray[j].Base());
				meshBuilder.AdvanceVertex();
			}

			meshBuilder.End();
			pMesh->Draw();
		}
	}
}
