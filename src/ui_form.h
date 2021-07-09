#pragma once

#include "ui_defs.h"
#include "ui_style.h"
#include "ui_draw.h"
#include "ui_object.h"
#include "ui_parent.h"
#include <vector>
#include <memory>

class ui_form : public ui_parent
{
  int m_tab_setting;
  const char* m_title;
  bool m_depth_enable;

  /// mouse caching for drag handling
  ui_mouse m_mouse;
  ui_mouse m_last_mouse;

  int m_last_tab;
public:
  ui_form(ui_dimension dimensions, const char* title, int tab_setting, bool depth_enable);
  ~ui_form();

  void push(std::shared_ptr<ui_object> object);
  virtual bool think(std::shared_ptr<ui_style> style_ptr) override;
  virtual void input(ui_input& input) override;
  virtual void render(std::shared_ptr<ui_draw> draw_ptr) override;
};