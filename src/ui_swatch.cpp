#include "ui_swatch.h"

ui_swatch::ui_swatch(const ui_color* color)
{
  m_color = color;
}

bool ui_swatch::think(std::shared_ptr<ui_style> style_ptr)
{
  set_style(style_ptr);
  return true;
}

void ui_swatch::input(ui_input& input)
{
}

void ui_swatch::render(std::shared_ptr<ui_draw> draw_ptr)
{
  auto style = get_style();

  if (!style || !m_color)
    return;

  auto dim = get_dimensions();

  // Rows span to the parent's right edge; controls trim one padding off
  // their right side themselves. 1px vertical inset so adjacent rows breathe.
  //
  draw_ptr->draw_rectangle(ui_dimension(dim.m_x, dim.m_y + 1.f,
    dim.m_w - style->m_padding, style->m_control_height - 2.f), *m_color);
}
