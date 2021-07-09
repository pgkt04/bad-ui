#include "ui_form.h"
#include <Windows.h>

// parent class for all controls
// tab_settings - 0 (none), 1 (top), 2 (bottom), 3 (left), 4 (right)
//
ui_form::ui_form(ui_dimension dimensions, const char* title, int tab_setting, bool depth_enable)
{
  set_dimensions(dimensions);
  m_title = title;
  m_tab_setting = tab_setting;
  m_depth_enable = depth_enable;
  m_mouse = {};
  m_last_mouse = {};
  m_last_tab = -1;
}

ui_form::~ui_form()
{
}

void ui_form::push(std::shared_ptr<ui_object> object)
{
  add_child(object);
}

bool ui_form::think(std::shared_ptr<ui_style> style_ptr)
{
  set_style(style_ptr);

  m_last_mouse.pos_x = m_mouse.pos_x;
  m_last_mouse.pos_y = m_mouse.pos_y;
  m_mouse.pos_x = get_input().mouse.pos_x;
  m_mouse.pos_y = get_input().mouse.pos_y;

  // apply dragging
  if (get_selected())
  {
    auto dimensions = get_dimensions();
    auto delta_x = (m_mouse.pos_x - m_last_mouse.pos_x);
    auto delta_y = (m_mouse.pos_y - m_last_mouse.pos_y);
    dimensions.m_x += delta_x;
    dimensions.m_y += delta_y;

    if (abs(delta_x) > 1.f || abs(delta_y) > 1.f)
      set_dimensions(dimensions);
  }

  handle_relocations(style_ptr);

  for (auto child : get_children())
  {
    child->set_style(style_ptr);
    child->think(style_ptr);
  }

  return true;
}

void ui_form::input(ui_input& input)
{
  set_input(input);

  /// drag handling
  auto style = get_style();

  if (style)
  {
    auto draggable_area = get_dimensions();
    draggable_area.m_h = style->m_window_title_height;

    if (!input.mouse.buttons[ui_button_left])
      set_selected(false);

    else if (UI_IN_AREA(input.mouse, draggable_area))
      set_selected(true);
  }

  for (auto child : get_children())
    child->input(input);
}

void ui_form::render(std::shared_ptr<ui_draw> draw_ptr)
{
  // background
  draw_ptr->draw_rectangle(get_dimensions(), get_style()->m_background);

  // text area
  auto text_dimension = get_dimensions();
  text_dimension.m_h = get_style()->m_window_title_height;
  draw_ptr->draw_rectangle(text_dimension, get_style()->m_foreground);
  draw_ptr->draw_text(m_title, get_dimensions().m_x, get_dimensions().m_y, get_style()->m_accent);

  // render all children

  for (auto child : get_children())
  {
    if (!child->get_render_last())
      child->render(draw_ptr);
  }

  for (auto child : get_children())
  {
    if (child->get_render_last())
      child->render(draw_ptr);
  }

  // render cursor
  draw_ptr->draw_text("X", m_mouse.pos_x, m_mouse.pos_y, get_style()->m_accent);
}
