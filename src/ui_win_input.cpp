#include "ui_win_input.h"

ui_input win_input::get_input(UINT message, WPARAM wParam, LPARAM lParam)
{
  static ui_input ret = {};

  switch (message)
  {
  case WM_MOUSEMOVE:
    ret.mouse.pos_x = (float)((short)LOWORD(lParam));
    ret.mouse.pos_y = (float)((short)HIWORD(lParam));
    break;
  case WM_LBUTTONDOWN:
    ret.mouse.buttons[ui_button_left] = true;
    break;
  case WM_LBUTTONUP:
    ret.mouse.buttons[ui_button_left] = false;
    break;
  case WM_RBUTTONDOWN:
    ret.mouse.buttons[ui_button_right] = true;
    break;
  case WM_RBUTTONUP:
    ret.mouse.buttons[ui_button_right] = false;
    break;
  case WM_MBUTTONDOWN:
    ret.mouse.buttons[ui_button_middle] = true;
    break;
  case WM_MBUTTONUP:
    ret.mouse.buttons[ui_button_middle] = false;
    break;
  case WM_KEYDOWN:
    ret.keymap.keys[wParam].down = true;
    break;
  case WM_KEYUP:
    ret.keymap.keys[wParam].down = false;
    break;
  }

  return ret;
}
