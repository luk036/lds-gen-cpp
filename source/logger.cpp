#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>

#include <ldsgen/logger.hpp>

namespace ldsgen {

    /// @brief Helper to create the file logger singleton.
    /// @details Wrapped in a noexcept function so the static local initializer
    ///          cannot throw (see bugprone-throwing-static-initialization).
    /// @return The configured file logger.
    auto make_logger() noexcept -> std::shared_ptr<spdlog::logger> {
        auto log = spdlog::basic_logger_mt("file_logger", "ldsgen.log");
        log->set_level(spdlog::level::info);
        log->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%n] [%^%l%$] %v");
        log->flush_on(spdlog::level::info);
        return log;
    }

    void log_with_spdlog(const std::string& message) {
        static auto logger = make_logger();
        logger->info("LdsGen message: {}", message);
        logger->flush();
    }

}  // namespace ldsgen