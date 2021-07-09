#include "ui_checkbox.h"

ui_checkbox::ui_checkbox(const char* name, bool* checked)
{
  m_name = name;
  m_checked = checked;
}

bool ui_checkbox::think(std::shared_ptr<ui_style> style_ptr)
{
  set_style(style_ptr);
  return true;
}

void ui_checkbox::input(ui_input& input)
{
  if (!get_style())
    return;

  auto style = get_style();
  auto dimension = get_dimensions();
  auto box_area = ui_dimension(dimension.m_x, dimension.m_y,
    style->m_control_height, style->m_control_height);

  if (UI_IN_AREA(input.mouse, box_area) && input.mouse.buttons[ui_button_left])
    set_selected(true);

  if (!input.mouse.buttons[ui_button_left] && get_selected())
  {
    set_selected(false);
    *m_checked = !(*m_checked);
  }
}

void ui_checkbox::render(std::shared_ptr<ui_draw> draw_ptr)
{
  auto style = get_style();

  if (!style)
    return;

  // we define the w and h here, since we are actually reserved the entire row
  //
  auto dim = get_dimensions();
  dim.m_w = style->m_control_height;
  dim.m_h = style->m_control_height;

  auto base_col = style->m_foreground;

  if (*m_checked)
    base_col = style->m_accent;

  draw_ptr->draw_rectangle(dim, base_col);
  draw_ptr->draw_text(m_name, dim.m_x + dim.m_w + style->m_padding,
    dim.m_y, style->m_text);
}
