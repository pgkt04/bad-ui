#pragma once

#include "ui_defs.h"

/// Implementing a text size calculator may come in great use!
/// https://github.com/ocornut/imgui/blob/417ac68f82912ae4291e8e9b05abf38877a64b83/imgui_draw.cpp#L2769
/// ImGui implementation as reference

class ui_draw
{
public:
  virtual void draw_line(float x0, float y0, float x1, float y1, ui_color color) = 0;
  virtual void draw_rectangle(ui_dimension dimension, ui_color color) = 0;
  virtual void draw_text(const char* text, float x, float y, ui_color color) = 0;
};