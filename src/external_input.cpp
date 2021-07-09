#include "external_input.h"

Events::Events(event_handler _handler)
{
  handler = _handler;
  server = "\\\\.\\pipe\\";
  pipe = nullptr;
}

void Events::run_server(const char* name)
{
  server.append(name);

  pipe = CreateNamedPipeA(
    server.c_str(),
    PIPE_ACCESS_DUPLEX, PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,
    1,
    1024 * 16,
    1024 * 16,
    NMPWAIT_USE_DEFAULT_WAIT,
    0
  );

  auto size = sizeof(event_data);
  char* buffer = new char[size];
  memset(buffer, 0, size);
  DWORD read_size;

  while (pipe != INVALID_HANDLE_VALUE)
  {
    if (ConnectNamedPipe(pipe, NULL) != FALSE)
    {
      while (ReadFile(pipe, buffer, size, &read_size, NULL))
        handler(reinterpret_cast<event_data*>(buffer));
    }
    DisconnectNamedPipe(pipe);
  }
}