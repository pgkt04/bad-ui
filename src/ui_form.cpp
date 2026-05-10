#include "ui_form.h"
#include <cmath>

static float clamp_rounding(float value)
{
  if (value < 0.f)
    return 0.f;

  if (value > 1.f)
    return 1.f;

  return value;
}

static float rounded_rect_radius(ui_dimension dimension, float rounding)
{
  auto max_radius = dimension.m_w < dimension.m_h ? dimension.m_w * 0.5f : dimension.m_h * 0.5f;

  return max_radius * clamp_rounding(rounding);
}

static float max_float(float a, float b)
{
  return a > b ? a : b;
}

static ui_dimension get_resize_grip_area(ui_dimension dimension, std::shared_ptr<ui_style> style)
{
  auto size = style->m_control_height;

  if (size < 12.f)
    size = 12.f;

  return ui_dimension(
    dimension.m_x + dimension.m_w - size,
    dimension.m_y + dimension.m_h - size,
    size,
    size
  );
}

static void draw_rounded_rect_with_radius(std::shared_ptr<ui_draw> draw_ptr, ui_dimension dimension, ui_color color, float radius, bool top_only)
{
  if (radius <= 0.f)
  {
    draw_ptr->draw_rectangle(dimension, color);
    return;
  }

  auto y0 = static_cast<int>(std::floor(dimension.m_y));
  auto y1 = static_cast<int>(std::ceil(dimension.m_y + dimension.m_h));

  for (auto y = y0; y < y1; y++)
  {
    auto fy = static_cast<float>(y) + 0.5f;
    auto inset = 0.f;
    auto top_distance = fy - dimension.m_y;
    auto bottom_distance = dimension.m_y + dimension.m_h - fy;

    if (top_distance < radius)
    {
      auto dy = radius - top_distance;
      inset = radius - std::sqrt(radius * radius - dy * dy);
    }
    else if (!top_only && bottom_distance < radius)
    {
      auto dy = radius - bottom_distance;
      inset = radius - std::sqrt(radius * radius - dy * dy);
    }

    auto x = dimension.m_x + inset;
    auto w = dimension.m_w - inset * 2.f;

    if (w > 0.f)
      draw_ptr->draw_rectangle(ui_dimension(x, static_cast<float>(y), w, 1.f), color);
  }
}

static void draw_cursor(std::shared_ptr<ui_draw> draw_ptr, float x, float y, ui_color color)
{
  auto shadow = ui_color(0, 0, 0, 180);

  draw_ptr->draw_line(x + 1.f, y + 1.f, x + 1.f, y + 17.f, shadow);
  draw_ptr->draw_line(x + 1.f, y + 1.f, x + 12.f, y + 12.f, shadow);
  draw_ptr->draw_line(x + 1.f, y + 17.f, x + 5.f, y + 13.f, shadow);
  draw_ptr->draw_line(x + 5.f, y + 13.f, x + 8.f, y + 20.f, shadow);
  draw_ptr->draw_line(x + 8.f, y + 20.f, x + 10.f, y + 19.f, shadow);
  draw_ptr->draw_line(x + 10.f, y + 19.f, x + 7.f, y + 12.f, shadow);
  draw_ptr->draw_line(x + 7.f, y + 12.f, x + 12.f, y + 12.f, shadow);

  draw_ptr->draw_line(x, y, x, y + 16.f, color);
  draw_ptr->draw_line(x, y, x + 11.f, y + 11.f, color);
  draw_ptr->draw_line(x, y + 16.f, x + 4.f, y + 12.f, color);
  draw_ptr->draw_line(x + 4.f, y + 12.f, x + 7.f, y + 19.f, color);
  draw_ptr->draw_line(x + 7.f, y + 19.f, x + 9.f, y + 18.f, color);
  draw_ptr->draw_line(x + 9.f, y + 18.f, x + 6.f, y + 11.f, color);
  draw_ptr->draw_line(x + 6.f, y + 11.f, x + 11.f, y + 11.f, color);
}

static void draw_resize_grip(std::shared_ptr<ui_draw> draw_ptr, ui_dimension grip, ui_color foreground)
{
  draw_ptr->draw_line(grip.m_x + grip.m_w - 3.f, grip.m_y + 3.f, grip.m_x + 3.f, grip.m_y + grip.m_h - 3.f, foreground);
  draw_ptr->draw_line(grip.m_x + grip.m_w - 3.f, grip.m_y + 7.f, grip.m_x + 7.f, grip.m_y + grip.m_h - 3.f, foreground);
  draw_ptr->draw_line(grip.m_x + grip.m_w - 3.f, grip.m_y + 11.f, grip.m_x + 11.f, grip.m_y + grip.m_h - 3.f, foreground);
}

