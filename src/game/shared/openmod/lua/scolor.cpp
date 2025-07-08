//========= Copyleft Openmod, All rights reserved. ============================//
//
// Purpose:
//
// $NoKeywords: $
//=============================================================================//
#include "cbase.h"
#include "bindings.h"
#include "Color.h"

bool bindColor(sol::state& state)
{
	state.new_usertype<Color>("Color",
        sol::constructors<
            Color(),
            Color(int, int, int),
            Color(int, int, int, int)
        >(),

        // member functions
        "SetColor", &Color::SetColor,
        "GetColor", [](const Color& c) {
            int r, g, b, a;
            c.GetColor(r, g, b, a);
            return std::make_tuple(r, g, b, a);
        },
        "SetRawColor", &Color::SetRawColor,
        "GetRawColor", &Color::GetRawColor,

        // accessors
        "r", &Color::r,
        "g", &Color::g,
        "b", &Color::b,
        "a", &Color::a,

        "get", [](const Color& c, int index) { return c[index]; },
        "set", [](Color& c, int index, unsigned char value) { c[index] = value; },

        // equality operators
        sol::meta_function::equal_to, &Color::operator==,
        sol::meta_function::to_string, [](const Color& c) {
            return "Color(" + std::to_string(c.r()) + "," + std::to_string(c.g()) + "," + std::to_string(c.b()) + "," + std::to_string(c.a()) + ")";
        }
    );

	return true;
}