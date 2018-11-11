/**
 * @file portability.h
 * @brief Platform-independent definitions.
 *
 */

#pragma once

#if defined(_MSC_VER)
#elif defined(__clang__)
#elif (defined(linux) || defined(__unix__)) && defined(__GNUC__)
#else
  std::static_assert(false);  // Unsupported compiler
#endif


// "Pretty Function" macro
#if defined(_MSC_VER)
  #define ALK8_PRETTY_FUNCTION __FUNCTION__
#elif defined (__clang__)
  #define ALK8_PRETTY_FUNCTION __PRETTY_FUNCTION__
#elif defined(__GNUC__)
  #define ALK8_PRETTY_FUNCTION __PRETTY_FUNCTION__
#endif