//========= Copyright OpenMod, All rights reserved. ============//
//
// Purpose:
//
// $NoKeywords: $
//=============================================================================//

#ifndef NEWGAMEMENU_H
#define NEWGAMEMENU_H
#ifdef _WIN32
#pragma once
#endif

#include "basemenu.h"

class NewGame
{
public:
	virtual void		Create(vgui::VPANEL parent) = 0;
	virtual void		Destroy(void) = 0;
	virtual void		Activate(void) = 0;
};

extern NewGame* newgame;

#endif