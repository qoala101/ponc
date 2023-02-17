#include "draw_state.h"

#include <iostream>

#include "app_state.h"
#include "core_state.h"
#include "coreui_i_node_drawer.h"

namespace esc::draw {
auto DrawState::GetColorForFlowValue(float value) const -> ImColor {
  if (!color_flow) {
    return ImColor{255, 255, 255};
  }

  const auto blue = ImColor{0, 0, 255};
  const auto blue_green = ImColor{0, 255, 255};
  const auto green = ImColor{0, 255, 0};
  const auto green_red = ImColor{255, 255, 0};
  const auto red = ImColor{255, 0, 0};

  if (value < min) {
    return blue;
  }

  if (value >= max) {
    return red;
  }

  const auto range = (max - min);
  const auto value_percentage = (value - min) / range;
  const auto low_percentage = (low - min) / range;
  const auto high_percentage = (high - min) / range;

  auto percentage = 0.0F;
  auto start_color = ImColor{};
  auto end_color = ImColor{};

  if (value_percentage < low_percentage) {
    percentage = value_percentage / low_percentage;
    start_color = blue;
    end_color = blue_green;
  } else if (value_percentage >= high_percentage) {
    percentage =
        (value_percentage - high_percentage) / (1.0F - high_percentage);
    start_color = green_red;
    end_color = red;
  } else {
    const auto low_high_range = (high_percentage - low_percentage);
    percentage = (value_percentage - low_percentage) / low_high_range;

    if (percentage < 0.5F) {
      percentage = percentage * 2;
      start_color = blue_green;
      end_color = green;
    } else {
      percentage = (percentage - 0.5F) * 2;
      start_color = green;
      end_color = green_red;
    }
  }

  return ImColor{start_color.Value.x +
                     percentage * (end_color.Value.x - start_color.Value.x),
                 start_color.Value.y +
                     percentage * (end_color.Value.y - start_color.Value.y),
                 start_color.Value.z +
                     percentage * (end_color.Value.z - start_color.Value.z)};
}

auto DrawState::CanConnectFromPinToPin(core::CoreState &core_state,
                                       ne::PinId start_pin, ne::PinId end_pin)
    -> bool {
  if (start_pin == end_pin) {
    return false;
  }

  const auto rebind = new_link.has_value() && new_link->rebind.has_value();
  const auto &families = core_state.diagram_.GetFamilies();
  const auto &links = core_state.diagram_.GetLinks();

  auto *start_node = static_cast<core::INode *>(nullptr);
  auto *end_node = static_cast<core::INode *>(nullptr);

  for (const auto &family : families) {
    for (const auto &node : family->GetNodes()) {
      for (const auto pin : node->GetPinIds()) {
        if (pin == start_pin) {
          start_node = &*node;
        } else if (pin == end_pin) {
          end_node = &*node;
        }
      }
    }
  }

  if (start_node == end_node) {
    std::cout << "r0\n";
    return false;
  }

  auto a = StateNoQueue{.core_state = &core_state, .draw_state = this};
  const auto start_drawer =
      start_node->CreateDrawer(a)->CreatePinDrawer(start_pin);
  const auto end_drawer = end_node->CreateDrawer(a)->CreatePinDrawer(end_pin);

  if (rebind) {
    if (start_drawer->GetKind() != end_drawer->GetKind()) {
      std::cout << "r1\n";
      return false;
    }
  } else {
    if (start_drawer->GetKind() == end_drawer->GetKind()) {
      std::cout << "r2\n";
      return false;
    }
  }

  auto *start_link = static_cast<const core::Link *>(nullptr);
  auto *end_link = static_cast<const core::Link *>(nullptr);

  for (const auto &link : links) {
    if (link.start_pin_id == start_pin || link.end_pin_id == start_pin) {
      start_link = &link;
    } else if (link.start_pin_id == end_pin || link.end_pin_id == end_pin) {
      end_link = &link;
    }
  }

  if (rebind) {
    std::cout << "rebind1\n";
    if (start_link != nullptr && end_link != nullptr) {
      std::cout << "rebind2\n";
      if (start_link != end_link) {
        std::cout << "rebind3\n";
        return false;
      }

      // return f
    }
  } else {
    if (start_link != nullptr || end_link != nullptr) {
      std::cout << "r3\n";
      return false;
    }
  }

  return true;
}
}  // namespace esc::draw