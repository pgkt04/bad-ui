#pragma once

#include "ui_defs.h"
#include "ui_style.h"
#include "ui_draw.h"
#include "ui_object.h"
#include "ui_parent.h"
#include <vector>
#include <memory>

class ui_form : public ui_parent
{
  int m_tab_setting;
  const char* m_title;
  bool m_depth_enable;

  /// mouse caching for drag handling
  ui_mouse m_mouse;
  ui_mouse m_last_mouse;
  bool m_resizing;
  // Offset between the cursor and the window's bottom-right corner when the
  // resize started. Resizing tracks the cursor absolutely with this anchor
  // instead of accumulating per-frame deltas, so a drag clamped at the
  // minimum size does not jitter and only resumes once the cursor is back at
  // the corner.
  float m_resize_anchor_x;
  float m_resize_anchor_y;

  int m_last_tab;
public:
  ui_form(ui_dimension dimensions, const char* title, int tab_setting, bool depth_enable);
  ~ui_form();

  void push(std::shared_ptr<ui_object> object);
  virtual bool think(std::shared_ptr<ui_style> style_ptr) override;
  virtual void input(ui_input& input) override;
  virtual void render(std::shared_ptr<ui_draw> draw_ptr) override;
};
