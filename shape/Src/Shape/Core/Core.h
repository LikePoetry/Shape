#pragma once

#define SHAPE_EXPORT
#define SHAPE_PROFILE
#define BIT(x) (1 << x)

#ifdef SHAPE_ENABLE_ASSERTS

#define SHAPE_ASSERT_NO_MESSAGE(condition)        \
    {                                             \
        if(!(condition))                          \
        {                                         \
            SHAPE_LOG_ERROR("Assertion Failed!"); \
            SHAPE_BREAK();                        \
        }                                         \
    }

#define SHAPE_ASSERT_MESSAGE(condition, ...)     \
    {                                            \
        if(!(condition))                         \
        {                                        \
            SHAPE_LOG_ERROR("Assertion Failed"); \
            SHAPE_LOG_ERROR(__VA_ARGS__);        \
            SHAPE_BREAK();                       \
        }                                        \
    }

#define SHAPE_CLIENT_ASSERT SHAPE_ASSERT_MESSAGE
#define SHAPE_CORE_ASSERT SHAPE_ASSERT_MESSAGE
#else
#define SHAPE_CLIENT_ASSERT(...)
#define SHAPE_CORE_ASSERT(...)
#define SHAPE_ASSERT_NO_MESSAGE(...)
#define SHAPE_ASSERT_MESSAGE(condition)
#endif

#ifdef SHAPE_ENGINE
#define SHAPE_ASSERT SHAPE_CORE_ASSERT
#endif // SHAPE_ENGINE


#define NONCOPYABLE(type_identifier)                             \
    type_identifier(const type_identifier&)            = delete; \
    type_identifier& operator=(const type_identifier&) = delete;


#define DISABLE_WARNING_PUSH __pragma(warning(push))
#define DISABLE_WARNING_POP __pragma(warning(pop))
#define DISABLE_WARNING(warningNumber) __pragma(warning(disable \
                                                        : warningNumber))

#define DISABLE_WARNING_UNREFERENCED_FORMAL_PARAMETER DISABLE_WARNING(4100)
#define DISABLE_WARNING_UNREFERENCED_FUNCTION DISABLE_WARNING(4505)
#define DISABLE_WARNING_CONVERSION_TO_SMALLER_TYPE DISABLE_WARNING(4267)