// parent class for all controls
// tab_settings - 0 (none), 1 (top), 2 (bottom), 3 (left), 4 (right)
//
ui_form::ui_form(ui_dimension dimensions, const char* title, int tab_setting, bool depth_enable)
{
  set_dimensions(dimensions);
  m_title = title;
  m_tab_setting = tab_setting;
  m_depth_enable = depth_enable;
  m_mouse = {};
  m_last_mouse = {};
  m_resizing = false;
  m_last_tab = -1;
}

ui_form::~ui_form()
{
}

void ui_form::push(std::shared_ptr<ui_object> object)
{
  add_child(object);
}

bool ui_form::think(std::shared_ptr<ui_style> style_ptr)
{
  set_style(style_ptr);

  m_last_mouse.pos_x = m_mouse.pos_x;
  m_last_mouse.pos_y = m_mouse.pos_y;
  m_mouse.pos_x = get_input().mouse.pos_x;
  m_mouse.pos_y = get_input().mouse.pos_y;

  if (m_resizing)
  {
    auto dimensions = get_dimensions();
    auto delta_x = (m_mouse.pos_x - m_last_mouse.pos_x);
    auto delta_y = (m_mouse.pos_y - m_last_mouse.pos_y);
    dimensions.m_w = max_float(dimensions.m_w + delta_x, 220.f);
    dimensions.m_h = max_float(dimensions.m_h + delta_y, 160.f);

    if (std::abs(delta_x) > 1.f || std::abs(delta_y) > 1.f)
      set_dimensions(dimensions);
  }
  // apply dragging
  else if (get_selected())
  {
    auto dimensions = get_dimensions();
    auto delta_x = (m_mouse.pos_x - m_last_mouse.pos_x);
    auto delta_y = (m_mouse.pos_y - m_last_mouse.pos_y);
    dimensions.m_x += delta_x;
    dimensions.m_y += delta_y;

    if (std::abs(delta_x) > 1.f || std::abs(delta_y) > 1.f)
      set_dimensions(dimensions);
  }

  handle_relocations(style_ptr);

  for (auto child : get_children())
  {
    child->set_style(style_ptr);
    child->think(style_ptr);
  }

  return true;
}

void ui_form::input(ui_input& input)
{
  set_input(input);

  /// drag handling
  auto style = get_style();

  if (style)
  {
    auto draggable_area = get_dimensions();
    draggable_area.m_h = style->m_window_title_height;
    auto grip_area = get_resize_grip_area(get_dimensions(), style);

    if (!input.mouse.buttons[ui_button_left])
    {
      set_selected(false);
      m_resizing = false;
    }
    else if (style->m_window_resize_enabled && m_resizing)
    {
      input.handled = true;
      return;
    }
    else if (style->m_window_resize_enabled && UI_IN_AREA(input.mouse, grip_area))
    {
      m_resizing = true;
      set_selected(false);
      input.handled = true;
      return;
    }

    else if (UI_IN_AREA(input.mouse, draggable_area))
      set_selected(true);
  }

  input_children(input);
}

void ui_form::render(std::shared_ptr<ui_draw> draw_ptr)
{
  auto style = get_style();
  auto dimensions = get_dimensions();
  auto window_radius = rounded_rect_radius(dimensions, style->m_window_rounding);

  // background
  if (style->m_window_rounding_enabled)
    draw_rounded_rect_with_radius(draw_ptr, dimensions, style->m_background, window_radius, false);
  else
    draw_ptr->draw_rectangle(dimensions, style->m_background);

  // text area
  auto text_dimension = dimensions;
  text_dimension.m_h = style->m_window_title_height;

  if (style->m_window_rounding_enabled)
    draw_rounded_rect_with_radius(draw_ptr, text_dimension, style->m_foreground, window_radius, true);
  else
    draw_ptr->draw_rectangle(text_dimension, style->m_foreground);

  auto title_x = dimensions.m_x;

  if (style->m_window_rounding_enabled)
    title_x += style->m_padding + window_radius * 0.5f;

  draw_ptr->draw_text(m_title, title_x, dimensions.m_y, style->m_accent);

  // render all children

  for (auto child : get_children())
  {
    if (!child->get_render_last())
      child->render(draw_ptr);
  }

  for (auto child : get_children())
  {
    if (child->get_render_last())
      child->render(draw_ptr);
  }

  if (style->m_window_resize_enabled)
    draw_resize_grip(draw_ptr, get_resize_grip_area(dimensions, style), style->m_accent);

  // render cursor
  draw_cursor(draw_ptr, m_mouse.pos_x, m_mouse.pos_y, style->m_accent);
}
