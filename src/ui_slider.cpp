#include "ui_slider.h"

ui_slider::ui_slider(const char* name, float* value)
{
  m_name = name;
  m_value = value;
}

bool ui_slider::think(std::shared_ptr<ui_style> style_ptr)
{
  set_style(style_ptr);

  return false;
}

void ui_slider::input(ui_input& input)
{
  if (!get_style())
    return;

  // TO-DO:
  // handle dragging

}

void ui_slider::render(std::shared_ptr<ui_draw> draw_ptr)
{
  // TO-DO:
  // draw slider
  // i guess we have two modes, the entire width and a fixed length?
  //

  auto style = get_style();

  if (!style)
    return;

  auto dim = get_dimensions();
  auto base_col = style->m_foreground;

  // draw the base
  draw_ptr->draw_rectangle(dim, base_col);
}
