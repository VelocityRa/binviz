#pragma once

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>

#define LOG_TRACE SPDLOG_TRACE
#define LOG_DEBUG SPDLOG_DEBUG
#define LOG_INFO SPDLOG_INFO
#define LOG_WARN SPDLOG_WARN
#define LOG_ERROR SPDLOG_ERROR
#define LOG_CRITICAL SPDLOG_CRITICAL

namespace logging {

void init();

}  // namespace logging
