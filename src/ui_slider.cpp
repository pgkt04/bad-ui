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

// Width reserved right of the track for the value text. ui_draw has no text
// metrics, so this is a fixed slot sized for "0.00" that scales with the
// style instead of a measured width.
//
static float get_value_slot_width(std::shared_ptr<ui_style> style)
{
  return style->m_control_height * 2.f;
}

static ui_dimension get_slider_area(ui_dimension dimension, std::shared_ptr<ui_style> style, bool show_value)
{
  auto label_width = 100.f;
  auto right_padding = style->m_padding;

  if (show_value)
    right_padding += get_value_slot_width(style) + style->m_padding;

  if (dimension.m_w <= label_width + right_padding)
    label_width = 0.f;

  auto width = dimension.m_w - label_width - right_padding;

  if (width < 0.f)
    width = 0.f;

  return ui_dimension(
    dimension.m_x + label_width,
    dimension.m_y,
    width,
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

  auto slider_area = get_slider_area(get_dimensions(), get_style(), m_show_value);
  auto fresh_press = take_fresh_press(input);

  if (!input.mouse.buttons[ui_button_left])
  {
    set_selected(false);
    return;
  }

  if (!get_selected() && fresh_press && UI_IN_AREA(input.mouse, slider_area))
    set_selected(true);

  if (get_selected())
  {
    if (slider_area.m_w > 0.f)
      *m_value = clamp_slider_value((input.mouse.pos_x - slider_area.m_x) / slider_area.m_w);

    // Keep the drag captured so no other control reacts until release.
    //
    input.handled = true;
  }
}

void ui_slider::render(std::shared_ptr<ui_draw> draw_ptr)
{
  auto style = get_style();

  if (!style)
    return;

  auto dim = get_dimensions();
  auto slider_area = get_slider_area(dim, style, m_show_value);
  auto value = clamp_slider_value(*m_value);

  // Line style track: thin dim line for the remaining range, thicker accent
  // line for the filled range, and a vertical tick as the handle. The input
  // hit area stays the full row height so the track is easy to grab.
  //
  auto center_y = slider_area.m_y + slider_area.m_h * 0.5f;
  auto fill_w = slider_area.m_w * value;

  if (fill_w < slider_area.m_w)
    draw_ptr->draw_rectangle(ui_dimension(slider_area.m_x + fill_w, center_y - 1.f, slider_area.m_w - fill_w, 2.f), style->m_foreground);

  if (fill_w > 0.f)
    draw_ptr->draw_rectangle(ui_dimension(slider_area.m_x, center_y - 2.f, fill_w, 4.f), style->m_accent);

  // Tick handle rides the end of the fill; kept inside the track so it never
  // clips outside the control at 0 or 1.
  //
  auto tick_w = 4.f;
  auto tick_h = slider_area.m_h * 0.8f;
  auto tick_x = slider_area.m_x + (slider_area.m_w - tick_w) * value;
  draw_ptr->draw_rectangle(ui_dimension(tick_x, center_y - tick_h * 0.5f, tick_w, tick_h), style->m_accent);

  draw_ptr->draw_text(m_name, dim.m_x, dim.m_y, style->m_text);

  if (m_show_value)
  {
    char label[16];
    std::snprintf(label, sizeof(label), "%.2f", value);
    draw_ptr->draw_text(label, slider_area.m_x + slider_area.m_w + style->m_padding, dim.m_y, style->m_text);
  }
}
