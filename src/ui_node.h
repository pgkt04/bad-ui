#pragma once

#include "ui_parent.h"

#include <vector>

// A port is an attachment point for wires, anchored to one of the node's
// content rows: inputs sit on the node's left edge, outputs on the right.
//
struct ui_node_port
{
  bool m_is_input;
  int m_row;
};

// Free-floating titled panel for ui_canvas. Position is local to the canvas
// origin (canvas position + pan), which the canvas passes in through
// set_parent_dimensions each frame. Children stack as standard rows under the
// title bar; the node's height follows its row count.
//
class ui_node : public ui_parent
{
  const char* m_title;
  float m_local_x;
  float m_local_y;
  float m_width;
  std::vector<ui_node_port> m_ports;
  bool m_dragging;
  float m_drag_dx;
  float m_drag_dy;

public:
  ui_node(const char* title, float local_x, float local_y, float width = 180.f);

  void push(std::shared_ptr<ui_object> object);

  int add_input(int row);
  int add_output(int row);

  int get_port_count() const;
  ui_node_port get_port(int index) const;

  // World-space center of a port; valid after think() placed the node.
  void get_port_pos(int index, float& out_x, float& out_y);

  virtual void input(ui_input& input) override;
  virtual bool think(std::shared_ptr<ui_style> style_ptr) override;
  virtual void render(std::shared_ptr<ui_draw> draw_ptr) override;
};
