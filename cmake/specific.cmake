# Specific dependencies for LdsGen project

# Add spdlog for logging functionality
CPMAddPackage(
  NAME spdlog
  GIT_TAG v1.17.0
  GITHUB_REPOSITORY gabime/spdlog
  OPTIONS "SPDLOG_INSTALL YES" # create an installable target
)

set(SPECIFIC_LIBS fmt::fmt spdlog::spdlog)
