#include "logger.h"

#include "spdlog/sinks/basic_file_sink.h"
#include <mutex>
#include "spdlog/details/null_mutex.h"

namespace fengine {
    using editor_log_sink_mt = editor_log_sink<std::mutex>;
    using editor_log_sink_st = editor_log_sink<spdlog::details::null_mutex>;

	std::shared_ptr<spdlog::logger> s_logger;

	void Logger::Init(EditorUI& editorUI) {
		auto editor_sink = std::make_shared<editor_log_sink_mt>(editorUI);
		auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/log.txt", true);
		spdlog::sinks_init_list sink_list = { file_sink, editor_sink };

		s_logger = std::make_shared<spdlog::logger>("Logger", sink_list.begin(), sink_list.end());
		s_logger->set_level(spdlog::level::trace);
		s_logger->set_pattern("[%Y-%m-%d %H:%M:%S] [%L] %v");
	}
}
