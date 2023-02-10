#ifndef VH_ESC_APP_STATE_H_
#define VH_ESC_APP_STATE_H_

#include "core_app.h"
#include "json_diagram_serializer.h"

namespace esc {
struct AppState {
 public:
  AppState();

  void OpenDiagramFromFile(const std::string &file_path);
  void SaveDiagramToFile(const std::string &file_path) const;

  //  private:
  core::App app_{};
  std::shared_ptr<esc::IdGenerator> id_generator_{};
};
}  // namespace esc

#endif  // VH_ESC_APP_STATE_H_
