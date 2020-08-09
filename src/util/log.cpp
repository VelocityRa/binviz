#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include <chrono>
#include <initializer_list>
#include <string>
#include <util/log.hpp>
#include <vector>

namespace logging {

static constexpr auto ENABLE_FILE_LOGGING = false;
static constexpr auto LOG_FILENAME = "voxels.log";
static constexpr auto DEFAULT_LOG_PATTERN = "%^|%L| %16s:%-3# %v%$";

void init() {
  // Set up sinks
  spdlog::sink_ptr cmd_sink = std::make_shared<spdlog::sinks::stdout_color_sink_st>();
  spdlog::sink_ptr file_sink_main = std::make_shared<spdlog::sinks::basic_file_sink_st>(LOG_FILENAME, true);

  std::initializer_list<spdlog::sink_ptr> main_sinks;

  if (ENABLE_FILE_LOGGING)
    main_sinks = { cmd_sink, file_sink_main };
  else
    main_sinks = { cmd_sink };

  // Set up default logger
  auto default_logger = std::make_shared<spdlog::logger>("main", main_sinks);
  default_logger->set_level(spdlog::level::trace);
  default_logger->flush_on(spdlog::level::trace);
  default_logger->set_pattern(DEFAULT_LOG_PATTERN);
  spdlog::register_logger(default_logger);

  // Configure spdlog
  spdlog::set_default_logger(default_logger);
}

}  // namespace logging
