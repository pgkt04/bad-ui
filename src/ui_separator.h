#pragma once

#include "ui_control.h"

class ui_separator : public ui_control
{
  const char* m_name;

public:
  ui_separator(const char* name = nullptr);
  virtual bool think(std::shared_ptr<ui_style> style_ptr) override;
  virtual void input(ui_input& input) override;
  virtual void render(std::shared_ptr<ui_draw> draw_ptr) override;
};
