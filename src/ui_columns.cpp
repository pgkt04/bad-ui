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
  input_children(input, get_scroll_enabled());
}

bool ui_column::think(std::shared_ptr<ui_style> style_ptr)
{
  set_style(style_ptr);

  auto outer = get_dimensions();
  auto layout = outer;

  if (m_visible)
  {
    layout.m_x += 1.f;
    layout.m_y += 1.f;
    layout.m_w -= 2.f;
    layout.m_h -= 2.f;

    if (layout.m_w < 0.f)
      layout.m_w = 0.f;

    if (layout.m_h < 0.f)
      layout.m_h = 0.f;
  }
  else
  {
    // Hidden columns come from hidden groups and should not create their own
    // top inset; they are just layout rails for the visible child groups.
    layout.m_y -= style_ptr->m_padding;
    layout.m_h += style_ptr->m_padding;
  }

  set_dimensions(layout);
  handle_relocations(style_ptr);
  set_dimensions(outer);

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

  render_children(draw_ptr, get_scroll_enabled());
}
