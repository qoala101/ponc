/**
 * PONC @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#include "core_project.h"

#include <imgui_node_editor.h>

#include <algorithm>
#include <exception>
#include <string>
#include <utility>
#include <vector>

#include "core_diagram.h"
#include "core_i_node.h"
#include "core_id_generator.h"
#include "core_id_value.h"
#include "core_link.h"
#include "core_tag.h"
#include "cpp_assert.h"

namespace vh::ponc::core {
namespace {
///
auto HasDiagramNamed(std::string_view name,
                     const std::vector<Diagram>& diagrams) {
  return std::any_of(
      diagrams.cbegin(), diagrams.cend(),
      [name](const auto& diagram) { return diagram.GetName() == name; });
}

///
void RemoveIndexedPostfix(std::string& name, std::string_view postfix) {
  if (postfix.empty()) {
    return;
  }

  const auto postfix_pos = name.find_last_of(postfix);

  if (postfix_pos == std::string::npos) {
    return;
  }

  const auto after_postfix = name.substr(postfix_pos + 1);
  const auto before_postfix_pos = postfix_pos - postfix.size();

  if (after_postfix.empty()) {
    name = name.substr(0, before_postfix_pos);
    return;
  }

  try {
    const auto index = std::stoi(after_postfix);

    if (const auto same_postfix =
            after_postfix == (" " + std::to_string(index))) {
      name = name.substr(0, before_postfix_pos);
    }
  } catch (const std::exception&) {
  }
}
}  // namespace

///
auto Project::FindMaxId() const {
  auto max_id = UnspecifiedIdValue{1};

  for (const auto& family : families_) {
    max_id = std::max(family->GetId().Get(), max_id);
  }

  for (const auto& diagram : diagrams_) {
    for (const auto& node : diagram.GetNodes()) {
      max_id = std::max(node->GetId().Get(), max_id);

      for (const auto& pin : INode::GetAllPins(*node)) {
        max_id = std::max(pin.first.Get(), max_id);
      }
    }

    for (const auto& link : diagram.GetLinks()) {
      max_id = std::max(link.id.Get(), max_id);
    }
  }

  return max_id;
}

///
auto Project::IsEmpty(const Project& project) -> bool {
  return std::all_of(project.diagrams_.cbegin(), project.diagrams_.cend(),
                     [](const auto& diagram) {
                       return diagram.GetNodes().empty() &&
                              diagram.GetLinks().empty();
                     });
}

///
auto Project::FindFamily(const Project& project, FamilyId family_id)
    -> IFamily& {
  const auto family = std::find_if(
      project.families_.cbegin(), project.families_.cend(),
      [family_id](const auto& family) { return family->GetId() == family_id; });

  Expects(family != project.families_.cend());
  return **family;
}

///
auto Project::MakeUniqueDiagramName(const Project& project,
                                    std::string source_name,
                                    std::string_view postfix) -> std::string {
  const auto& diagrams = project.GetDiagrams();

  if (!HasDiagramNamed(source_name, diagrams)) {
    return source_name;
  }

  RemoveIndexedPostfix(source_name, postfix);
  source_name += " ";

  if (!postfix.empty()) {
    source_name += postfix;

    if (!HasDiagramNamed(source_name, diagrams)) {
      return source_name;
    }

    source_name += " ";
  }

  auto index = 2;
  auto name = std::string{};
  auto name_is_unique = false;

  while (!name_is_unique) {
    name = source_name + std::to_string(index);
    name_is_unique = !HasDiagramNamed(name, diagrams);
    ++index;
  }

  return name;
}

///
Project::Project(Settings settings,
                 std::vector<std::unique_ptr<IFamily>> families,
                 std::vector<Diagram> diagrams, Tags tags)
    : settings_{std::move(settings)},
      families_{std::move(families)},
      diagrams_{std::move(diagrams)},
      tags_{std::move(tags)},
      id_generator_{FindMaxId() + 1} {}

///
auto Project::GetSettings() const -> const Settings& {
  // NOLINTNEXTLINE(*-const-cast)
  return const_cast<Project*>(this)->GetSettings();
}

///
auto Project::GetSettings() -> Settings& { return settings_; }

///
auto Project::GetFamilies() const
    -> const std::vector<std::unique_ptr<IFamily>>& {
  return families_;
}

///
auto Project::GetDiagrams() const -> const std::vector<Diagram>& {
  // NOLINTNEXTLINE(*-const-cast)
  return const_cast<Project*>(this)->GetDiagrams();
}

///
auto Project::GetDiagrams() -> std::vector<Diagram>& { return diagrams_; }

///
auto Project::EmplaceDiagram(Diagram diagram) -> Diagram& {
  return diagrams_.emplace_back(std::move(diagram));
}

///
void Project::DeleteDiagram(int index) {
  Expects(static_cast<int>(diagrams_.size()) > index);
  diagrams_.erase(diagrams_.cbegin() + index);
}

///
auto Project::GetTags() const -> const Tags& {
  // NOLINTNEXTLINE(*-const-cast)
  return const_cast<Project*>(this)->GetTags();
}

///
auto Project::GetTags() -> Tags& { return tags_; }

///
auto Project::EmplaceTag(std::shared_ptr<Tag> tag)
    -> const std::shared_ptr<Tag>& {
  return tags_.emplace_back(tag);
}

///
void Project::DeleteTag(std::string_view tag_name) {
  std::erase_if(tags_,
                [tag_name](const auto& tag) { return tag->name == tag_name; });
}

///
auto Project::GetIdGenerator() const -> const IdGenerator& {
  // NOLINTNEXTLINE(*-const-cast)
  return const_cast<Project*>(this)->GetIdGenerator();
}

///
auto Project::GetIdGenerator() -> IdGenerator& { return id_generator_; }
}  // namespace vh::ponc::core