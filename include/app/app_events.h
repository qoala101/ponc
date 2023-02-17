#ifndef VH_APP_EVENTS_H_
#define VH_APP_EVENTS_H_

#include <memory>

#include "app_state.h"
#include "core_i_family.h"
#include "imgui.h"

namespace esc::event {
// ---
struct OpenDiagramFromFile {
  // ---
  void operator()(StateNoQueue &state) const;

  // ---
  std::string file_path{};
};

// ---
struct SaveDiagramToFile {
  // ---
  void operator()(StateNoQueue &state) const;

  // ---
  std::string file_path{};
};

// ---
struct ResetDiagram {
  // ---
  void operator()(StateNoQueue &state) const;
};

// ---
struct CreateNode {
  // ---
  void operator()(StateNoQueue &state) const;

  // ---
  std::weak_ptr<core::IFamily> family{};
  // ---
  ImVec2 position{};
};

// ---
struct DeleteNode {
  // ---
  void operator()(StateNoQueue &state) const;

  // ---
  ne::NodeId node_id{};
};

// ---
struct DeleteNodeWithLinks {
  // ---
  void operator()(StateNoQueue &state) const;

  // ---
  ne::NodeId node_id{};
};

// ---
struct DeleteLink {
  // ---
  void operator()(StateNoQueue &state) const;

  // ---
  ne::LinkId link_id{};
};

// ---
struct CreateGroup {
  // ---
  void operator()(StateNoQueue &state) const;
};
}  // namespace esc::event

#endif  // VH_APP_EVENTS_H_
