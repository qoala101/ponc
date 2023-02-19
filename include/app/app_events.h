/**
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 */

#ifndef VH_APP_EVENTS_H_
#define VH_APP_EVENTS_H_

#include <memory>

#include "app_state.h"
#include "core_i_family.h"
#include "core_project.h"
#include "imgui.h"

namespace esc::event {
// // ---
// struct OpenDiagramFromFile {
//   // ---
//   void operator()(core::Project &project) const;

//   // ---
//   std::string file_path{};
// };

// // ---
// struct SaveDiagramToFile {
//   // ---
//   void operator()(core::Project &project) const;

//   // ---
//   std::string file_path{};
// };

// // ---
// struct ResetDiagram {
//   // ---
//   void operator()(core::Project &project) const;
// };

// ---
struct CreateNode {
  // ---
  void operator()(const AppState &app_state) const;

  // ---
  std::weak_ptr<core::IFamily> family{};
  // ---
  ImVec2 position{};
};

// // ---
// struct DeleteNode {
//   // ---
//   void operator()(core::Project &project) const;

//   // ---
//   ne::NodeId node_id{};
// };

// // ---
// struct DeleteNodeWithLinks {
//   // ---
//   void operator()(core::Project &project) const;

//   // ---
//   ne::NodeId node_id{};
// };

// // ---
// struct DeleteLink {
//   // ---
//   void operator()(core::Project &project) const;

//   // ---
//   ne::LinkId link_id{};
// };

// // ---
// struct CreateGroup {
//   // ---
//   void operator()(core::Project &project) const;

//   // ---
//   std::vector<ne::NodeId> node_ids{};
// };
}  // namespace esc::event

#endif  // VH_APP_EVENTS_H_
