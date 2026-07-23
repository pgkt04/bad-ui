#include "ui_color_picker.h"

#include <cmath>
#include <cstdio>

enum color_picker_drag_mode
{
  drag_none = -1,
  drag_square,
  drag_hue,
  drag_alpha
};

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

static ui_color hsv_to_rgb(float hue, float saturation, float value, float alpha)
{
  hue = clamp_normalized(hue);
  saturation = clamp_normalized(saturation);
  value = clamp_normalized(value);

  auto h = hue * 6.f;
  auto i = static_cast<int>(h);
  auto f = h - static_cast<float>(i);
  auto p = value * (1.f - saturation);
  auto q = value * (1.f - f * saturation);
  auto t = value * (1.f - (1.f - f) * saturation);
  auto r = value;
  auto g = t;
  auto b = p;

  switch (i % 6)
  {
  case 0:
    r = value;
    g = t;
    b = p;
    break;
  case 1:
    r = q;
    g = value;
    b = p;
    break;
  case 2:
    r = p;
    g = value;
    b = t;
    break;
  case 3:
    r = p;
    g = q;
    b = value;
    break;
  case 4:
    r = t;
    g = p;
    b = value;
    break;
  default:
    r = value;
    g = p;
    b = q;
    break;
  }

  return ui_color(r * 255.f, g * 255.f, b * 255.f, alpha);
}

static ui_color lerp_color(ui_color from, ui_color to, float amount)
{
  amount = clamp_normalized(amount);

  return ui_color(
    from.m_r + (to.m_r - from.m_r) * amount,
    from.m_g + (to.m_g - from.m_g) * amount,
    from.m_b + (to.m_b - from.m_b) * amount,
    from.m_a + (to.m_a - from.m_a) * amount
  );
}

static ui_color sv_square_color(ui_color hue_color, float saturation, float value)
{
  auto saturated = lerp_color(ui_color(255, 255, 255, 255), hue_color, saturation);

  return lerp_color(ui_color(0, 0, 0, 255), saturated, value);
}

