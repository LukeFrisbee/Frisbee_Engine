#pragma once

#include <memory>
#include "spdlog/spdlog.h"
#include "spdlog/sinks/base_sink.h"

#include "f_imgui_draw.h"

namespace fengine {
    template<typename Mutex>
    class editor_log_sink : public spdlog::sinks::base_sink <Mutex>
    {
    public:
        editor_log_sink(EditorUI& editorUI) : m_editorUI{ editorUI } {};
    protected:
        EditorUI& m_editorUI;

        void sink_it_(const spdlog::details::log_msg& msg) override
        {
            // log_msg is a struct containing the log entry info like level, timestamp, thread id etc.
            // msg.raw contains pre formatted log

            // If needed (very likely but not mandatory), the sink formats the message before sending it to its final destination:
            spdlog::memory_buf_t formatted;
            spdlog::sinks::base_sink<Mutex>::formatter_->format(msg, formatted);
            m_editorUI.Log(fmt::to_string(formatted));
        }

        void flush_() override
        {
            m_editorUI.Flush();
        }
    };

	class Logger {
	public:
		static void Init(EditorUI& editorUI);

		inline static std::shared_ptr<spdlog::logger>& GetLogger() { return s_logger; }

	private:
		inline static std::shared_ptr<spdlog::logger> s_logger;
	};
}

#define LOG_INFO(...) ::fengine::Logger::GetLogger()->info(__VA_ARGS__)
#define LOG_DEBUG(...) ::fengine::Logger::GetLogger()->debug(__VA_ARGS__)
#define LOG_WARN(...) ::fengine::Logger::GetLogger()->warn(__VA_ARGS__)
#define LOG_ERROR(...) ::fengine::Logger::GetLogger()->error(__VA_ARGS__)
#define LOG_CRITCAL(...) ::fengine::Logger::GetLogger()->critcal(__VA_ARGS__)