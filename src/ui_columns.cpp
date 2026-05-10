#include "ui_columns.h"
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

static void draw_rounded_rect(std::shared_ptr<ui_draw> draw_ptr, ui_dimension dimension, ui_color color, float radius)
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
    else if (bottom_distance < radius)
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

ui_column::ui_column(bool visible)
{
  set_dynamic(true);
  set_is_col(true);

  m_visible = visible;
}

void ui_column::push(std::shared_ptr<ui_object> object)
{
  add_child(object);
}

void ui_column::input(ui_input& input)
{
  set_input(input);
  input_children(input, get_scroll_enabled());
}

bool ui_column::think(std::shared_ptr<ui_style> style_ptr)
{
  set_style(style_ptr);

  auto outer = get_dimensions();
  auto layout = outer;

  if (m_visible)
  {
    layout.m_x += 1.f;
    layout.m_y += 1.f;
    layout.m_w -= 2.f;
    layout.m_h -= 2.f;

    if (layout.m_w < 0.f)
      layout.m_w = 0.f;

    if (layout.m_h < 0.f)
      layout.m_h = 0.f;
  }
  else
  {
    // Hidden columns come from hidden groups and should not create their own
    // top inset; they are just layout rails for the visible child groups.
    layout.m_y -= style_ptr->m_padding;
    layout.m_h += style_ptr->m_padding;
  }

  set_dimensions(layout);
  handle_relocations(style_ptr);
  set_dimensions(outer);

  for (auto child : get_children())
    child->think(style_ptr);

  return true;
}

void ui_column::render(std::shared_ptr<ui_draw> draw_ptr)
{
  // draw the outline then render all its children
  //
  if (m_visible)
  {
    auto style = get_style();
    auto radius = style->m_group_rounding_enabled ?
      rounded_rect_radius(get_dimensions(), style->m_group_rounding) : 0.f;

    draw_rounded_rect(draw_ptr, get_dimensions(), style->m_text, radius);
    auto inner = get_dimensions();
    inner.m_x += 1;
    inner.m_y += 1;
    inner.m_w -= 2;
    inner.m_h -= 2;

    if (radius > 1.f)
      radius -= 1.f;

    draw_rounded_rect(draw_ptr, inner, style->m_background, radius);
  }

  render_children(draw_ptr, get_scroll_enabled());
}
