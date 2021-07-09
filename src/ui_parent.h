#pragma once
#include "ui_object.h"
#include <vector>

class ui_parent : public ui_object
{
  std::vector<std::shared_ptr<ui_object>> m_children;
  int m_last_tab;

public:
  ui_parent();

  void add_child(std::shared_ptr<ui_object> object);
  std::vector<std::shared_ptr<ui_object>> get_children();
  void handle_relocations(std::shared_ptr<ui_style> style_ptr);
};