#pragma once

#define SHAPE_EXPORT

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
