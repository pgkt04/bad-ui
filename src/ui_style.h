#pragma once

#include "ui_defs.h"

class ui_style
{
public:
  ui_color m_background;
  ui_color m_foreground;
  ui_color m_accent;
  ui_color m_text;
  float m_window_title_height = 15.f;
  float m_padding = 5.f;
  float m_control_height = 15.f;
};