/**
 * PONC @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#include "draw_string_buffer.h"

namespace vh::ponc::draw {
///
StringBuffer::StringBuffer(int size) : size_{size} { buffer_.resize(size_); }

///
auto StringBuffer::AsTrimmed() const -> std::string {
  // NOLINTNEXTLINE(*-redundant-string-cstr)
  return std::string{buffer_.c_str()};
}

///
void StringBuffer::Set(std::string value) {
  buffer_ = std::move(value);
  buffer_.resize(std::max(static_cast<int>(buffer_.size() + 1), size_));
}

///
auto StringBuffer::GetSize() const -> int {
  return static_cast<int>(buffer_.size());
}

///
auto StringBuffer::GetData() -> char* { return buffer_.data(); }
}  // namespace vh::ponc::draw