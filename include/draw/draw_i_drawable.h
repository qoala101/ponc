#ifndef VH_DRAW_I_DRAWABLE_H_
#define VH_DRAW_I_DRAWABLE_H_

namespace esc::draw {
// NOLINTNEXTLINE(*-special-member-functions)
class IDrawable {
 public:
  virtual ~IDrawable() = default;

  virtual void Draw() = 0;

  auto IsVisible() const -> bool;
  void SetVisible(bool visible);

 protected:
  IDrawable() = default;

  auto GetVisible() -> bool &;

  virtual void OnVisibleChanged();

 private:
  bool visible_{};
};
}  // namespace esc::draw

#endif  // VH_DRAW_I_DRAWABLE_H_
