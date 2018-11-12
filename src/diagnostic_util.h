/**
 * @file diagnostic_util.h
 * @brief Utility classes to help with allocation diagnostics.
 *
 */
#pragma once

// Project headers

// Library headers



namespace allok8or {
namespace diagnostic {

/**
 * Credit for this technique of "stamping" allocated memory with the caller's
 * details goes to: http://www.almostinfinite.com/memtrack.html, Copyright (c)
 * 2002, 2008 Curtis Bartley.
 */

/**
 * Data structure for capturing information about a function's caller.
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