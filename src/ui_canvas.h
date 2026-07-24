#pragma once

#include "ui_control.h"
#include "ui_node.h"

#include <functional>
#include <memory>
#include <vector>

struct ui_wire
{
  ui_node* m_from;
  int m_out_port;
  ui_node* m_to;
  int m_in_port;
};

// Node-graph surface: owns free-floating ui_node children (absolute layout,
// no row flow), background-drag panning, a subtle grid, z-raise on press and
// bezier wires between node ports. Behaves as one dynamic control inside the
// normal layout, so it can sit in a tab like any other child.
//
class ui_canvas : public ui_control
{
  std::vector<std::shared_ptr<ui_node>> m_nodes;
  std::vector<ui_wire> m_wires;
  std::function<void()> m_on_think;

  float m_pan_x;
  float m_pan_y;
  bool m_panning;
  float m_pan_grab_x;
  float m_pan_grab_y;

  // In-flight wire drag; always anchored at an output port, dropping on an
  // input port connects.
  ui_node* m_drag_from;
  int m_drag_port;
  bool m_drag_active;

  // Node that accepted the current press; it alone receives input until
  // release, so a held press cannot leak fresh interactions into overlapping
  // nodes. Raw pointer is safe: nodes are owned by m_nodes and there is no
  // removal API (a future remove must sweep this, m_drag_from and m_wires).
  ui_node* m_press_node;

  bool find_port_at(float x, float y, ui_node** out_node, int* out_port);
  void disconnect_input(ui_node* node, int in_port);
  void raise_node_at(float x, float y);

public:
  ui_canvas();

  void push(std::shared_ptr<ui_node> node);
  bool connect(ui_node* from, int out_port, ui_node* to, int in_port);
  const std::vector<ui_wire>& get_wires() const;

  // Called at the start of every think; the place for app-side graph
  // evaluation (reading wires, moving values between nodes). Node dimensions
  // and port positions still hold the previous frame's layout at that point.
  void set_on_think(std::function<void()> on_think);

  virtual void input(ui_input& input) override;
  virtual bool think(std::shared_ptr<ui_style> style_ptr) override;
  virtual void render(std::shared_ptr<ui_draw> draw_ptr) override;
};
