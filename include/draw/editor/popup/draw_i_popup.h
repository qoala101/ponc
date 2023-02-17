#ifndef VH_DRAW_I_POPUP_H_
#define VH_DRAW_I_POPUP_H_

#include <string>

#include "cpp_interface.h"

namespace esc {
class State;
}  // namespace esc

namespace esc::draw {
// ---
class IPopup : public cpp::Interface {
 public:
  // ---
  void Show();
  // ---
  void Draw(State &state);

 private:
  // ---
  virtual auto GetLabel [[nodiscard]] () const -> std::string = 0;
  // ---
  virtual void DrawContent(State &state) = 0;
};
}  // namespace esc::draw

#endif  // VH_DRAW_I_POPUP_H_
