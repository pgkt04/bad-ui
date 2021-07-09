#include "ui_columns.h"

ui_column::ui_column(bool visible)
{
  set_dynamic(true);
  set_is_col(true);

  m_visible = visible;
}

void ui_column::push(std::shared_ptr<ui_object> object)
{
  add_child(object);
}

void ui_column::input(ui_input& input)
{
  set_input(input);

  for (auto child : get_children())
    child->input(input);
}

bool ui_column::think(std::shared_ptr<ui_style> style_ptr)
{
  set_style(style_ptr);

  handle_relocations(style_ptr);

  for (auto child : get_children())
    child->think(style_ptr);

  return true;
}

void ui_column::render(std::shared_ptr<ui_draw> draw_ptr)
{
  // draw the outline then render all its children
  //
  if (m_visible)
  {
    // TODO:
    //
    draw_ptr->draw_rectangle(get_dimensions(), get_style()->m_text);
    auto inner = get_dimensions();
    inner.m_x += 1;
    inner.m_y += 1;
    inner.m_w -= 2;
    inner.m_h -= 2;
    draw_ptr->draw_rectangle(inner, get_style()->m_background);
  }

  for (auto child : get_children())
    child->render(draw_ptr);
}
