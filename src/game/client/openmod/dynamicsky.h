//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//
#ifndef __DYNAMICSKY_H
#define __DYNAMICSKY_H
#ifdef _WIN32
#pragma once
#endif

void R_UnloadSkys(void);
void R_LoadSkys(void);
void R_DrawSkyBox(float zFar, int nDrawFlags = 0x3F);

#endif