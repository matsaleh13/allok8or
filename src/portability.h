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
