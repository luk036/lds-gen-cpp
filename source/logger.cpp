#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>

#include <ldsgen/logger.hpp>

namespace ldsgen {

void log_with_spdlog(const std::string& message) {
    static auto logger = []() -> std::shared_ptr<spdlog::logger> {
        auto log = spdlog::basic_logger_mt("file_logger", "ldsgen.log");
        log->set_level(spdlog::level::info);
        log->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%n] [%^%l%$] %v");
        log->flush_on(spdlog::level::info);
        return log;
    }();
    logger->info("LdsGen message: {}", message);
    logger->flush();
}

}  // namespace ldsgen