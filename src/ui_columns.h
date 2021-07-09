#pragma once
#include "ui_object.h"
#include "ui_parent.h"

class ui_column : public ui_parent
{
  bool m_visible;
public:
  ui_column(bool visible);
  void push(std::shared_ptr<ui_object> object);
  virtual void input(ui_input& input) override;
  virtual bool think(std::shared_ptr<ui_style> style_ptr) override;
  virtual void render(std::shared_ptr<ui_draw> draw_ptr) override;
};