#pragma once

//#if SHAPE_PROFILE
#define TRACY_CALLSTACK 1
#include <Tracy/Tracy.hpp>

#define SHAPE_PROFILE_SCOPE(name) ZoneScopedN(name)
#define SHAPE_PROFILE_FUNCTION() ZoneScoped
#define SHAPE_PROFILE_FRAMEMARKER() FrameMark
#define SHAPE_PROFILE_LOCK(type, var, name) TracyLockableN(type, var, name)
#define SHAPE_PROFILE_LOCKMARKER(var) LockMark(var)
#define SHAPE_PROFILE_SETTHREADNAME(name) tracy::SetThreadName(name)
//#endif // SHAPE_PROFILE
