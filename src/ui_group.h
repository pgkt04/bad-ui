#pragma once

#include "ui_object.h"
#include "ui_parent.h"
#include "ui_columns.h"
#include <vector>

class ui_group : public ui_parent
{
  bool m_visible;
  bool m_init;
  int m_object_count;

  /// keeps track of where the split should happen
  std::vector<int> m_splits;
  std::vector<std::shared_ptr<ui_object>> m_children;
  std::vector<std::shared_ptr<ui_column>> m_columns;
public:
  ui_group(bool visible);
  void split();

  void push(std::shared_ptr<ui_object> object);
  virtual void input(ui_input& input) override;
  virtual bool think(std::shared_ptr<ui_style> style_ptr) override;
  virtual void render(std::shared_ptr<ui_draw> draw_ptr) override;
};