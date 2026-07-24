#include "ui_canvas.h"

#include <cmath>

ui_canvas::ui_canvas()
{
  set_dynamic(true);

  m_pan_x = 0.f;
  m_pan_y = 0.f;
  m_panning = false;
  m_pan_grab_x = 0.f;
  m_pan_grab_y = 0.f;
  m_drag_from = nullptr;
  m_drag_port = 0;
  m_drag_active = false;
  m_press_node = nullptr;
}

void ui_canvas::push(std::shared_ptr<ui_node> node)
{
  m_nodes.push_back(node);
}

void ui_canvas::disconnect_input(ui_node* node, int in_port)
{
  for (size_t i = 0; i < m_wires.size(); i++)
  {
    if (m_wires[i].m_to == node && m_wires[i].m_in_port == in_port)
    {
      m_wires.erase(m_wires.begin() + static_cast<long>(i));
      return;
    }
  }
}

bool ui_canvas::connect(ui_node* from, int out_port, ui_node* to, int in_port)
{
  if (!from || !to)
    return false;

  if (out_port < 0 || out_port >= from->get_port_count())
    return false;

  if (in_port < 0 || in_port >= to->get_port_count())
    return false;

  if (from->get_port(out_port).m_is_input || !to->get_port(in_port).m_is_input)
    return false;

  // One wire per input; a new connection replaces the old one.
  //
  disconnect_input(to, in_port);
  m_wires.push_back(ui_wire{ from, out_port, to, in_port });

  return true;
}

const std::vector<ui_wire>& ui_canvas::get_wires() const
{
  return m_wires;
}

void ui_canvas::set_on_think(std::function<void()> on_think)
{
  m_on_think = on_think;
}

bool ui_canvas::find_port_at(float x, float y, ui_node** out_node, int* out_port)
{
  auto hit_radius = 7.f;

  for (size_t i = m_nodes.size(); i > 0; i--)
  {
    auto node = m_nodes[i - 1].get();

    for (auto p = 0; p < node->get_port_count(); p++)
    {
      float px = 0.f;
      float py = 0.f;
      node->get_port_pos(p, px, py);

      if ((x - px) * (x - px) + (y - py) * (y - py) <= hit_radius * hit_radius)
      {
        *out_node = node;
        *out_port = p;
        return true;
      }
    }
  }

  return false;
}

void ui_canvas::raise_node_at(float x, float y)
{
  for (size_t i = m_nodes.size(); i > 0; i--)
  {
    auto dim = m_nodes[i - 1]->get_dimensions();

    if (x > dim.m_x && x < dim.m_x + dim.m_w && y > dim.m_y && y < dim.m_y + dim.m_h)
    {
      auto node = m_nodes[i - 1];
      m_nodes.erase(m_nodes.begin() + static_cast<long>(i - 1));
      m_nodes.push_back(node);
      return;
    }
  }
}

void ui_canvas::input(ui_input& input)
{
  // First frame: input runs before any think has assigned dimensions/styles.
  //
  if (!get_style())
    return;

  set_input(input);

  auto dim = get_dimensions();
  auto fresh_press = take_fresh_press(input);
  auto inside = UI_IN_AREA(input.mouse, dim);
  auto down = input.mouse.buttons[ui_button_left];

  // Active wire drag: captured until release; dropping on an input port
  // connects, anywhere else discards.
  //
  if (m_drag_active)
  {
    if (!down)
    {
      ui_node* node = nullptr;
      int port = 0;

      // The drop point must be inside the canvas: ports of nodes dragged
      // beyond the clip rect are invisible and must not accept wires.
      //
      if (inside && find_port_at(input.mouse.pos_x, input.mouse.pos_y, &node, &port))
        connect(m_drag_from, m_drag_port, node, port);

      m_drag_active = false;
      m_drag_from = nullptr;
    }

    input.handled = true;
    return;
  }

  // Pan capture continues before nodes get a look: node rects lag one think
  // behind the pan, so a fast sweep can land inside a stale rect and the
  // node would swallow the event, freezing the pan mid-drag.
  //
  if (m_panning)
  {
    m_pan_x = input.mouse.pos_x - m_pan_grab_x;
    m_pan_y = input.mouse.pos_y - m_pan_grab_y;

    if (!down)
      m_panning = false;

    input.handled = true;
    return;
  }

  // Port press: outputs start a new wire; a connected input detaches its wire
  // and re-drags it from the source output.
  //
  if (!input.handled && fresh_press && inside)
  {
    ui_node* node = nullptr;
    int port = 0;

    if (find_port_at(input.mouse.pos_x, input.mouse.pos_y, &node, &port))
    {
      if (!node->get_port(port).m_is_input)
      {
        m_drag_from = node;
        m_drag_port = port;
        m_drag_active = true;
      }
      else
      {
        // A connected input hands its wire back for re-dragging; an
        // unconnected one has nothing to drag, but the press still belongs
        // to the port and must not fall through to raise/pan beneath.
        //
        for (auto& wire : m_wires)
        {
          if (wire.m_to == node && wire.m_in_port == port)
          {
            m_drag_from = wire.m_from;
            m_drag_port = wire.m_out_port;
            m_drag_active = true;
            disconnect_input(node, port);
            break;
          }
        }
      }

      input.handled = true;
      return;
    }
  }

  // Nodes, topmost first. A press is owned by the node that accepted it and
  // only that node sees input until release; without the latch a held press
  // sliding into an overlapping neighbour would start a fresh interaction
  // there. Fresh presses only count inside the canvas rect, since nodes
  // panned under other UI are clipped invisible.
  //
  if (!down)
    m_press_node = nullptr;

  if (m_press_node)
  {
    m_press_node->input(input);
    input.handled = true;
    return;
  }

  if (fresh_press && inside)
  {
    raise_node_at(input.mouse.pos_x, input.mouse.pos_y);

    for (size_t i = m_nodes.size(); i > 0 && !input.handled; i--)
    {
      m_nodes[i - 1]->input(input);

      if (input.handled)
        m_press_node = m_nodes[i - 1].get();
    }
  }
  else if (!down)
  {
    // Hover/release passes reach every node so their press trackers stay in
    // sync. Held presses that started elsewhere (outside the canvas, or on
    // another control) never reach nodes, so a clipped-invisible node cannot
    // start a drag from a press it never legitimately received.
    //
    for (size_t i = m_nodes.size(); i > 0 && !input.handled; i--)
      m_nodes[i - 1]->input(input);
  }

  // Pan: fresh press on empty canvas background; captured above on the
  // following events.
  //
  if (!input.handled && fresh_press && inside)
  {
    m_panning = true;
    m_pan_grab_x = input.mouse.pos_x - m_pan_x;
    m_pan_grab_y = input.mouse.pos_y - m_pan_y;
    input.handled = true;
  }
}

