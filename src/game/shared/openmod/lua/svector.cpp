//========= Copyleft Openmod, All rights reserved. ============================//
//
// Purpose:
//
// $NoKeywords: $
//=============================================================================//
#include "cbase.h"
#include "bindings.h"

bool bindVector(sol::state& state)
{
    state.new_usertype<Vector>("Vector",
        sol::constructors<Vector(), Vector(float, float, float)>(),

        "x", &Vector::x,
        "y", &Vector::y,
        "z", &Vector::z,

        "Length", &Vector::Length,
        "LengthSqr", &Vector::LengthSqr,
        "Dot", &Vector::Dot,
        "Cross", &Vector::Cross,

        sol::meta_function::addition, [](const Vector& a, const Vector& b) { return a + b; },
        sol::meta_function::subtraction, [](const Vector& a, const Vector& b) { return a - b; },
        sol::meta_function::multiplication, [](const Vector& a, float f) { return a * f; },
        sol::meta_function::to_string, [](const Vector& v) {
            return "Vector(" + std::to_string(v.x) + ", " + std::to_string(v.y) + ", " + std::to_string(v.z) + ")";
        }
    );

    state.new_usertype<QAngle>("QAngle",
        sol::constructors<QAngle(), QAngle(float, float, float)>(),

        "pitch", &QAngle::x,
        "yaw", &QAngle::y,
        "roll", &QAngle::z,

        sol::meta_function::to_string, [](const QAngle& q) {
            return "QAngle(" + std::to_string(q.x) + ", " + std::to_string(q.y) + ", " + std::to_string(q.z) + ")";
        }
    );

    return true;
}