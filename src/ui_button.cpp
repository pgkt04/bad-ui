#include "ui_button.h"

#include <cstring>

static ui_dimension get_button_area(ui_dimension dimension, std::shared_ptr<ui_style> style)
{
  return ui_dimension(
    dimension.m_x,
    dimension.m_y,
    dimension.m_w - style->m_padding,
    style->m_control_height
  );
}

ui_button::ui_button(const char* name, std::function<void()> callback)
{
  m_name = name;
  m_callback = callback;
  m_pressed = false;
}

bool ui_button::think(std::shared_ptr<ui_style> style_ptr)
{
  set_style(style_ptr);
  return true;
}

void ui_button::input(ui_input& input)
{
  auto style = get_style();

  if (!style)
    return;

  auto button_area = get_button_area(get_dimensions(), style);
  auto in_button = UI_IN_AREA(input.mouse, button_area);

  if (!input.mouse.buttons[ui_button_left])
  {
    if (m_pressed)
    {
      input.handled = true;

      if (in_button && m_callback)
        m_callback();
    }

    m_pressed = false;
    return;
  }

  if (m_pressed)
  {
    input.handled = true;
    return;
  }

  if (in_button)
  {
    m_pressed = true;
    input.handled = true;
  }
}

void ui_button::render(std::shared_ptr<ui_draw> draw_ptr)
{
  auto style = get_style();

  if (!style)
    return;

  auto button_area = get_button_area(get_dimensions(), style);
  auto text_width = static_cast<float>(std::strlen(m_name)) * 6.f;
  auto text_x = button_area.m_x + (button_area.m_w - text_width) * 0.5f;

  if (text_x < button_area.m_x + style->m_padding)
    text_x = button_area.m_x + style->m_padding;

  draw_ptr->draw_rectangle(button_area, m_pressed ? style->m_accent : style->m_foreground);
  draw_ptr->draw_text(m_name, text_x, button_area.m_y, style->m_text);
}
