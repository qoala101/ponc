/**
 * PONC @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#include "coreui_node_replacer.h"

#include <algorithm>
#include <compare>
#include <iterator>
#include <optional>
#include <utility>

#include "core_diagram.h"
#include "core_i_family.h"
#include "core_id_generator.h"
#include "core_project.h"
#include "coreui_diagram.h"
#include "coreui_event_loop.h"
#include "coreui_project.h"

namespace vh::ponc::coreui {
///
NodeReplacer::NodeReplacer(cpp::SafePtr<coreui::Project> parent_project)
    : parent_project_{std::move(parent_project)} {}

///
auto NodeReplacer::CanReplaceNode(const core::INode& source_node,
                                  const core::INode& target_node) const
    -> bool {
  if (const auto same_family =
          source_node.GetFamilyId() == target_node.GetFamilyId()) {
    return true;
  }

  const auto& diagram = parent_project_->GetDiagram().GetDiagram();
  const auto& source_input_pin = source_node.GetInputPinId();
  const auto source_has_input_link =
      source_input_pin.has_value() &&
      core::Diagram::HasLink(diagram, *source_input_pin);

  if (source_has_input_link && !target_node.GetInputPinId().has_value()) {
    return false;
  }

  const auto& target_output_pins = target_node.GetOutputPinIds();
  const auto& source_output_pins = source_node.GetOutputPinIds();

  if (target_output_pins.size() >= source_output_pins.size()) {
    return true;
  }

  const auto num_linked_output_pins =
      std::count_if(source_output_pins.cbegin(), source_output_pins.cend(),
                    [&diagram](const auto pin_id) {
                      return core::Diagram::HasLink(diagram, pin_id);
                    });

  return static_cast<int>(target_output_pins.size()) >= num_linked_output_pins;
}

///
auto NodeReplacer::RewireUsedIds(
    const core::INode& source_node,
    const std::vector<ne::PinId>& source_output_pins,
    core::INode& target_node) const {
  const auto target_ids = target_node.GetIds();
  *target_ids.node_id = source_node.GetId();

  const auto& diagram = parent_project_->GetDiagram().GetDiagram();
  auto ids_to_generate = std::vector<ne::PinId*>{};

  if (target_ids.input_pin_id.has_value()) {
    const auto source_input_pin = source_node.GetInputPinId();
    const auto source_input_is_linked =
        source_input_pin.has_value() &&
        core::Diagram::HasLink(diagram, *source_input_pin);

    if (source_input_is_linked) {
      **target_ids.input_pin_id = *source_input_pin;
    } else {
      **target_ids.input_pin_id = core::UnspecifiedIdValue{};
      ids_to_generate.emplace_back(*target_ids.input_pin_id);
    }
  }

  const auto num_source_pins_with_links =
      std::count_if(source_output_pins.cbegin(), source_output_pins.cend(),
                    [&diagram](const auto pin_id) {
                      return core::Diagram::HasLink(diagram, pin_id);
                    });

  const auto& target_pins = target_ids.output_pin_ids;
  auto num_empty_pins_available =
      static_cast<int>(target_pins.size()) - num_source_pins_with_links;

  auto source_pin = source_output_pins.cbegin();

  for (auto target_pin = target_pins.cbegin(); target_pin != target_pins.cend();
       ++target_pin) {
    if (source_pin == source_output_pins.cend()) {
      **target_pin = core::UnspecifiedIdValue{};
      ids_to_generate.emplace_back(*target_pin);
      continue;
    }

    const auto prev_source_pin = *source_pin;
    ++source_pin;

    if (core::Diagram::HasLink(diagram, prev_source_pin)) {
      **target_pin = prev_source_pin;
      continue;
    }

    if (num_empty_pins_available > 0) {
      **target_pin = core::UnspecifiedIdValue{};
      ids_to_generate.emplace_back(*target_pin);
      --num_empty_pins_available;
      continue;
    }

    --target_pin;
  }

  return ids_to_generate;
}

///
auto NodeReplacer::GetFreeIds(
    std::vector<core::UnspecifiedIdValue> source_ids,
    std::vector<core::UnspecifiedIdValue> target_ids,
    std::vector<core::UnspecifiedIdValue> reusable_ids) const {
  std::sort(source_ids.begin(), source_ids.end());
  std::sort(reusable_ids.begin(), reusable_ids.end());

  auto all_reusable_ids = std::vector<core::UnspecifiedIdValue>{};
  std::set_union(source_ids.cbegin(), source_ids.cend(), reusable_ids.cbegin(),
                 reusable_ids.cend(), std::back_inserter(all_reusable_ids));

  std::sort(target_ids.begin(), target_ids.end());

  auto free_ids = std::vector<core::UnspecifiedIdValue>{};
  std::set_difference(all_reusable_ids.cbegin(), all_reusable_ids.cend(),
                      target_ids.cbegin(), target_ids.cend(),
                      std::back_inserter(free_ids));

  auto& id_generator = parent_project_->GetProject().GetIdGenerator();
  const auto next_id = std::lower_bound(free_ids.cbegin(), free_ids.cend(),
                                        id_generator.GetNextId());
  free_ids.erase(next_id, free_ids.cend());

  return free_ids;
}

///
void NodeReplacer::ReuseSourceIds(
    std::vector<core::UnspecifiedIdValue> source_ids,
    std::vector<core::UnspecifiedIdValue> target_ids,
    std::vector<core::UnspecifiedIdValue> reusable_ids,
    const std::vector<ne::PinId*>& ids_to_generate) const {
  const auto free_ids = GetFreeIds(std::move(source_ids), std::move(target_ids),
                                   std::move(reusable_ids));

  auto id_to_generate = ids_to_generate.cbegin();
  auto id_to_reuse = free_ids.cbegin();

  for (; (id_to_generate != ids_to_generate.cend()) &&
         (id_to_reuse != free_ids.cend());
       ++id_to_generate, ++id_to_reuse) {
    **id_to_generate = *id_to_reuse;
  }

  auto& id_generator = parent_project_->GetProject().GetIdGenerator();
  auto prev_generated_id = id_generator.GetNextId() - 1;

  if (id_to_reuse != free_ids.cend()) {
    for (auto unused_id = free_ids.cend() - 1;
         (unused_id >= id_to_reuse) && (*unused_id == prev_generated_id);
         --unused_id, --prev_generated_id) {
    }
  }

  id_generator = core::IdGenerator{prev_generated_id + 1};

  if (id_to_generate != ids_to_generate.cend()) {
    for (; id_to_generate != ids_to_generate.cend(); ++id_to_generate) {
      **id_to_generate = id_generator.Generate<ne::PinId>();
    }
  }
}

///
auto NodeReplacer::ReplaceNode(
    const core::INode& source_node,
    const std::vector<ne::PinId>& source_output_pins,
    std::unique_ptr<core::INode> target_node,
    std::vector<core::UnspecifiedIdValue> reusable_ids) const -> Event& {
  const auto ids_to_generate =
      RewireUsedIds(source_node, source_output_pins, *target_node);

  ReuseSourceIds(core::INode::GetIds(source_node),
                 core::INode::GetIds(std::as_const(*target_node)),
                 std::move(reusable_ids), ids_to_generate);

  target_node->SetPos(source_node.GetPos());

  parent_project_->GetEventLoop().PostEvent(
      [parent_project = parent_project_, node_id = source_node.GetId()]() {
        parent_project->GetDiagram().GetDiagram().DeleteNode(node_id);
      });

  return parent_project_->GetDiagram().AddNode(std::move(target_node));
}
}  // namespace vh::ponc::coreui