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

public:
  ui_parent();

  void add_child(std::shared_ptr<ui_object> object);
  std::vector<std::shared_ptr<ui_object>> get_children();
  void input_children(ui_input& input);
  void handle_relocations(std::shared_ptr<ui_style> style_ptr);
  void render_children(std::shared_ptr<ui_draw> draw_ptr);
  void render_scrollbar(std::shared_ptr<ui_draw> draw_ptr);
};
