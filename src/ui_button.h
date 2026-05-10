#pragma once

#include "ui_control.h"

#include <functional>

class ui_button : public ui_control
{
  const char* m_name;
  std::function<void()> m_callback;
  bool m_pressed;

public:
  ui_button(const char* name, std::function<void()> callback);
  virtual bool think(std::shared_ptr<ui_style> style_ptr) override;
  virtual void input(ui_input& input) override;
  virtual void render(std::shared_ptr<ui_draw> draw_ptr) override;
};
