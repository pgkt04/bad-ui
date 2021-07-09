#pragma once

#include "ui_defs.h"
#include <cstdint>
#include <Windows.h>

namespace win_input
{
  // windows implementation of getting the input handler
  //
  ui_input get_input(UINT message, WPARAM wParam, LPARAM lParam);
}