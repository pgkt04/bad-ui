#pragma once

#include "ui_defs.h"
#include "ui_style.h"
#include "ui_draw.h"
#include <memory>

class ui_object
{
  ui_dimension m_dimensions;
  ui_dimension m_parent_dimensions;

  bool m_dynamic;
  bool m_autofill; /// autofill automatically stretches the dynamic area to the size of its children if it is a dynamic type
  bool m_is_tab;
  bool m_is_group;
  bool m_is_col;
  bool m_selected;
  bool m_render_last;

  std::shared_ptr<ui_style> m_style;
  ui_input m_input;
public:

  /// Constructors / deconstructors

  ui_object();

  /// Getter / Setters

  bool get_is_tab();
  void set_is_tab(bool is_tab);

  bool get_is_group();
  void set_is_group(bool is_group);

  bool get_is_col();
  void set_is_col(bool is_col);

  bool get_dynamic();
  void set_dynamic(bool dynamic);

  bool get_autofill();
  void set_autofill(bool autofill);

  bool get_render_last();
  void set_render_last(bool render_last);

  bool get_selected();
  void set_selected(bool selected);

  ui_dimension get_dimensions();
  void set_dimensions(ui_dimension dimension);

  ui_dimension get_parent_dimensions();
  void set_parent_dimensions(ui_dimension dimension);

  std::shared_ptr<ui_style> get_style();
  void set_style(std::shared_ptr<ui_style> style);

  ui_input get_input();
  void set_input(ui_input input);

  /// All functions below must be called by user
  virtual void input(ui_input& input) = 0;
  virtual bool think(std::shared_ptr<ui_style> style_ptr) = 0;
  virtual void render(std::shared_ptr<ui_draw> draw_ptr) = 0;
};