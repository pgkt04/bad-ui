#pragma once

#include "ui_defs.h"
#include <process.h>
#include <Windows.h>
#include <string>
#include <cstdint>

struct event_data
{
  bool key_state[256];
  int32_t cursor_x = 0;
  int32_t cursor_y = 0;
};

typedef void(*event_handler)(event_data*);

class Events
{
public:
  Events(event_handler);
  void run_server(const char*);

private:
  std::string server;
  HANDLE pipe;
  event_handler handler;
};