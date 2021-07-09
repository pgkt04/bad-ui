#include "ui_tab.h"

ui_tab::ui_tab(const char* name)
{
  set_dynamic(true);
  set_is_tab(true);

  m_name = name;
  m_button_pressed = false;
  m_last_tab = -1;
}

void ui_tab::push(std::shared_ptr<ui_object> object)
{
  add_child(object);
}

bool ui_tab::think(std::shared_ptr<ui_style> style_ptr)
{
  set_style(style_ptr);
  handle_relocations(style_ptr);

  for (auto child : get_children())
    child->think(style_ptr);

  return true;
}

void ui_tab::input(ui_input& input)
{
  if (!get_style())
    return;

  /// if the area is pressed
  auto tab_area = get_dimensions();
  tab_area.m_h = get_style()->m_control_height;

  if (input.mouse.buttons[ui_button_left])
  {
    if (UI_IN_AREA(input.mouse, tab_area) && !m_button_pressed)
    {
      m_button_pressed = true;
    }
  }
  else if (m_button_pressed)
  {
    set_selected(true);

    m_button_pressed = false;
  }

  if (get_selected())
  {
    for (auto child : get_children())
      child->input(input);
  }
}

void ui_tab::render(std::shared_ptr<ui_draw> draw_ptr)
{
  /// Draw tab
  auto dimensions = get_dimensions();
  auto style = get_style();

  /// background
  /// draw_ptr->draw_rectangle(ui_dimension(dimensions.m_x, dimensions.m_y, dimensions.m_w, dimensions.m_h), style->m_text);

  draw_ptr->draw_rectangle(ui_dimension(dimensions.m_x, dimensions.m_y, dimensions.m_w, style->m_control_height), style->m_foreground);
  draw_ptr->draw_text(m_name, dimensions.m_x, dimensions.m_y, style->m_text);

  /// Render all children
  if (get_selected())
  {
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
  }
}
