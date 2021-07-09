#pragma once

#include "ui_control.h"

class ui_slider : public ui_control
{
  const char* m_name;
  float* m_value;

public:
  ui_slider(const char* name, float* value);
  virtual bool think(std::shared_ptr<ui_style> style_ptr) override;
  virtual void input(ui_input& input) override;
  virtual void render(std::shared_ptr<ui_draw> draw_ptr) override;
};
