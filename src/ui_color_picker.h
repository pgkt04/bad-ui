#pragma once

#include "ui_control.h"

class ui_color_picker : public ui_control
{
  const char* m_name;
  ui_color* m_color;
  bool m_open;
  bool m_pressed;
  int m_drag_mode;
  float m_hue;
  float m_saturation;
  float m_value;

public:
  ui_color_picker(const char* name, ui_color* color);
  virtual bool think(std::shared_ptr<ui_style> style_ptr) override;
  virtual void input(ui_input& input) override;
  virtual void render(std::shared_ptr<ui_draw> draw_ptr) override;
  virtual float get_min_width(std::shared_ptr<ui_style> style) override;
};
