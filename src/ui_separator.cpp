#include "ui_separator.h"

#include <cstring>

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

  // Labeled separators start the line right after the text. ui_draw has no
  // text metrics, so the width is estimated at 8px per character (same
  // convention as ui_text::get_min_width); a fixed label column would leave
  // a hole between short labels and the line start.
  //
  if (m_name)
  {
    draw_ptr->draw_text(m_name, dim.m_x, dim.m_y, style->m_text);
    line_x += static_cast<float>(std::strlen(m_name)) * 8.f + style->m_padding;
  }

  // Rows span to the parent's right edge; controls trim one padding off their
  // right side themselves (same as button/slider/dropdown).
  auto line_w = dim.m_x + dim.m_w - style->m_padding - line_x;

  if (line_w > 0.f)
    draw_ptr->draw_rectangle(ui_dimension(line_x, line_y, line_w, 2.f), style->m_foreground);
}
