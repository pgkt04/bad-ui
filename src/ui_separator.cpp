#include "ui_separator.h"

ui_separator::ui_separator(const char* name)
{
  m_name = name;
}

bool ui_separator::think(std::shared_ptr<ui_style> style_ptr)
{
  set_style(style_ptr);
  return true;
}

void ui_separator::input(ui_input& input)
{
}

void ui_separator::render(std::shared_ptr<ui_draw> draw_ptr)
{
  auto style = get_style();

  if (!style)
    return;

  auto dim = get_dimensions();
  auto line_x = dim.m_x;
  auto line_y = dim.m_y + style->m_control_height * 0.5f - 1.f;

  // Labeled separators draw a short lead line, the text, then the line to the
  // row end, using the backend's measured text width.
  //
  if (m_name)
  {
    auto lead_w = style->m_control_height;
    auto text_x = dim.m_x + lead_w + style->m_padding;

    draw_ptr->draw_rectangle(ui_dimension(dim.m_x, line_y, lead_w, 2.f), style->m_foreground);
    draw_ptr->draw_text(m_name, text_x, dim.m_y, style->m_text);
    line_x = text_x + draw_ptr->measure_text(m_name) + style->m_padding;
  }

  // Rows span to the parent's right edge; controls trim one padding off their
  // right side themselves (same as button/slider/dropdown).
  auto line_w = dim.m_x + dim.m_w - style->m_padding - line_x;

  if (line_w > 0.f)
    draw_ptr->draw_rectangle(ui_dimension(line_x, line_y, line_w, 2.f), style->m_foreground);
}
