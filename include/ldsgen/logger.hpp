#pragma once

#include <string>

namespace ldsgen {

/**
 * @brief Log a message using spdlog
 *
 * This function provides a simple wrapper around spdlog for logging messages.
 * It creates a file logger that writes to "ldsgen.log" and logs at the info level.
 *
 * @param message The message to log
 */
void log_with_spdlog(const std::string& message);

} // namespace ldsgen