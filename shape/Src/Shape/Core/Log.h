#pragma once
#include "Core.h"

#pragma warning(push,0)
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>
#pragma warning(pop)

// Core log macros
#define SHAPE_LOG_TRACE(...) SPDLOG_LOGGER_CALL(::Shape::Debug::Log::GetCoreLogger(), spdlog::level::level_enum::trace, __VA_ARGS__)
#define SHAPE_LOG_INFO(...) SPDLOG_LOGGER_CALL(::Shape::Debug::Log::GetCoreLogger(), spdlog::level::level_enum::info, __VA_ARGS__)
#define SHAPE_LOG_WARN(...) SPDLOG_LOGGER_CALL(::Shape::Debug::Log::GetCoreLogger(), spdlog::level::level_enum::warn, __VA_ARGS__)
#define SHAPE_LOG_ERROR(...) SPDLOG_LOGGER_CALL(::Shape::Debug::Log::GetCoreLogger(), spdlog::level::level_enum::err, __VA_ARGS__)
#define SHAPE_LOG_CRITICAL(...) SPDLOG_LOGGER_CALL(::Shape::Debug::Log::GetCoreLogger(), spdlog::level::level_enum::critical, __VA_ARGS__)

namespace Shape
{
	namespace Debug
	{
		class SHAPE_EXPORT Log
		{
		public:
			static void OnInit();
			static void OnRelease();

			inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }

			static void AddSink(spdlog::sink_ptr& sink);

		private:
			static std::shared_ptr<spdlog::logger> s_CoreLogger;
		};
	}
}