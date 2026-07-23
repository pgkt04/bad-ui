#include "ui_checkbox.h"

#include <cstring>

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
  auto fresh_press = take_fresh_press(input);

  if (!input.mouse.buttons[ui_button_left])
  {
    if (get_selected())
    {
      set_selected(false);
      *m_checked = !(*m_checked);
    }

    return;
  }

  if (fresh_press && UI_IN_AREA(input.mouse, box_area))
    set_selected(true);

  // Keep the press captured so no other control reacts until release.
  //
  if (get_selected())
    input.handled = true;
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

float ui_checkbox::get_min_width(std::shared_ptr<ui_style> style)
{
  // Box, one padding, then the label at ~8px per character.
  auto label = m_name ? static_cast<float>(std::strlen(m_name)) * 8.f : 0.f;

  return style->m_control_height + style->m_padding + label + style->m_padding;
}
