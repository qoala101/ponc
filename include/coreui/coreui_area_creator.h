/**
 * PONC @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#ifndef VH_PONC_COREUI_AREA_CREATOR_H_
#define VH_PONC_COREUI_AREA_CREATOR_H_

#include <imgui.h>
#include <imgui_node_editor.h>

#include <optional>

#include "core_area.h"
#include "core_id_generator.h"
#include "coreui_event.h"
#include "cpp_safe_ptr.h"

namespace vh::ponc::coreui {
///
class Diagram;

///
class AreaCreator {
 public:
  ///
  AreaCreator(cpp::SafePtr<Diagram> parent_diagram,
              cpp::SafePtr<core::IdGenerator> id_generator);

  ///
  auto IsCreating() const -> bool;
  ///
  auto StartCreateAreaAt(const ImVec2 &start_pos) -> Event &;
  ///
  void ResizeAreaTo(const ImVec2 &pos);
  ///
  void AcceptCreateArea();
  ///
  auto DiscardCreateArea() -> Event &;

 private:
  ///
  struct Area {
    ///
    core::AreaId id{};
    ///
    ImVec2 start_pos{};
  };

  ///
  cpp::SafePtr<Diagram> parent_diagram_;
  ///
  cpp::SafePtr<core::IdGenerator> id_generator_;
  ///
  cpp::SafeOwner safe_owner_{};
  ///
  std::optional<Area> area_{};
};
}  // namespace vh::ponc::coreui

#endif  // VH_PONC_COREUI_AREA_CREATOR_H_
