/**
 * PONC @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#ifndef VH_PONC_DRAW_LINKS_H_
#define VH_PONC_DRAW_LINKS_H_

#include <vector>

#include "coreui_link.h"
#include "draw_string_buffer.h"

namespace vh::ponc::draw {
///
class Links {
 public:
  ///
  void Draw(const std::vector<coreui::Link> &links);

 private:
  ///
  void DrawLinkDrop(const coreui::Link &link);

  ///
  StringBuffer drop_buffer_{};
};
}  // namespace vh::ponc::draw

#endif  // VH_PONC_DRAW_LINKS_H_
