//========= Copyleft Openmod, All rights reserved. ============================//
//
// Purpose:
//
// $NoKeywords: $
//=============================================================================//
#include "cbase.h"
#include "bindings.h"
#include <string>

static int LMsg(const std::string& fmt)
{
    Msg("%s\n", fmt.c_str());
	return 0;
}

static int LDevMsg(const std::string& fmt)
{
    DevMsg("%s\n", fmt.c_str());
	return 0;
}

static int LWarning(const std::string& fmt)
{
    Warning("%s\n", fmt.c_str());
	return 0;
}

static int LDevWarning(const std::string& fmt)
{
    DevWarning("%s\n", fmt.c_str());
	return 0;
}

static int LConColorMsg(const Color& clr, const std::string& fmt)
{
    ConColorMsg(clr, "%s\n", fmt.c_str());
	return 0;
}

bool bindDbg(sol::state& state)
{
	state.set_function("Msg", &LMsg);
    state.set_function("Warning", &LWarning);
    state.set_function("DevMsg", &LDevMsg);
    state.set_function("DevWarning", &LDevWarning);
	state.set_function("ConColorMsg", &LConColorMsg);

	// overriding slop
	state.set_function("print", &LMsg);

	return true;
}