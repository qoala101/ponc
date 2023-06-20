/**
 * PONC @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#ifndef VH_PONC_DRAW_STRING_BUFFER_H_
#define VH_PONC_DRAW_STRING_BUFFER_H_

#include <string>

namespace vh::ponc::draw {
///
class StringBuffer {
 public:
  ///
  explicit StringBuffer(int size = 255);

  ///
  auto AsTrimmed() const -> std::string;
  ///
  auto GetSize() const -> int;
  ///
  auto GetData() -> char *;
  ///
  void Set(std::string value);
  ///
  void Clear();

 private:
  ///
  int size_{};
  ///
  std::string buffer_{};
};
}  // namespace vh::ponc::draw

#endif  // VH_PONC_DRAW_STRING_BUFFER_H_
