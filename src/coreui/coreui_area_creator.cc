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
auto AreaCreator::IsCreating() const -> bool { return area_.has_value(); }

///
auto AreaCreator::StartCreateAreaAt(const ImVec2& start_pos) -> Event& {
  const auto area_id = id_generator_->Generate<core::AreaId>();
  const auto area = core::Area{.id = area_id,
                               .name = "Area #" + std::to_string(area_id.Get()),
                               .pos = start_pos};

  return parent_diagram_->AddArea(area).Then(
      [safe_this = safe_owner_.MakeSafe(this),
       area = Area{.id = area_id, .start_pos = start_pos}]() {
        safe_this->area_ = area;
      });
}

///
void AreaCreator::ResizeAreaTo(const ImVec2& pos) {
  Expects(area_.has_value());
  parent_diagram_->GetNodeMover().MoveAreaTo(area_->id, area_->start_pos, pos);
}

///
void AreaCreator::AcceptCreateArea() { area_.reset(); }

///
auto AreaCreator::DiscardCreateArea() -> Event& {
  Expects(area_.has_value());

  const auto area_id = area_->id;
  area_.reset();

  return parent_diagram_->DeleteArea(area_id);
}
}  // namespace vh::ponc::coreui