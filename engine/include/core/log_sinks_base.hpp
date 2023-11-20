#ifndef ENGINEY_LOG_SINKS_BASE_HPP
#define ENGINEY_LOG_SINKS_BASE_HPP

//// Ideas for these custom sinks were taken from https://github.com/gabime/spdlog/pull/2610 
////        which is already merged into v1.12.0 but I am using 1.11.0 and it was simple enough to recreate

#include <mutex>
#include <string>

#include <spdlog/details/null_mutex.h>
#include <spdlog/details/synchronous_factory.h>
#include <spdlog/sinks/base_sink.h>

namespace spdlog {

    typedef std::function<void(const details::log_msg&)> callback_t;

namespace sinks {

    template <typename Mutex>
    class callback_sink : public base_sink<Mutex> {
        callback_t callback;

        protected:
            void sink_it_(const details::log_msg& msg) override { callback(msg); }
            void flush_() override {}

        public:
            explicit callback_sink(callback_t callback) 
                : callback{ callback } {}
    };

    using callback_sink_mt = callback_sink<std::mutex>;
    using callback_sink_st = callback_sink<details::null_mutex>;

} // namespace sinks

    template <typename Factory = spdlog::synchronous_factory>
    inline std::shared_ptr<spdlog::logger> callback_logger_mt(const std::string& logger_name, callback_t callback) {
        return Factory::template create<sinks::callback_sink_mt>(logger_name, callback);
    }

    template <typename Factory = spdlog::synchronous_factory>
    inline std::shared_ptr<spdlog::logger> callback_logger_st(const std::string& logger_name, callback_t callback) {
        return Factory::template create<sinks::callback_sink_st>(logger_name, callback);
    }

} // namespace spdlog

#endif // !YE_LOG_SINKS_BASE_HPP