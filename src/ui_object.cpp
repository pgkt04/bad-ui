#include "ui_object.h"

ui_object::ui_object()
{
  m_input = {};
  m_dynamic = false;
  m_autofill = false;
  m_is_tab = false;
  m_is_col = false;
  m_is_group = false;
  m_render_last = false;
  m_selected = false;
}

void ui_object::set_dynamic(bool dynamic)
{
  m_dynamic = dynamic;
}

bool ui_object::get_autofill()
{
  if (m_dynamic)
    return m_autofill;

  return false;
}

void ui_object::set_autofill(bool autofill)
{
  m_autofill = autofill;
}

bool ui_object::get_render_last()
{
  return m_render_last;
}

void ui_object::set_render_last(bool render_last)
{
  m_render_last = render_last;
}

bool ui_object::get_selected()
{
  return m_selected;
}

void ui_object::set_selected(bool selected)
{
  m_selected = selected;
}

bool ui_object::get_is_tab()
{
  return m_is_tab;
}

void ui_object::set_is_tab(bool is_tab)
{
  m_is_tab = is_tab;
}

bool ui_object::get_is_group()
{
  return m_is_group;
}

void ui_object::set_is_group(bool is_group)
{
  m_is_group = is_group;
}

bool ui_object::get_is_col()
{
  return m_is_col;
}

void ui_object::set_is_col(bool is_col)
{
  m_is_col = is_col;
}

bool ui_object::get_dynamic()
{
  return m_dynamic;
}

ui_dimension ui_object::get_dimensions()
{
  return m_dimensions;
}

void ui_object::set_dimensions(ui_dimension dimension)
{
  m_dimensions = dimension;
}

ui_dimension ui_object::get_parent_dimensions()
{
  return m_parent_dimensions;
}

void ui_object::set_parent_dimensions(ui_dimension dimension)
{
  m_parent_dimensions = dimension;
}

std::shared_ptr<ui_style> ui_object::get_style()
{
  return m_style;
}

void ui_object::set_style(std::shared_ptr<ui_style> style)
{
  m_style = style;
}

ui_input ui_object::get_input()
{
  return m_input;
}

void ui_object::set_input(ui_input input)
{
  m_input = input;
}

