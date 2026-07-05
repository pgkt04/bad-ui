#pragma once
#include "ui_object.h"
#include <vector>

class ui_parent : public ui_object
{
  std::vector<std::shared_ptr<ui_object>> m_children;
  int m_last_tab;
  float m_scroll_offset;
  float m_content_height;
  bool m_scroll_dragging;
  float m_scroll_drag_offset;
  bool m_scroll_enabled;
  // Separate from ui_object's press tracking: derived classes (tab, form)
  // consume take_fresh_press for the same event before input_children runs.
  bool m_scroll_mouse_was_down;

public:
  ui_parent();

  void add_child(std::shared_ptr<ui_object> object);
  std::vector<std::shared_ptr<ui_object>> get_children();
  bool get_scroll_enabled();
  void set_scroll_enabled(bool enabled);
  void input_children(ui_input& input, bool allow_scroll = true);
  void handle_relocations(std::shared_ptr<ui_style> style_ptr);
  void render_children(std::shared_ptr<ui_draw> draw_ptr, bool draw_scrollbar = true);
  void render_scrollbar(std::shared_ptr<ui_draw> draw_ptr);
};
