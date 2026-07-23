#include "ui_tab.h"

static float clamp_tab_channel(float value)
{
  if (value < 0.f)
    return 0.f;

  if (value > 255.f)
    return 255.f;

  return value;
}

static ui_color selected_tab_color(ui_color color)
{
  return ui_color(
    clamp_tab_channel(color.m_r + 35.f),
    clamp_tab_channel(color.m_g + 35.f),
    clamp_tab_channel(color.m_b + 35.f),
    color.m_a
  );
}

static float rounded_tab_text_offset(ui_dimension dimensions, ui_dimension parent_dimensions, std::shared_ptr<ui_style> style)
{
  if (!style || !style->m_window_rounding_enabled)
    return 0.f;

  if (dimensions.m_x > parent_dimensions.m_x + 0.5f || dimensions.m_y > parent_dimensions.m_y + style->m_window_title_height + 0.5f)
    return 0.f;

  auto max_radius = parent_dimensions.m_w < parent_dimensions.m_h ? parent_dimensions.m_w * 0.5f : parent_dimensions.m_h * 0.5f;
  return max_radius * style->m_window_rounding * 0.5f;
}

ui_tab::ui_tab(const char* name)
{
  set_dynamic(true);
  set_is_tab(true);
  // Tabs scroll their content by default when it no longer fits; the
  // scrollbar only shows up while there is actual overflow.
  set_scroll_enabled(true);

  m_name = name;
  m_button_pressed = false;
  m_last_tab = -1;
}

void ui_tab::push(std::shared_ptr<ui_object> object)
{
  add_child(object);
}

bool ui_tab::think(std::shared_ptr<ui_style> style_ptr)
{
  set_style(style_ptr);
  handle_relocations(style_ptr);

  for (auto child : get_children())
    child->think(style_ptr);

  return true;
}

void ui_tab::input(ui_input& input)
{
  if (!get_style())
    return;

  /// if the area is pressed
  auto tab_area = get_dimensions();
  tab_area.m_h = get_style()->m_control_height;
  auto fresh_press = take_fresh_press(input);

  if (input.mouse.buttons[ui_button_left])
  {
    if (fresh_press && UI_IN_AREA(input.mouse, tab_area) && !m_button_pressed)
    {
      m_button_pressed = true;
    }
  }
  else if (m_button_pressed)
  {
    set_selected(true);

    m_button_pressed = false;
  }

  if (get_selected())
    input_children(input);
}

void ui_tab::render(std::shared_ptr<ui_draw> draw_ptr)
{
  /// Draw tab
  auto dimensions = get_dimensions();
  auto style = get_style();

  /// background
  /// draw_ptr->draw_rectangle(ui_dimension(dimensions.m_x, dimensions.m_y, dimensions.m_w, dimensions.m_h), style->m_text);

  auto tab_area = ui_dimension(dimensions.m_x, dimensions.m_y, dimensions.m_w, style->m_control_height);
  auto color = get_selected() ? selected_tab_color(style->m_foreground) : style->m_foreground;

  draw_ptr->draw_rectangle(tab_area, color);
  auto text_x = dimensions.m_x + style->m_padding + rounded_tab_text_offset(dimensions, get_parent_dimensions(), style);
  draw_ptr->draw_text(m_name, text_x, dimensions.m_y, style->m_text);

  /// Render all children
  if (get_selected())
    render_children(draw_ptr);
}
