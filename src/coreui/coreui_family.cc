#include "coreui_family.h"

#include "core_link.h"
#include "coreui_i_family_traits.h"
#include "cpp_assert.h"
#include "imgui_node_editor.h"

namespace esc::coreui {
///
Family::Family(cpp::SafePtr<const core::IFamily> family,
               cpp::SafePtr<core::IdGenerator> id_generator,
               Callbacks callbacks)
    : family_{std::move(family)},
      id_generator_{std::move(id_generator)},
      callbacks_{std::move(callbacks)} {}

///
auto Family::GetLabel() const -> std::string {
  return family_->CreateUiTraits()->GetLabel();
}

///
auto Family::NodesHavePinOfKind(ne::PinKind pin_kind) const -> bool {
  auto id_generator = core::IdGenerator{};
  const auto node = family_->CreateNode(id_generator);

  if (pin_kind == ne::PinKind::Input) {
    return node->GetInputPinId().has_value();
  }

  return !node->GetOutputPinIds().empty();
}

///
void Family::CreateNodeAt(const ImVec2& pos) const {
  auto new_node = family_->CreateNode(*id_generator_);
  new_node->SetPos(pos);

  callbacks_.node_created(std::move(new_node));
}

///
void Family::CreateNodeAtAndConnectTo(const ImVec2& pos, ne::PinId pin_id,
                                      ne::PinKind pin_kind) const {
  auto new_node = family_->CreateNode(*id_generator_);
  new_node->SetPos(pos);

  auto new_link = core::Link{.id = id_generator_->Generate<ne::LinkId>()};

  if (pin_kind == ne::PinKind::Input) {
    new_link.end_pin_id = pin_id;

    const auto& ouput_pins = new_node->GetOutputPinIds();
    Expects(!ouput_pins.empty());

    new_link.start_pin_id = ouput_pins[0];
  } else {
    new_link.start_pin_id = pin_id;

    const auto& input_pin = new_node->GetInputPinId();
    Expects(input_pin.has_value());

    new_link.end_pin_id = *input_pin;
  }

  callbacks_.node_created(std::move(new_node));
  callbacks_.link_created(new_link);
}
}  // namespace esc::coreui