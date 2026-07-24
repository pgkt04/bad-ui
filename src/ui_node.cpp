#include "ui_node.h"

ui_node::ui_node(const char* title, float local_x, float local_y, float width)
{
  m_title = title;
  m_local_x = local_x;
  m_local_y = local_y;
  m_width = width;
  m_dragging = false;
  m_drag_dx = 0.f;
  m_drag_dy = 0.f;
}

void ui_node::push(std::shared_ptr<ui_object> object)
{
  add_child(object);
}

int ui_node::add_input(int row)
{
  m_ports.push_back(ui_node_port{ true, row });
  return static_cast<int>(m_ports.size()) - 1;
}

int ui_node::add_output(int row)
{
  m_ports.push_back(ui_node_port{ false, row });
  return static_cast<int>(m_ports.size()) - 1;
}

int ui_node::get_port_count() const
{
  return static_cast<int>(m_ports.size());
}

ui_node_port ui_node::get_port(int index) const
{
  if (index < 0 || index >= static_cast<int>(m_ports.size()))
    return ui_node_port{ true, 0 };

  return m_ports[static_cast<size_t>(index)];
}

// Top of the i-th content row; think() and the port anchors share this so
// ports always line up with their rows.
//
static float get_row_y(ui_dimension dimension, std::shared_ptr<ui_style> style, int row)
{
  return dimension.m_y + style->m_control_height + style->m_padding
    + (style->m_control_height + style->m_padding) * static_cast<float>(row);
}

void ui_node::get_port_pos(int index, float& out_x, float& out_y)
{
  auto style = get_style();

  out_x = 0.f;
  out_y = 0.f;

  // Style is unset until the first think has run; canvas queries may race
  // that on freshly pushed nodes.
  //
  if (!style || index < 0 || index >= static_cast<int>(m_ports.size()))
    return;

  auto dim = get_dimensions();
  auto port = m_ports[static_cast<size_t>(index)];

  out_x = port.m_is_input ? dim.m_x : dim.m_x + dim.m_w;
  out_y = get_row_y(dim, style, port.m_row) + style->m_control_height * 0.5f;
}

static ui_dimension get_title_area(ui_dimension dimension, std::shared_ptr<ui_style> style)
{
  return ui_dimension(dimension.m_x, dimension.m_y, dimension.m_w, style->m_control_height);
}

void ui_node::input(ui_input& input)
{
  auto style = get_style();

  if (!style)
    return;

  set_input(input);

  auto fresh_press = take_fresh_press(input);
  auto dim = get_dimensions();

  if (!input.mouse.buttons[ui_button_left])
    m_dragging = false;

  if (m_dragging)
  {
    // Keep the title drag captured until release; local position follows the
    // mouse relative to the canvas origin stored in parent dimensions.
    //
    auto origin = get_parent_dimensions();
    m_local_x = input.mouse.pos_x - origin.m_x - m_drag_dx;
    m_local_y = input.mouse.pos_y - origin.m_y - m_drag_dy;
    input.handled = true;
    return;
  }

  input_children(input, false);

  if (input.handled)
    return;

  if (fresh_press && UI_IN_AREA(input.mouse, get_title_area(dim, style)))
  {
    m_dragging = true;
    m_drag_dx = input.mouse.pos_x - dim.m_x;
    m_drag_dy = input.mouse.pos_y - dim.m_y;
    input.handled = true;
    return;
  }

  // Swallow presses anywhere on the node body so they cannot fall through to
  // the canvas below (pan or nodes underneath).
  //
  if (input.mouse.buttons[ui_button_left] && UI_IN_AREA(input.mouse, dim))
    input.handled = true;
}

bool ui_node::think(std::shared_ptr<ui_style> style_ptr)
{
  set_style(style_ptr);

  auto origin = get_parent_dimensions();
  auto children = get_children();
  // The per-row pitch already carries one padding after the last row, which
  // becomes the bottom inset and matches the title-to-row gap.
  //
  auto height = style_ptr->m_control_height + style_ptr->m_padding
    + (style_ptr->m_control_height + style_ptr->m_padding) * static_cast<float>(children.size());

  auto dim = ui_dimension(origin.m_x + m_local_x, origin.m_y + m_local_y, m_width, height);
  set_dimensions(dim);

  // Standard rows under the title bar: rows run to the node's right edge and
  // each control trims one padding off its right side itself.
  //
  auto row = 0;

  for (auto child : children)
  {
    child->set_parent_dimensions(dim);
    child->set_dimensions(ui_dimension(dim.m_x + style_ptr->m_padding,
      get_row_y(dim, style_ptr, row), dim.m_w - style_ptr->m_padding,
      style_ptr->m_control_height));
    child->think(style_ptr);
    row += 1;
  }

  return true;
}

void ui_node::render(std::shared_ptr<ui_draw> draw_ptr)
{
  auto style = get_style();

  if (!style)
    return;

  auto dim = get_dimensions();
  auto title = get_title_area(dim, style);

  // Opaque background first (hides grid/wires), then a translucent foreground
  // wash so the body reads slightly lighter than the canvas behind it.
  //
  draw_ptr->draw_rectangle(dim, style->m_background);
  draw_ptr->draw_rectangle(dim, ui_color(style->m_foreground.m_r,
    style->m_foreground.m_g, style->m_foreground.m_b, 40.f));
  draw_ptr->draw_rectangle(title, style->m_foreground);
  draw_ptr->draw_text(m_title, title.m_x + style->m_padding, title.m_y, style->m_text);

  render_children(draw_ptr, false);

  // Ports last so they sit on top of the node edges.
  //
  for (auto i = 0; i < get_port_count(); i++)
  {
    float px = 0.f;
    float py = 0.f;
    get_port_pos(i, px, py);
    draw_ptr->draw_circle(px, py, 4.f, style->m_foreground);
    draw_ptr->draw_circle(px, py, 2.5f, style->m_text);
  }
}
