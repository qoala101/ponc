/**
 * PONC @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#include "coreui_area_creator.h"

#include "core_diagram.h"
#include "coreui_diagram.h"
#include "coreui_event.h"
#include "cpp_assert.h"

namespace vh::ponc::coreui {
///
AreaCreator::AreaCreator(cpp::SafePtr<Diagram> parent_diagram,
                         cpp::SafePtr<core::IdGenerator> id_generator)
    : parent_diagram_{std::move(parent_diagram)},
      id_generator_{std::move(id_generator)} {}

///
auto AreaCreator::IsCreating() const -> bool { return area_id_.has_value(); }

///
auto AreaCreator::StartCreateAreaAt(const ImVec2& start_pos) -> Event& {
  const auto area_id = id_generator_->Generate<core::AreaId>();
  const auto area = core::Area{.id = area_id,
                               .name = "Area #" + std::to_string(area_id.Get()),
                               .pos = start_pos};

  return parent_diagram_->AddArea(area).Then(
      [safe_this = safe_owner_.MakeSafe(this), area_id]() {
        safe_this->area_id_ = area_id;
      });
}

///
void AreaCreator::ResizeAreaTo(const ImVec2& pos) {
  Expects(area_id_.has_value());

  const auto& area =
      core::Diagram::FindArea(parent_diagram_->GetDiagram(), *area_id_);

  parent_diagram_->GetNodeMover().MoveAreaTo(area.id, area.pos, pos - area.pos);
}

///
void AreaCreator::AcceptCreateArea() { area_id_.reset(); }

///
auto AreaCreator::DiscardCreateArea() -> Event& {
  Expects(area_id_.has_value());

  const auto area_id = *area_id_;
  area_id_.reset();

  return parent_diagram_->DeleteArea(area_id);
}
}  // namespace vh::ponc::coreui