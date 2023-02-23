#pragma once

#if SHAPES_PROFILE
#ifdef SHAPES_PLATFORM_WINDOWS
#define TRACY_CALLSTACK 1
#endif
#include <Tracy/Tracy.hpp>
#define SHAPES_PROFILE_SCOPE(name) ZoneScopedN(name)
#define SHAPES_PROFILE_FUNCTION() ZoneScoped
#define SHAPES_PROFILE_FRAMEMARKER() FrameMark
#define SHAPES_PROFILE_LOCK(type, var, name) TracyLockableN(type, var, name)
#define SHAPES_PROFILE_LOCKMARKER(var) LockMark(var)
#define SHAPES_PROFILE_SETTHREADNAME(name) tracy::SetThreadName(name)
#else
#define SHAPES_PROFILE_SCOPE(name)
#define SHAPES_PROFILE_FUNCTION()
#define SHAPES_PROFILE_FRAMEMARKER()
#define SHAPES_PROFILE_LOCK(type, var, name) type var
#define SHAPES_PROFILE_LOCKMARKER(var)
#define SHAPES_PROFILE_SETTHREADNAME(name)

#endif