bool ui_canvas::think(std::shared_ptr<ui_style> style_ptr)
{
  set_style(style_ptr);

  if (m_on_think)
    m_on_think();

  auto dim = get_dimensions();
  auto origin = ui_dimension(dim.m_x + m_pan_x, dim.m_y + m_pan_y, 0.f, 0.f);

  for (auto& node : m_nodes)
  {
    node->set_parent_dimensions(origin);
    node->think(style_ptr);
  }

  return true;
}

static float grid_start(float edge, float pan, float spacing)
{
  auto offset = std::fmod(pan, spacing);

  if (offset < 0.f)
    offset += spacing;

  return edge + offset;
}

void ui_canvas::render(std::shared_ptr<ui_draw> draw_ptr)
{
  auto style = get_style();

  if (!style)
    return;

  auto dim = get_dimensions();

  draw_ptr->push_clip(dim);

  // Grid, offset by the pan so it visibly moves with the graph.
  //
  auto spacing = 28.f;
  auto grid_color = ui_color(style->m_foreground.m_r, style->m_foreground.m_g,
    style->m_foreground.m_b, 60);

  for (auto x = grid_start(dim.m_x, m_pan_x, spacing); x < dim.m_x + dim.m_w; x += spacing)
    draw_ptr->draw_rectangle(ui_dimension(x, dim.m_y, 1.f, dim.m_h), grid_color);

  for (auto y = grid_start(dim.m_y, m_pan_y, spacing); y < dim.m_y + dim.m_h; y += spacing)
    draw_ptr->draw_rectangle(ui_dimension(dim.m_x, y, dim.m_w, 1.f), grid_color);

  // Wires under the nodes, like the reference editors. The tangent length
  // follows the endpoint distance (not just the horizontal gap) so
  // near-vertical wires still curve instead of collapsing to straight lines.
  //
  auto wire_bend = [](float x0, float y0, float x1, float y1)
  {
    auto bend = std::sqrt((x1 - x0) * (x1 - x0) + (y1 - y0) * (y1 - y0)) * 0.5f;

    return bend > 60.f ? 60.f : bend;
  };

  for (auto& wire : m_wires)
  {
    float x0 = 0.f, y0 = 0.f, x1 = 0.f, y1 = 0.f;
    wire.m_from->get_port_pos(wire.m_out_port, x0, y0);
    wire.m_to->get_port_pos(wire.m_in_port, x1, y1);

    auto bend = wire_bend(x0, y0, x1, y1);

    draw_ptr->draw_bezier(x0, y0, x0 + bend, y0, x1 - bend, y1, x1, y1, style->m_text);
  }

  if (m_drag_active && m_drag_from)
  {
    float x0 = 0.f, y0 = 0.f;
    m_drag_from->get_port_pos(m_drag_port, x0, y0);

    auto mouse = get_input().mouse;
    auto bend = wire_bend(x0, y0, mouse.pos_x, mouse.pos_y);

    draw_ptr->draw_bezier(x0, y0, x0 + bend, y0, mouse.pos_x - bend, mouse.pos_y,
      mouse.pos_x, mouse.pos_y, style->m_accent);
  }

  for (auto& node : m_nodes)
    node->render(draw_ptr);

  draw_ptr->pop_clip();
}
