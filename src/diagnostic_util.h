/**
 * @file diagnostic_util.h
 * @brief Utility classes to help with allocation diagnostics.
 *
 */
#pragma once

namespace allok8or {
namespace diagnostic {

/**
 * @brief Get the type name at compile time without RTTI.
 *
 * Credit for this cool hack:
 * https://blog.molecular-matters.com/2015/12/11/getting-the-type-of-a-template-argument-as-string-without-rtti/.
 * Copyright(c) 2012-2017 Stefan Reinalter
 * MIT License: https://blog.molecular-matters.com/source-code-license/
 *
 * @tparam T Type from which to get the name.
 */
template <typename T>
struct TypeNameHelper {
  constexpr static const char* get_type_name(void);
};

#if defined(_MSC_VER)
// MSVS implementation: relies on parsing __FUNCTION__.
template <typename T>
constexpr const char* TypeNameHelper<T>::get_type_name(void) {
  constexpr static const size_t FRONT_SIZE =
      sizeof("allok8or::diagnostic::TypeNameHelper<") - 1u;
  constexpr static const size_t BACK_SIZE = sizeof(">::get_type_name") - 1u;

  constexpr static const char* ALK8_PRETTY_FUNCTION = __FUNCTION__;
  constexpr static const size_t FUNC_SIZE = sizeof(__FUNCTION__);

  constexpr static const size_t type_name_size =
      FUNC_SIZE - FRONT_SIZE - BACK_SIZE;
  static char type_name[type_name_size] = {};

  memcpy(type_name, ALK8_PRETTY_FUNCTION + FRONT_SIZE, type_name_size - 1u);
  if (type_name[type_name_size - 2] == ' ') {
    // account for space before closing angle bracket.
    type_name[type_name_size - 2] = 0;
  }

  return type_name;
}

#elif defined(__clang__) || defined(__GNUC__)
// Clang/GCC implementation: relies on parsing __PRETTY_FUNCTION__.

template <typename T>
constexpr static const size_t TypeNameHelper<T>::FRONT_SIZE =
    sizeof("static const char *allok8or::diagnostic::TypeNameHelper<") - 1u;

template <typename T>
constexpr static const size_t
    TypeNameHelper<T>::BACK_SIZE = sizeof(">::get_type_name") - 1u;

template <typename T>
constexpr static const char* TypeNameHelper<T>::get_type_name(void) {

  constexpr static const char* ALK8_PRETTY_FUNCTION = __PRETTY_FUNCTION__;
  constexpr static const size_t FUNC_SIZE = sizeof(__PRETTY_FUNCTION__);

  constexpr static const size_t type_name_size =
      FUNC_SIZE - FRONT_SIZE - BACK_SIZE;
  static char type_name[type_name_size] = {};

  memcpy(type_name, ALK8_PRETTY_FUNCTION + FRONT_SIZE, type_name_size - 1u);
  if (type_name[type_name_size - 2] == ' ') {
    // account for space before closing angle bracket.
    type_name[type_name_size - 2] = 0;
  }

  return type_name;
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

/**
 * @brief Overload of operator* that merges caller details with the object
 * allocated.
 *
 * @tparam T Data type of the object to be "stamped".
 * @param caller_details A CallerDetails containing the data to be used to
 * "stamp" the object.
 * @param user_data Pointer to the object to be "stamped".
 * @return T* Pointer to the input object.
 */
template <typename T>
inline constexpr T* operator*(const CallerDetails& caller_details,
                              T* user_data) {
  BlockHeader::set_caller_details<T>(caller_details, user_data);
  return user_data;
}

} // namespace diagnostic
} // namespace allok8or