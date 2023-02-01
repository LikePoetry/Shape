#include "hzpch.h"
#include "Log.h"

#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace Shape::Debug
{
	std::shared_ptr<spdlog::logger> Log::s_CoreLogger;
	std::vector<spdlog::sink_ptr> sinks;

	void Log::OnInit()
	{
		sinks.emplace_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>()); // debug
		// sinks.emplace_back(std::make_shared<ImGuiConsoleSink_mt>()); // ImGuiConsole
	
#ifdef LOG_TO_TEXT_FILE
		auto logFileSink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>("LumosLog.txt", 1048576 * 5, 3);
		sinks.emplace_back(logFileSink); // Log file
#endif

		// create the loggers
		s_CoreLogger = std::make_shared<spdlog::logger>("Shape", begin(sinks), end(sinks));
		spdlog::register_logger(s_CoreLogger);

		// configure the loggers
		spdlog::set_pattern("%^[%T] %v%$");
		s_CoreLogger->set_level(spdlog::level::trace);
	}

	void Log::AddSink(spdlog::sink_ptr& sink)
	{
		s_CoreLogger->sinks().push_back(sink);
		s_CoreLogger->set_pattern("%^[%T] %v%$");
	}

	void Log::OnRelease()
	{
		s_CoreLogger.reset();
		spdlog::shutdown();
	}
}