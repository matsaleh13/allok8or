/**
 * @file logging-test.cpp
 * @brief Unit tests for the logging system.
 *
 */

// My header
#include "logging.h"

// Project headers

// Library headers
#include "doctest.h"
#include <string>

using namespace allok8or;

//
// Tests
//

// TODO: full coverage for all log levels (refactor into function).

TEST_CASE("logging") {

  auto format_string = "Formatting numbers: [%d] [%0.3f] [%08x], string: [%s]";
  auto formatted_string =
      "Formatting numbers: [42] [42.420] [00000042], string: [hello world]";

  // String to hold log result.
  std::string test_message;

  // Lambda callback.
  auto log_callback = [&](const char* message, size_t size) {
    test_message = {message, size - 1};
  };

  SUBCASE("register_callback") {
    // First register should be oK
    CHECK_MESSAGE(logging::Logger::register_callback(log_callback),
                  "register_callback returned false");
  }

  SUBCASE("register_callback_when_registered") {
    // First register should be oK
    logging::Logger::register_callback(log_callback);

    // Second should fail
    CHECK_FALSE_MESSAGE(logging::Logger::register_callback(log_callback),
                        "register_callback returned true");
  }

  SUBCASE("log_raw") {
    logging::Logger::register_callback(log_callback);

    LOG_RAW(format_string, 42, 42.42, 0x42, "hello world");

    CHECK_EQ(std::string(formatted_string), test_message);
  }

  SUBCASE("log_raw_when_not_registered") {
    LOG_RAW(format_string, 42, 42.42, 0x42, "hello world");

    // Nothing logged.
    CHECK_NE(std::string(formatted_string), test_message);
  }

  SUBCASE("log_info") {
    logging::Logger::register_callback(log_callback);

    LOG_INFO(format_string, 42, 42.42, 0x42, "hello world");

    CHECK_EQ(std::string(formatted_string), test_message);
  }

  SUBCASE("log_info_when_level_is_warning") {
    logging::Logger::register_callback(log_callback);
    logging::Logger::set_level(logging::Logger::warning);

    LOG_INFO(format_string, 42, 42.42, 0x42, "hello world");

    // Nothing logged.
    CHECK_NE(std::string(formatted_string), test_message);
  }

  // Teardown here

  // Clear static variable that may have been set.
  logging::Logger::register_callback(nullptr);
  logging::Logger::set_level(logging::Logger::invalid);
}