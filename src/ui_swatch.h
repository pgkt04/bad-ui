#pragma once

#include "ui_control.h"

// Non-interactive color display row.
//
class ui_swatch : public ui_control
{
  const ui_color* m_color;

public:
  ui_swatch(const ui_color* color);
  virtual bool think(std::shared_ptr<ui_style> style_ptr) override;
  virtual void input(ui_input& input) override;
  virtual void render(std::shared_ptr<ui_draw> draw_ptr) override;
};
