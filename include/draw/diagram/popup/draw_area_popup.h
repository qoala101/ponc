/**
 * PONC @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#ifndef VH_PONC_DRAW_AREA_POPUP_H_
#define VH_PONC_DRAW_AREA_POPUP_H_

#include <vector>

#include "core_area.h"
#include "coreui_diagram.h"
#include "cpp_callbacks.h"
#include "draw_string_buffer.h"

namespace vh::ponc::draw {
///
class AreaPopup {
 public:
  ///
  struct Callbacks {
    ///
    cpp::Query<bool> was_just_opened{};
  };

  ///
  void Draw(const std::vector<core::AreaId> &selected_areas,
            coreui::Diagram &diagram, const Callbacks &callbacks);

 private:
  ///
  StringBuffer rename_buffer_{};
};
}  // namespace vh::ponc::draw

#endif  // VH_PONC_DRAW_AREA_POPUP_H_
