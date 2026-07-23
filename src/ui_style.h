#pragma once

#include "ui_defs.h"

class ui_style
{
public:
  ui_color m_background;
  ui_color m_foreground;
  ui_color m_accent;
  ui_color m_text;
  ui_color m_border;
  float m_window_title_height = 15.f;
  bool m_window_rounding_enabled = false;
  float m_window_rounding = 0.f;
  bool m_window_resize_enabled = false;
  // Minimum window size while resizing. With m_window_min_dynamic (default)
  // the form additionally clamps its width to what the currently selected
  // tab's content needs, since there is no horizontal scrolling. Set it to
  // false to only enforce the static values below.
  bool m_window_min_dynamic = true;
  float m_window_min_width = 220.f;
  float m_window_min_height = 160.f;
  bool m_hide_os_cursor = false;
  bool m_group_rounding_enabled = false;
  float m_group_rounding = 0.f;
  float m_padding = 5.f;
  float m_control_height = 15.f;
};
