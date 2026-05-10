#pragma once

#include "ui_control.h"

class ui_dropdown : public ui_control
{
  const char* m_name;
  const char** m_items;
  int m_item_count;
  int* m_selected;
  bool m_open;
  bool m_pressed;

public:
  ui_dropdown(const char* name, const char** items, int item_count, int* selected);
  virtual bool think(std::shared_ptr<ui_style> style_ptr) override;
  virtual void input(ui_input& input) override;
  virtual void render(std::shared_ptr<ui_draw> draw_ptr) override;
};
