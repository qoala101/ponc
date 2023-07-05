/**
 * PONC @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#include "coreui_area_creator.h"

#include "coreui_diagram.h"
#include "cpp_assert.h"

namespace vh::ponc::coreui {
///
AreaCreator::AreaCreator(cpp::SafePtr<Diagram> parent_diagram)
    : parent_diagram_{std::move(parent_diagram)} {}

///
auto AreaCreator::GetStartPos() const -> const std::optional<ImVec2>& {
  return start_pos_;
}

///
void AreaCreator::StartCreateAreaAt(const ImVec2& start_pos) {
  start_pos_ = start_pos;
}

///
auto AreaCreator::AcceptCreateAreaAt(const ImVec2& end_pos) -> Event& {
  Expects(start_pos_.has_value());

  const auto pos = *start_pos_;
  start_pos_.reset();

  const auto size = end_pos - pos;
  return parent_diagram_->CreateArea(pos, size);
}

///
void AreaCreator::DiscardCreateArea() { start_pos_.reset(); }
}  // namespace vh::ponc::coreui