#include "ui_slider.h"

#include <cstdio>

static float clamp_slider_value(float value)
{
  if (value < 0.f)
    return 0.f;

  if (value > 1.f)
    return 1.f;

  return value;
}

static ui_dimension get_slider_area(ui_dimension dimension, std::shared_ptr<ui_style> style)
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

ui_slider::ui_slider(const char* name, float* value, bool show_value)
{
  m_name = name;
  m_value = value;
  m_show_value = show_value;
}

bool ui_slider::think(std::shared_ptr<ui_style> style_ptr)
{
  set_style(style_ptr);

  return true;
}

void ui_slider::input(ui_input& input)
{
  if (!get_style())
    return;

  auto slider_area = get_slider_area(get_dimensions(), get_style());

  if (!input.mouse.buttons[ui_button_left])
  {
    set_selected(false);
    return;
  }

  if (UI_IN_AREA(input.mouse, slider_area))
    set_selected(true);

  if (get_selected() && slider_area.m_w > 0.f)
  {
    auto value = (input.mouse.pos_x - slider_area.m_x) / slider_area.m_w;
    *m_value = clamp_slider_value(value);
  }

}

void ui_slider::render(std::shared_ptr<ui_draw> draw_ptr)
{
  auto style = get_style();

  if (!style)
    return;

  auto dim = get_dimensions();
  auto slider_area = get_slider_area(dim, style);
  auto value = clamp_slider_value(*m_value);

  // Skinny track centered in the row; the handle keeps the full control
  // height. The input hit area stays the full row so it is easy to grab.
  //
  auto track = slider_area;
  track.m_h = slider_area.m_h / 3.f;
  track.m_y = slider_area.m_y + (slider_area.m_h - track.m_h) * 0.5f;

  auto fill_area = track;
  fill_area.m_w *= value;

  draw_ptr->draw_rectangle(track, style->m_foreground);

  if (fill_area.m_w > 0.f)
    draw_ptr->draw_rectangle(fill_area, style->m_accent);

  auto handle_width = 4.f;
  auto handle_x = slider_area.m_x + ((slider_area.m_w - handle_width) * value);
  auto handle = ui_dimension(handle_x, slider_area.m_y, handle_width, slider_area.m_h);
  draw_ptr->draw_rectangle(handle, style->m_text);

  // Value rides along with the label instead of sitting on the slider body.
  //
  if (m_show_value)
  {
    char label[64];
    std::snprintf(label, sizeof(label), "%s %.2f", m_name, value);
    draw_ptr->draw_text(label, dim.m_x, dim.m_y, style->m_text);
  }
  else
  {
    draw_ptr->draw_text(m_name, dim.m_x, dim.m_y, style->m_text);
  }
}