static void rgb_to_hsv(ui_color color, float fallback_hue, float& hue, float& saturation, float& value)
{
  auto r = clamp_channel(color.m_r) / 255.f;
  auto g = clamp_channel(color.m_g) / 255.f;
  auto b = clamp_channel(color.m_b) / 255.f;
  auto max = r;
  auto min = r;

  if (g > max)
    max = g;
  if (b > max)
    max = b;
  if (g < min)
    min = g;
  if (b < min)
    min = b;

  auto delta = max - min;
  value = max;
  saturation = max <= 0.f ? 0.f : delta / max;

  if (delta <= 0.f)
  {
    hue = fallback_hue;
    return;
  }

  if (max == r)
    hue = (g - b) / delta;
  else if (max == g)
    hue = 2.f + (b - r) / delta;
  else
    hue = 4.f + (r - g) / delta;

  hue /= 6.f;

  if (hue < 0.f)
    hue += 1.f;
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

static ui_dimension get_square_area(ui_dimension button_area, std::shared_ptr<ui_style> style)
{
  auto size = button_area.m_w;

  if (size > 120.f)
    size = 120.f;

  if (size < 0.f)
    size = 0.f;

  return ui_dimension(
    button_area.m_x,
    button_area.m_y + style->m_control_height + style->m_padding,
    size,
    size
  );
}

static ui_dimension get_hue_area(ui_dimension square_area, std::shared_ptr<ui_style> style)
{
  return ui_dimension(
    square_area.m_x,
    square_area.m_y + square_area.m_h + style->m_padding,
    square_area.m_w,
    style->m_control_height
  );
}

static ui_dimension get_alpha_area(ui_dimension hue_area, std::shared_ptr<ui_style> style)
{
  return ui_dimension(
    hue_area.m_x,
    hue_area.m_y + hue_area.m_h + style->m_padding,
    hue_area.m_w,
    style->m_control_height
  );
}

static void update_color(ui_color* color, float hue, float saturation, float value)
{
  if (!color)
    return;

  auto alpha = clamp_channel(color->m_a);
  *color = hsv_to_rgb(hue, saturation, value, alpha);
}

static void update_square_from_mouse(ui_color* color, float& saturation, float& value, float hue, ui_input& input, ui_dimension square_area)
{
  if (square_area.m_w <= 0.f || square_area.m_h <= 0.f)
    return;

  saturation = clamp_normalized((input.mouse.pos_x - square_area.m_x) / square_area.m_w);
  value = 1.f - clamp_normalized((input.mouse.pos_y - square_area.m_y) / square_area.m_h);
  update_color(color, hue, saturation, value);
}

static void update_hue_from_mouse(ui_color* color, float& hue, float saturation, float value, ui_input& input, ui_dimension hue_area)
{
  if (hue_area.m_w <= 0.f)
    return;

  hue = clamp_normalized((input.mouse.pos_x - hue_area.m_x) / hue_area.m_w);
  update_color(color, hue, saturation, value);
}

static void update_alpha_from_mouse(ui_color* color, ui_input& input, ui_dimension alpha_area)
{
  if (!color || alpha_area.m_w <= 0.f)
    return;

  color->m_a = clamp_normalized((input.mouse.pos_x - alpha_area.m_x) / alpha_area.m_w) * 255.f;
}

static float gradient_position(float position, float size, float step)
{
  auto denominator = size - step;

  if (denominator <= 0.f)
    return 0.f;

  return clamp_normalized(position / denominator);
}

static float gradient_cell_size(float position, float size, float step)
{
  auto remaining = size - position;

  if (remaining <= 0.f)
    return 0.f;

  return remaining < step ? remaining : step;
}

static ui_dimension pixel_snap(ui_dimension dimension)
{
  auto x0 = std::floor(dimension.m_x);
  auto y0 = std::floor(dimension.m_y);
  auto x1 = std::ceil(dimension.m_x + dimension.m_w);
  auto y1 = std::ceil(dimension.m_y + dimension.m_h);

  return ui_dimension(x0, y0, x1 - x0, y1 - y0);
}

static void draw_rect_border(std::shared_ptr<ui_draw> draw_ptr, ui_dimension dimension, ui_color color)
{
  draw_ptr->draw_rectangle(ui_dimension(dimension.m_x - 1.f, dimension.m_y - 1.f, dimension.m_w + 2.f, 1.f), color);
  draw_ptr->draw_rectangle(ui_dimension(dimension.m_x - 1.f, dimension.m_y + dimension.m_h, dimension.m_w + 2.f, 1.f), color);
  draw_ptr->draw_rectangle(ui_dimension(dimension.m_x - 1.f, dimension.m_y, 1.f, dimension.m_h), color);
  draw_ptr->draw_rectangle(ui_dimension(dimension.m_x + dimension.m_w, dimension.m_y, 1.f, dimension.m_h), color);
}

// Composites a translucent color over a base color on the cpu, so alpha is
// visible on any backend, even ones that cannot alpha blend.
//
static ui_color blend_over(ui_color base, ui_color color)
{
  auto alpha = clamp_channel(color.m_a) / 255.f;

  return ui_color(
    base.m_r + (clamp_channel(color.m_r) - base.m_r) * alpha,
    base.m_g + (clamp_channel(color.m_g) - base.m_g) * alpha,
    base.m_b + (clamp_channel(color.m_b) - base.m_b) * alpha,
    255.f
  );
}

static ui_color checker_color(int index)
{
  return (index & 1) ? ui_color(120, 120, 120, 255) : ui_color(190, 190, 190, 255);
}

// Transparency checkerboard with the color composited over it.
//
static void draw_color_swatch(std::shared_ptr<ui_draw> draw_ptr, ui_dimension area, ui_color color)
{
  auto cell = area.m_h * 0.5f;

  if (cell <= 0.f)
    return;

  auto index = 0;

  for (auto x = 0.f; x < area.m_w; x += cell, index++)
  {
    auto cell_w = gradient_cell_size(x, area.m_w, cell);

    draw_ptr->draw_rectangle(ui_dimension(area.m_x + x, area.m_y, cell_w, cell),
      blend_over(checker_color(index), color));
    draw_ptr->draw_rectangle(ui_dimension(area.m_x + x, area.m_y + cell, cell_w, area.m_h - cell),
      blend_over(checker_color(index + 1), color));
  }
}

ui_color_picker::ui_color_picker(const char* name, ui_color* color)
{
  m_name = name;
  m_color = color;
  m_open = false;
  m_pressed = false;
  m_drag_mode = drag_none;
  m_hue = 0.f;
  m_saturation = 0.f;
  m_value = 0.f;
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

    if (m_drag_mode == drag_none)
      rgb_to_hsv(*m_color, m_hue, m_hue, m_saturation, m_value);
  }

  return true;
}

