#include "ui_dropdown.h"

static int clamp_dropdown_index(int value, int item_count)
{
  if (item_count <= 0)
    return 0;

  if (value < 0)
    return 0;

  if (value >= item_count)
    return item_count - 1;

  return value;
}

static ui_dimension get_dropdown_button_area(ui_dimension dimension, std::shared_ptr<ui_style> style)
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

static ui_dimension get_dropdown_item_area(ui_dimension button_area, std::shared_ptr<ui_style> style, int index)
{
  return ui_dimension(
    button_area.m_x,
    button_area.m_y + (style->m_control_height * static_cast<float>(index + 1)),
    button_area.m_w,
    style->m_control_height
  );
}

ui_dropdown::ui_dropdown(const char* name, const char** items, int item_count, int* selected)
{
  m_name = name;
  m_items = items;
  m_item_count = item_count;
  m_selected = selected;
  m_open = false;
  m_pressed = false;
}

bool ui_dropdown::think(std::shared_ptr<ui_style> style_ptr)
{
  set_style(style_ptr);

  if (m_selected)
    *m_selected = clamp_dropdown_index(*m_selected, m_item_count);

  set_render_last(m_open);

  return true;
}

void ui_dropdown::input(ui_input& input)
{
  auto style = get_style();

  if (!style || !m_selected)
    return;

  auto button_area = get_dropdown_button_area(get_dimensions(), style);
  auto in_button = UI_IN_AREA(input.mouse, button_area);

  if (!input.mouse.buttons[ui_button_left])
  {
    m_pressed = false;
    return;
  }

  if (m_pressed)
  {
    if (m_open && in_button)
      input.handled = true;

    return;
  }

  m_pressed = true;

  if (in_button)
  {
    m_open = !m_open;
    set_render_last(m_open);
    input.handled = true;
    return;
  }

  if (m_open)
  {
    for (auto i = 0; i < m_item_count; i++)
    {
      auto item_area = get_dropdown_item_area(button_area, style, i);

      if (UI_IN_AREA(input.mouse, item_area))
      {
        *m_selected = i;
        m_open = false;
        set_render_last(false);
        input.handled = true;
        return;
      }
    }

    m_open = false;
    set_render_last(false);
    input.handled = true;
  }
}

void ui_dropdown::render(std::shared_ptr<ui_draw> draw_ptr)
{
  auto style = get_style();

  if (!style || !m_selected)
    return;

  auto dim = get_dimensions();
  auto button_area = get_dropdown_button_area(dim, style);
  auto selected = clamp_dropdown_index(*m_selected, m_item_count);
  auto selected_text = (m_item_count > 0 && m_items) ? m_items[selected] : "";

  draw_ptr->draw_text(m_name, dim.m_x, dim.m_y, style->m_text);
  draw_ptr->draw_rectangle(button_area, style->m_foreground);
  draw_ptr->draw_text(selected_text, button_area.m_x + style->m_padding, button_area.m_y, style->m_text);
  draw_ptr->draw_text(m_open ? "^" : "v", button_area.m_x + button_area.m_w - style->m_control_height, button_area.m_y, style->m_text);

  if (!m_open || m_item_count <= 0 || !m_items)
    return;

  for (auto i = 0; i < m_item_count; i++)
  {
    auto item_area = get_dropdown_item_area(button_area, style, i);
    auto color = (i == selected) ? style->m_accent : style->m_background;

    draw_ptr->draw_rectangle(item_area, color);
    draw_ptr->draw_text(m_items[i], item_area.m_x + style->m_padding, item_area.m_y, style->m_text);
  }
}
