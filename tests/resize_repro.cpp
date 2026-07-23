// Headless repro: drive the form's resize logic with synthetic input and
// watch the width while dragging far past the minimum.
#include "../src/demo.h"
#include <cstdio>

static ui_input g_input = {};

static void frame(std::shared_ptr<ui_form> form, std::shared_ptr<ui_style> style, float x, float y, bool down, const char* tag)
{
  g_input.mouse.pos_x = x;
  g_input.mouse.pos_y = y;
  g_input.mouse.buttons[ui_button_left] = down;
  g_input.handled = false;
  form->input(g_input);
  form->think(style);

  auto dim = form->get_dimensions();
  std::printf("%-8s mouse=(%7.1f,%7.1f) down=%d  form=(%.1f, %.1f, %.1f, %.1f)\n",
    tag, x, y, down, dim.m_x, dim.m_y, dim.m_w, dim.m_h);
}

int main()
{
  auto form = demo_create_ui();
  auto style = demo_create_style();

  // settle a few frames with mouse idle
  for (int i = 0; i < 3; i++)
    frame(form, style, 500.f, 500.f, false, "settle");

  auto dim = form->get_dimensions();
  auto grip_x = dim.m_x + dim.m_w - 5.f;
  auto grip_y = dim.m_y + dim.m_h - 5.f;

  // press on the grip
  frame(form, style, grip_x, grip_y, true, "press");

  // drag left+up far past the minimum
  auto x = grip_x;
  auto y = grip_y;

  for (int i = 0; i < 40; i++)
  {
    x -= 25.f;
    y -= 15.f;
    frame(form, style, x, y, true, "drag");
  }

  // hold roughly still, tiny wiggle like a human hand
  for (int i = 0; i < 20; i++)
  {
    x += (i & 1) ? 3.f : -3.f;
    y += (i & 1) ? -2.f : 2.f;
    frame(form, style, x, y, true, "wiggle");
  }

  // drag back right, width should only grow once cursor is back at corner
  for (int i = 0; i < 10; i++)
  {
    x += 30.f;
    frame(form, style, x, y, true, "return");
  }

  frame(form, style, x, y, false, "release");

  return 0;
}