void ui_color_picker::input(ui_input& input)
{
  auto style = get_style();

  if (!style || !m_color)
    return;

  auto button_area = get_picker_button_area(get_dimensions(), style);
  auto square_area = get_square_area(button_area, style);
  auto hue_area = get_hue_area(square_area, style);
  auto alpha_area = get_alpha_area(hue_area, style);
  auto fresh_press = take_fresh_press(input);

  if (!input.mouse.buttons[ui_button_left])
  {
    m_pressed = false;
    m_drag_mode = drag_none;
    return;
  }

  if (m_drag_mode == drag_square)
  {
    update_square_from_mouse(m_color, m_saturation, m_value, m_hue, input, square_area);
    input.handled = true;
    return;
  }

  if (m_drag_mode == drag_hue)
  {
    update_hue_from_mouse(m_color, m_hue, m_saturation, m_value, input, hue_area);
    input.handled = true;
    return;
  }

  if (m_drag_mode == drag_alpha)
  {
    update_alpha_from_mouse(m_color, input, alpha_area);
    input.handled = true;
    return;
  }

  if (m_pressed)
  {
    input.handled = true;
    return;
  }

  // Only react to presses that started on this event; a button held down
  // since a previous event belongs to another control's interaction.
  //
  if (!fresh_press)
    return;

  if (UI_IN_AREA(input.mouse, button_area))
  {
    m_open = !m_open;
    m_pressed = true;
    input.handled = true;
    return;
  }

  if (!m_open)
    return;

  if (UI_IN_AREA(input.mouse, square_area))
  {
    m_drag_mode = drag_square;
    m_pressed = true;
    update_square_from_mouse(m_color, m_saturation, m_value, m_hue, input, square_area);
    input.handled = true;
    return;
  }

  if (UI_IN_AREA(input.mouse, hue_area))
  {
    m_drag_mode = drag_hue;
    m_pressed = true;
    update_hue_from_mouse(m_color, m_hue, m_saturation, m_value, input, hue_area);
    input.handled = true;
    return;
  }

  if (UI_IN_AREA(input.mouse, alpha_area))
  {
    m_drag_mode = drag_alpha;
    m_pressed = true;
    update_alpha_from_mouse(m_color, input, alpha_area);
    input.handled = true;
    return;
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
  std::snprintf(hex, sizeof(hex), "#%02X%02X%02X%02X", channel_int(m_color->m_r), channel_int(m_color->m_g), channel_int(m_color->m_b), channel_int(m_color->m_a));

  draw_ptr->draw_text(m_name, dim.m_x, dim.m_y, style->m_text);
  draw_ptr->draw_rectangle(button_area, style->m_foreground);
  draw_color_swatch(draw_ptr, swatch, *m_color);
  draw_ptr->draw_text(hex, swatch.m_x + swatch.m_w + style->m_padding, button_area.m_y, style->m_text);
  draw_ptr->draw_chevron(ui_dimension(button_area.m_x + button_area.m_w - style->m_control_height, button_area.m_y, style->m_control_height, button_area.m_h), m_open, style->m_text);

  if (!m_open)
    return;

  auto square_area = get_square_area(button_area, style);
  auto hue_area = get_hue_area(square_area, style);
  auto alpha_area = get_alpha_area(hue_area, style);
  auto step = 1.f;
  square_area = pixel_snap(square_area);
  hue_area = pixel_snap(hue_area);
  alpha_area = pixel_snap(alpha_area);
  auto popup_area = ui_dimension(
    square_area.m_x - 1.f,
    square_area.m_y - 1.f,
    square_area.m_w + 2.f,
    alpha_area.m_y + alpha_area.m_h - square_area.m_y + 2.f
  );

  draw_ptr->draw_rectangle(popup_area, style->m_background);
  draw_ptr->draw_rectangle(square_area, ui_color(0, 0, 0, 255));

  auto hue_color = hsv_to_rgb(m_hue, 1.f, 1.f, 255.f);
  for (auto y = 0.f; y < square_area.m_h; y += step)
  {
    for (auto x = 0.f; x < square_area.m_w; x += step)
    {
      auto saturation = gradient_position(x + step * 0.5f, square_area.m_w, step);
      auto value = 1.f - gradient_position(y + step * 0.5f, square_area.m_h, step);
      auto cell_w = gradient_cell_size(x, square_area.m_w, step);
      auto cell_h = gradient_cell_size(y, square_area.m_h, step);
      draw_ptr->draw_rectangle(ui_dimension(square_area.m_x + x, square_area.m_y + y, cell_w, cell_h), sv_square_color(hue_color, saturation, value));
    }
  }

  draw_rect_border(draw_ptr, square_area, style->m_text);

  auto marker_x = square_area.m_x + m_saturation * square_area.m_w;
  auto marker_y = square_area.m_y + (1.f - m_value) * square_area.m_h;
  draw_ptr->draw_line(marker_x - 4.f, marker_y, marker_x + 4.f, marker_y, style->m_text);
  draw_ptr->draw_line(marker_x, marker_y - 4.f, marker_x, marker_y + 4.f, style->m_text);

  for (auto x = 0.f; x < hue_area.m_w; x += step)
  {
    auto hue = gradient_position(x + step * 0.5f, hue_area.m_w, step);
    auto cell_w = gradient_cell_size(x, hue_area.m_w, step);
    draw_ptr->draw_rectangle(ui_dimension(hue_area.m_x + x, hue_area.m_y, cell_w, hue_area.m_h), hsv_to_rgb(hue, 1.f, 1.f, 255.f));
  }

  auto hue_x = hue_area.m_x + m_hue * hue_area.m_w;
  draw_ptr->draw_line(hue_x, hue_area.m_y, hue_x, hue_area.m_y + hue_area.m_h, style->m_text);

  // Alpha bar: current color fading from transparent to opaque over a
  // checkerboard, composited on the cpu so it reflects on any backend.
  //
  auto checker = alpha_area.m_h * 0.5f;
  for (auto x = 0.f; x < alpha_area.m_w; x += step)
  {
    auto alpha = gradient_position(x + step * 0.5f, alpha_area.m_w, step) * 255.f;
    auto cell_w = gradient_cell_size(x, alpha_area.m_w, step);
    auto rgba = ui_color(m_color->m_r, m_color->m_g, m_color->m_b, alpha);
    auto index = checker > 0.f ? static_cast<int>(x / checker) : 0;

    draw_ptr->draw_rectangle(ui_dimension(alpha_area.m_x + x, alpha_area.m_y, cell_w, checker),
      blend_over(checker_color(index), rgba));
    draw_ptr->draw_rectangle(ui_dimension(alpha_area.m_x + x, alpha_area.m_y + checker, cell_w, alpha_area.m_h - checker),
      blend_over(checker_color(index + 1), rgba));
  }

  auto alpha_x = alpha_area.m_x + (clamp_channel(m_color->m_a) / 255.f) * alpha_area.m_w;
  draw_ptr->draw_line(alpha_x, alpha_area.m_y, alpha_x, alpha_area.m_y + alpha_area.m_h, style->m_text);
}

float ui_color_picker::get_min_width(std::shared_ptr<ui_style> style)
{
  // Label column, swatch, hex text (#RRGGBBAA at ~8px per character) and the
  // open/close arrow. Mirrors the fixed offsets used in render.
  auto label_width = 100.f;
  auto swatch = style->m_padding + style->m_control_height * 2.f;
  auto hex = style->m_padding + 9.f * 8.f;
  auto arrow = style->m_control_height + style->m_padding;

  return label_width + swatch + hex + arrow;
}
