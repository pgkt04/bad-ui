#pragma once
#include "ui_object.h"
#include "ui_parent.h"
#include <vector>

class ui_tab : public ui_parent
{
  const char* m_name;
  bool m_button_pressed;
  int m_last_tab;
public:
  ui_tab(const char* name);
  void push(std::shared_ptr<ui_object> object);
  virtual bool think(std::shared_ptr<ui_style> style_ptr) override;
  virtual void input(ui_input& input) override;
  virtual void render(std::shared_ptr<ui_draw> draw_ptr) override;
};