/**
 * PONC @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#ifndef VH_PONC_DRAW_AREA_CREATOR_H_
#define VH_PONC_DRAW_AREA_CREATOR_H_

#include "coreui_area_creator.h"

namespace vh::ponc::draw {
///
class AreaCreator {
 public:
  ///
  void Draw(coreui::AreaCreator &area_creator);

 private:
  ///
  void Reset();

  ///
  bool right_mouse_clicked_{};
  ///
  bool right_mouse_dragged_{};
};
}  // namespace vh::ponc::draw

#endif  // VH_PONC_DRAW_AREA_CREATOR_H_
