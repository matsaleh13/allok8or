#pragma once

/**
 * @brief Logging support.
 * 
 */

enum LogLevel {
  invalid = 0,
  trace,
  debug,
  info,
  warning,
  error
};

//
// Macros
//
#define LOG(level, message, ...)    // TODO
#define LOG_TRACE(message, ...)     // TODO
#define LOG_DEBUG(message, ...)     // TODO
#define LOG_INFO(message, ...)      // TODO
#define LOG_WARNING(message, ...)   // TODO
#define LOG_ERROR(message, ...)     // TODO

