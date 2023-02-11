#ifndef VH_ESC_APP_STATE_H_
#define VH_ESC_APP_STATE_H_

#include "core_app.h"
#include "core_id_generator.h"

namespace esc {
struct State {
 public:
  State();

  void OpenDiagramFromFile(const std::string &file_path);
  void SaveDiagramToFile(const std::string &file_path) const;
  void ResetDiagram();

  //  private:
  core::App app_{};
  std::optional<core::IdGenerator> id_generator_{};
};
}  // namespace esc

#endif  // VH_ESC_APP_STATE_H_
