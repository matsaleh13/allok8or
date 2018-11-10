/**
 * @file diagnostic_util.h
 * @brief Utility classes to help with allocation diagnostics.
 *
 */
#pragma once

// Project headers
#include "portability.h"

// Library headers
#include <cstddef>
#include <cstring>
#include <iostream>

namespace allok8or {
namespace diagnostic {

/**
 * @brief Get the type name at compile time without RTTI.
 *
 * NOTE: Requires parsing compiler-specific strings. Hence, the ugly #ifdefs in the implementation.
 *       Also, I really wanted to use constexpr, but that limits what we can do in parsing the 
 *       string. For ease of implementation in parsing, we allow some variation in returned results.
 *       I.e., returned results are not guaranteed to be the same across different compilers.
 * 
 * Inspiration for this hack:
 * https://blog.molecular-matters.com/2015/12/11/getting-the-type-of-a-template-argument-as-string-without-rtti/.
 * Copyright(c) 2012-2017 Stefan Reinalter
 * MIT License: https://blog.molecular-matters.com/source-code-license/
 * 
 * And also thanks to Antonio Perez, here: https://groups.google.com/a/isocpp.org/forum/#!topic/std-proposals/RKnK3MJpol8
 *
 * @tparam T Type from which to get the name.
 */
template <typename T>
class TypeNameHelper {
public:
  constexpr static const char* get_type_name(void);

private:
  constexpr static void
  get_type_name(const char* func_string, const char*& start, const char*& end);

  static const size_t TYPE_BUFFER_SIZE;
  static char m_type_name[TYPE_BUFFER_SIZE];
};

template <typename T>
const size_t TypeNameHelper<T>::TYPE_BUFFER_SIZE = 128;

template <typename T>
char TypeNameHelper<T>::m_type_name[TYPE_BUFFER_SIZE] = {'\0'};

/**
 * @brief Public method for extracting the name of the type parameter T.
 * 
 * NOTE: Repeated calls to this method should only return the stored contents from the original call.
 * 
 * @tparam T The type from which to get the name string.
 * @return Pointer to a static string for that type.
 */
template <typename T>
constexpr const char* TypeNameHelper<T>::get_type_name(void) {

  if (!m_type_name[0]) {
    // Populate only on first use.

    const char* start = nullptr;
    const char* end = nullptr;

    // std::cout << "ALK8_PRETTY_FUNCTION: " << ALK8_PRETTY_FUNCTION << std::endl;

    get_type_name(ALK8_PRETTY_FUNCTION, start, end);

    ptrdiff_t type_name_size = end - start;
    memcpy(m_type_name, start, type_name_size);
    m_type_name[type_name_size] = 0; // ending null.

    // Adjust whitespace if needed.
    if (m_type_name[type_name_size - 2] == ' ') {
      // account for space before closing angle bracket.
      m_type_name[type_name_size - 2] = 0;
    }
  }

  return m_type_name;
}

#if defined(_MSC_VER)
// MSVS implementation: relies on parsing T from the struct name in
// __FUNCTION__.

/**
 * @brief Parse the type out of the func_string.
 *
 * NOTE: We want `<typename>` from: `[T = <typename>]`, at the end of
 * __PRETTY_FUNCTION__.
 *
 * @param func_string The __PRETTY_FUNCTION__ string from the caller.
 * @param start [OUT] Pointer to start of the type string.
 * @param end [OUT] Pointer to end of the type string.
 */
template <typename T>
constexpr void TypeNameHelper<T>::get_type_name(const char* func_string,
                                                       const char*& start,
                                                       const char*& end) {
  // Find the end of the func_string
  for (end = func_string; *end; ++end)
    ; // TODO: guard against bad data?

  // Walk back to the closing `]`
  --end;

  // Find the `=` delimiter
  for (start = end; *start != '='; --start)
    ;

  // Walk forward to the first char of the type name.
  start += 2; // skip '=' and ' '
}

#elif defined(__clang__) || defined(__GNUC__)
// Clang/GCC implementation: relies on parsing T from the trailing annotation of
// __PRETTY_FUNCTION__.

/**
 * @brief Parse the type out of the func_string.
 *
 * NOTE: We want `<typename>` from: `[T = <typename>]`, at the end of
 * __PRETTY_FUNCTION__.
 *
 * @param func_string The __PRETTY_FUNCTION__ string from the caller.
 * @param start [OUT] Pointer to start of the type string.
 * @param end [OUT] Pointer to end of the type string.
 */
template <typename T>
constexpr void TypeNameHelper<T>::get_type_name(const char* func_string,
                                                       const char*& start,
                                                       const char*& end) {
  // Find the end of the func_string
  for (end = func_string; *end; ++end)
    ; // TODO: guard against bad data?

  // Walk back to the closing `]`
  --end;

  // Find the `=` delimiter
  for (start = end; *start != '='; --start)
    ;

  // Walk forward to the first char of the type name.
  start += 2; // skip '=' and ' '
}

#endif

/**
 * @brief Get the name of the type as a string.
 *
 * @tparam T Type from which to get the name.
 * @return const char* Pointer to the type name.
 */
template <typename T>
constexpr const char* get_type_name(void) {
  return TypeNameHelper<T>::get_type_name();
}

/**
 * Credit for this technique of "stamping" allocated memory with the caller's
 * details goes to: http://www.almostinfinite.com/memtrack.html, Copyright (c)
 * 2002, 2008 Curtis Bartley.
 */

/**
 * Utility for capturing information about a function's caller.
 */
class CallerDetails {
public:
  constexpr CallerDetails(const char* file_name, int line)
      : m_file_name(file_name), m_line(line) {}

  constexpr const char* file_name() const { return m_file_name; }
  constexpr int line() const { return m_line; }

private:
  const char* m_file_name;
  const int m_line;
};

} // namespace diagnostic
} // namespace allok8or