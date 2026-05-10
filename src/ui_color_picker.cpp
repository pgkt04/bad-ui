#include "ui_color_picker.h"

#include <cstdio>

static float clamp_channel(float value)
{
  if (value < 0.f)
    return 0.f;

  if (value > 255.f)
    return 255.f;

  return value;
}

static float clamp_normalized(float value)
{
  if (value < 0.f)
    return 0.f;

  if (value > 1.f)
    return 1.f;

  return value;
}

static int channel_int(float value)
{
  return static_cast<int>(clamp_channel(value));
}

static float get_channel(ui_color color, int channel)
{
  switch (channel)
  {
  case 0:
    return color.m_r;
  case 1:
    return color.m_g;
  case 2:
    return color.m_b;
  default:
    return color.m_a;
  }
}

static void set_channel(ui_color* color, int channel, float value)
{
  if (!color)
    return;

  value = clamp_channel(value);

  switch (channel)
  {
  case 0:
    color->m_r = value;
    break;
  case 1:
    color->m_g = value;
    break;
  case 2:
    color->m_b = value;
    break;
  default:
    color->m_a = value;
    break;
  }
}

static ui_color channel_color(int channel)
{
  switch (channel)
  {
  case 0:
    return ui_color(255, 0, 0, 255);
  case 1:
    return ui_color(0, 255, 0, 255);
  case 2:
    return ui_color(0, 0, 255, 255);
  default:
    return ui_color(255, 255, 255, 255);
  }
}

static ui_dimension get_picker_button_area(ui_dimension dimension, std::shared_ptr<ui_style> style)
{
  auto label_width = 100.f;
  auto right_padding = style->m_padding;

  if (dimension.m_w <= label_width + right_padding)
    label_width = 0.f;

  return ui_dimension(
    dimension.m_x + label_width,
    dimension.m_y,
    dimension.m_w - label_width - right_padding,
    style->m_control_height
  );
}

static ui_dimension get_picker_row_area(ui_dimension button_area, std::shared_ptr<ui_style> style, int row)
{
  return ui_dimension(
    button_area.m_x,
    button_area.m_y + style->m_control_height * static_cast<float>(row + 1),
    button_area.m_w,
    style->m_control_height
  );
}

static ui_dimension get_picker_bar_area(ui_dimension row_area, std::shared_ptr<ui_style> style)
{
  auto label_width = 36.f;
  auto width = row_area.m_w - label_width - style->m_padding;

  if (width < 0.f)
    width = 0.f;

  return ui_dimension(
    row_area.m_x + label_width,
    row_area.m_y + 2.f,
    width,
    row_area.m_h - 4.f
  );
}

static void update_channel_from_mouse(ui_color* color, int channel, ui_input& input, ui_dimension bar_area)
{
  if (bar_area.m_w <= 0.f)
    return;

  auto normalized = clamp_normalized((input.mouse.pos_x - bar_area.m_x) / bar_area.m_w);
  set_channel(color, channel, normalized * 255.f);
}

ui_color_picker::ui_color_picker(const char* name, ui_color* color)
{
  m_name = name;
  m_color = color;
  m_open = false;
  m_pressed = false;
  m_drag_channel = -1;
  set_render_last(true);
}

bool ui_color_picker::think(std::shared_ptr<ui_style> style_ptr)
{
  set_style(style_ptr);

  if (m_color)
  {
    m_color->m_r = clamp_channel(m_color->m_r);
    m_color->m_g = clamp_channel(m_color->m_g);
    m_color->m_b = clamp_channel(m_color->m_b);
    m_color->m_a = clamp_channel(m_color->m_a);
  }

  return true;
}

void ui_color_picker::input(ui_input& input)
{
  auto style = get_style();

  if (!style || !m_color)
    return;

  auto button_area = get_picker_button_area(get_dimensions(), style);

  if (!input.mouse.buttons[ui_button_left])
  {
    m_pressed = false;
    m_drag_channel = -1;
    return;
  }

  if (m_drag_channel >= 0)
  {
    auto row_area = get_picker_row_area(button_area, style, m_drag_channel + 1);
    auto bar_area = get_picker_bar_area(row_area, style);
    update_channel_from_mouse(m_color, m_drag_channel, input, bar_area);
    input.handled = true;
    return;
  }

  if (m_pressed)
  {
    input.handled = true;
    return;
  }

  if (UI_IN_AREA(input.mouse, button_area))
  {
    m_open = !m_open;
    m_pressed = true;
    input.handled = true;
    return;
  }

  if (!m_open)
    return;

  for (auto channel = 0; channel < 4; channel++)
  {
    auto row_area = get_picker_row_area(button_area, style, channel + 1);
    auto bar_area = get_picker_bar_area(row_area, style);

    if (UI_IN_AREA(input.mouse, bar_area))
    {
      m_drag_channel = channel;
      m_pressed = true;
      update_channel_from_mouse(m_color, channel, input, bar_area);
      input.handled = true;
      return;
    }
  }

  m_open = false;
  m_pressed = true;
  input.handled = true;
}

void ui_color_picker::render(std::shared_ptr<ui_draw> draw_ptr)
{
  auto style = get_style();

  if (!style || !m_color)
    return;

  auto dim = get_dimensions();
  auto button_area = get_picker_button_area(dim, style);
  auto swatch = ui_dimension(button_area.m_x + style->m_padding, button_area.m_y + 2.f, style->m_control_height * 2.f, button_area.m_h - 4.f);
  char hex[16];
  std::snprintf(hex, sizeof(hex), "#%02X%02X%02X", channel_int(m_color->m_r), channel_int(m_color->m_g), channel_int(m_color->m_b));

  draw_ptr->draw_text(m_name, dim.m_x, dim.m_y, style->m_text);
  draw_ptr->draw_rectangle(button_area, style->m_foreground);
  draw_ptr->draw_rectangle(swatch, *m_color);
  draw_ptr->draw_text(hex, swatch.m_x + swatch.m_w + style->m_padding, button_area.m_y, style->m_text);
  draw_ptr->draw_text(m_open ? "^" : "v", button_area.m_x + button_area.m_w - style->m_control_height, button_area.m_y, style->m_text);

  if (!m_open)
    return;

  auto preview_area = get_picker_row_area(button_area, style, 0);
  auto preview_swatch = preview_area;
  preview_swatch.m_x += style->m_padding;
  preview_swatch.m_y += 2.f;
  preview_swatch.m_w -= style->m_padding * 2.f;
  preview_swatch.m_h -= 4.f;

  draw_ptr->draw_rectangle(preview_area, style->m_background);
  draw_ptr->draw_rectangle(preview_swatch, *m_color);

  const char* labels[] = { "R", "G", "B", "A" };

  for (auto channel = 0; channel < 4; channel++)
  {
    auto row_area = get_picker_row_area(button_area, style, channel + 1);
    auto bar_area = get_picker_bar_area(row_area, style);
    auto value = clamp_channel(get_channel(*m_color, channel));
    auto fill_area = bar_area;
    char value_text[16];
    std::snprintf(value_text, sizeof(value_text), "%s %d", labels[channel], channel_int(value));
    fill_area.m_w *= value / 255.f;

    draw_ptr->draw_rectangle(row_area, style->m_background);
    draw_ptr->draw_text(value_text, row_area.m_x + style->m_padding, row_area.m_y, style->m_text);
    draw_ptr->draw_rectangle(bar_area, style->m_foreground);

    if (fill_area.m_w > 0.f)
      draw_ptr->draw_rectangle(fill_area, channel_color(channel));
  }
}
