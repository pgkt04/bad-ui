#pragma once

#include "ui_control.h"

class ui_checkbox : public ui_control
{
  const char* m_name;
  bool* m_checked;

public:
  ui_checkbox(const char* name, bool* checked);
  virtual bool think(std::shared_ptr<ui_style> style_ptr) override;
  virtual void input(ui_input& input) override;
  virtual void render(std::shared_ptr<ui_draw> draw_ptr) override;
};