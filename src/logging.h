/**
 * @file logging.h
 * @brief Support for logging to an external callback.
 *
 */
#pragma once

// Project headers

// Library headers
#include <cstddef>
#include <functional>

//
// Macros
//
#define LOG(level, message, ...)                                               \
  allok8or::logging::Logger::log(level, message, __VA_ARGS__)
#define LOG_RAW(message, ...)                                                  \
  LOG(allok8or::logging::Logger::raw, message, __VA_ARGS__)
#define LOG_TRACE(message, ...)                                                \
  LOG(allok8or::logging::Logger::trace, message, __VA_ARGS__)
#define LOG_DEBUG(message, ...)                                                \
  LOG(allok8or::logging::Logger::debug, message, __VA_ARGS__)
#define LOG_INFO(message, ...)                                                 \
  LOG(allok8or::logging::Logger::info, message, __VA_ARGS__)
#define LOG_WARNING(message, ...)                                              \
  LOG(allok8or::logging::Logger::warning, message, __VA_ARGS__)
#define LOG_ERROR(message, ...)                                                \
  LOG(allok8or::logging::Logger::error, message, __VA_ARGS__)

namespace allok8or {
namespace logging {

/**
 * @brief Prototype of the log callback function.
 *
 * NOTE: This class is not intrinsically thread-safe. That really only matters
 * for calls to set_level and register_callback. The caller can make the
 * callback thread-safe if desired.
 *
 * @param const char* Pointer to the log message string.
 * @param size_t Size of the log message string.
 */
typedef std::function<void(const char*, size_t)> LogFunc;

class Logger {
  static const size_t MAX_LOG_BUFFER = 1024;

public:
  enum LogLevel { raw = -1, invalid = 0, trace, debug, info, warning, error };

  static bool register_callback(LogFunc log_func);
  static void set_level(int level) { s_level = level; }

  template <typename... Args>
  constexpr static void log(int level, const char* format, Args... args);

private:
  static LogFunc s_log_callback;
  static int s_level;
};

//
// Static inits.
//
LogFunc Logger::s_log_callback = nullptr;
int Logger::s_level = Logger::invalid;

/**
 * @brief Set the callback function.
 *
 * NOTE: Call this with (nullptr) if already set and you want to change it.
 * NOTE: Not thread safe.
 *
 * @param log_func Function pointer to set as the log callback.
 * @return true If callback is not already set.
 * @return false If callback is already set.
 */
bool Logger::register_callback(LogFunc log_func) {
  if (!s_log_callback || log_func == nullptr) {
    s_log_callback = log_func;
    return true;
  }

  return false;
}

/**
 * @brief Call the log callback if it is set.
 *
 * NOTE: Passes the message string and its size, including terminating null, to
 * the callbaqck.
 *
 * @tparam Args Variadic template args for formatting the string.
 * @param level Internal log level for allok8or only.
 * @param format Format string of the style for printf.
 * @param args Variadic function args to pass to the format string.
 */
template <typename... Args>
constexpr void Logger::log(int level, const char* format, Args... args) {
  if (s_log_callback) {
    if (level == Logger::raw || level >= Logger::s_level) {
      constexpr const size_t buf_size = MAX_LOG_BUFFER;
      char buf[buf_size];
      const size_t actual_size = std::snprintf(nullptr, 0, format, args...) + 1;
      std::snprintf(buf, actual_size, format, args...);

      s_log_callback(buf, actual_size);
    }
  }
}

} // namespace logging
} // namespace allok8or