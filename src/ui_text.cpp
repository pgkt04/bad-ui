#include "ui_text.h"

ui_text::ui_text(const char* text)
{
  m_text = text;
}

bool ui_text::think(std::shared_ptr<ui_style> style_ptr)
{
  set_style(style_ptr);
  return true;
}

void ui_text::input(ui_input& input)
{
}

void ui_text::render(std::shared_ptr<ui_draw> draw_ptr)
{
  auto style = get_style();

  if (!style || !m_text)
    return;

  auto dim = get_dimensions();
  draw_ptr->draw_text(m_text, dim.m_x, dim.m_y, style->m_text);
}
