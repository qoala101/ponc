/**
 * PONC @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#ifndef VH_PONC_COREUI_AREA_CREATOR_H_
#define VH_PONC_COREUI_AREA_CREATOR_H_

#include <imgui.h>

#include "coreui_event.h"
#include "cpp_safe_ptr.h"

namespace vh::ponc::coreui {
///
class Diagram;

///
class AreaCreator {
 public:
  ///
  explicit AreaCreator(cpp::SafePtr<Diagram> parent_diagram);

  ///
  auto GetStartPos() const -> const std::optional<ImVec2> &;
  ///
  void StartCreateAreaAt(const ImVec2 &start_pos);
  ///
  auto AcceptCreateAreaAt(const ImVec2 &end_pos) -> Event &;
  ///
  void DiscardCreateArea();

 private:
  ///
  cpp::SafePtr<Diagram> parent_diagram_;
  ///
  std::optional<ImVec2> start_pos_{};
};
}  // namespace vh::ponc::coreui

#endif  // VH_PONC_COREUI_AREA_CREATOR_H_
