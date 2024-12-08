//========= Copyright OpenMod, All rights reserved. ============//
//
// Purpose:
//
// $NoKeywords: $
//=============================================================================//

#ifndef SPAWNMENU_H
#define SPAWNMENU_H
#ifdef _WIN32
#pragma once
#endif

#include "basemenu.h"

class SMLPanel
{
public:
	virtual void		Create(vgui::VPANEL parent) = 0;
	virtual void		Destroy(void) = 0;
	virtual void		Activate(void) = 0;
};

extern SMLPanel* smlmenu;

#endif