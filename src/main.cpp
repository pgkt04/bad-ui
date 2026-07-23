#ifdef _WIN32

#include <Windows.h>
#include <cassert>
#include "demo.h"
#include "ui_win_input.h"
#include "external_input.h"
#include <d3d9.h>
#include <d3dx9.h>
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

#ifdef _DEBUG
#include <iostream>
#endif

static std::shared_ptr<ui_form> g_form = nullptr;
static LPDIRECT3D9              g_pD3D = NULL;
static LPDIRECT3DDEVICE9        g_pd3dDevice = NULL;
static D3DPRESENT_PARAMETERS    g_d3dpp = {};

LRESULT CALLBACK window_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
bool create_device(HWND window_handle);
void cleanup_device();
void reset_device();
void begin_input_thread();

class ui_dx9_wrapper : public ui_draw
{
  LPDIRECT3DDEVICE9 m_device;
  ID3DXFont* m_font;

public:
  ui_dx9_wrapper()
  {
    m_device = nullptr;
    m_font = nullptr;
  }

  void set_device(LPDIRECT3DDEVICE9 device)
  {
    m_device = device;
  }

  virtual void impl_draw_line(float x0, float y0, float x1, float y1, ui_color color) override
  {
    assert(m_device);

    ID3DXLine* m_Line;
    D3DXCreateLine(m_device, &m_Line);
    D3DXVECTOR2 line[] = { D3DXVECTOR2(x0, y0), D3DXVECTOR2(x1, y1) };
    m_Line->SetWidth(1);
    m_Line->Begin();
    m_Line->Draw(line, 2, color.get_argb());
    m_Line->End();
    m_Line->Release();
  }

  virtual void impl_draw_rectangle(ui_dimension dimension, ui_color color) override
  {
    assert(m_device);

    D3DRECT BarRect = { (long)dimension.m_x, (long)dimension.m_y, (long)(dimension.m_x + dimension.m_w), (long)(dimension.m_y + dimension.m_h) };
    m_device->Clear(1, &BarRect, 0x00000001, color.get_argb(), 0, 0);
  }

  virtual void impl_draw_text(const char* text, float x, float y, ui_color color) override
  {
    assert(m_font);
    auto rect = RECT{ (LONG)x, (LONG)y, (LONG)x, (LONG)y };
    constexpr int left = 0;
    constexpr int center = 1;
    constexpr int right = 2;
    constexpr int orientation = left;
    switch (orientation)
    {
    case left:
      m_font->DrawTextA(NULL, text, -1, &rect, DT_LEFT | DT_NOCLIP, color.get_argb());
      break;
    case center:
      m_font->DrawTextA(NULL, text, -1, &rect, DT_CENTER | DT_NOCLIP, color.get_argb());
      break;
    case right:
      m_font->DrawTextA(NULL, text, -1, &rect, DT_RIGHT | DT_NOCLIP, color.get_argb());
      break;
    default:
      break;
    }
  }

  virtual float impl_measure_text(const char* text) override
  {
    assert(m_font);
    auto rect = RECT{ 0, 0, 0, 0 };
    m_font->DrawTextA(NULL, text, -1, &rect, DT_CALCRECT | DT_LEFT | DT_NOCLIP, 0);
    return static_cast<float>(rect.right - rect.left);
  }

  // Scissor rect so partially visible text is clipped pixel perfect.
  // Rectangles/lines are pre-clipped by ui_draw (Clear ignores the scissor).
  //
  virtual void apply_clip(const ui_dimension* clip) override
  {
    assert(m_device);

    if (clip)
    {
      RECT rect = {
        (LONG)clip->m_x,
        (LONG)clip->m_y,
        (LONG)(clip->m_x + clip->m_w),
        (LONG)(clip->m_y + clip->m_h)
      };

      m_device->SetScissorRect(&rect);
      m_device->SetRenderState(D3DRS_SCISSORTESTENABLE, TRUE);
    }
    else
    {
      m_device->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
    }
  }

  void set_font(const char* caption, int height, bool bold, bool italic)
  {
    assert(m_device);
    D3DXCreateFontA(m_device, height, 0, (bold) ? FW_BOLD : FW_NORMAL, 1, (italic) ? 1 : 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH, caption, &m_font);
  }
};

// Entry point
//
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
#if _DEBUG
  if (AllocConsole())
  {
    FILE* temp;
    freopen_s(&temp, ("CONOUT$"), ("w"), stdout);
    freopen_s(&temp, ("CONOUT$"), ("w"), stderr);
    freopen_s(&temp, ("CONIN$"), ("r"), stdin);
    std::cout.clear();
    std::clog.clear();
    std::cerr.clear();
    std::cin.clear();

    const auto h_con_out = CreateFileA
    (
      ("CONOUT$"),
      GENERIC_READ | GENERIC_WRITE,
      FILE_SHARE_READ | FILE_SHARE_WRITE,
      NULL,
      OPEN_EXISTING,
      FILE_ATTRIBUTE_NORMAL,
      NULL
    );

    const auto h_con_in = CreateFileA
    (
      ("CONIN$"),
      GENERIC_READ | GENERIC_WRITE,
      FILE_SHARE_READ | FILE_SHARE_WRITE,
      NULL,
      OPEN_EXISTING,
      FILE_ATTRIBUTE_NORMAL,
      NULL
    );

    SetStdHandle(STD_OUTPUT_HANDLE, h_con_out);
    SetStdHandle(STD_ERROR_HANDLE, h_con_out);
    SetStdHandle(STD_INPUT_HANDLE, h_con_in);
    std::wcout.clear();
    std::wclog.clear();
    std::wcerr.clear();
    std::wcin.clear();
  }
#endif

  // Create application window
  WNDCLASSEX wc = {
    sizeof(WNDCLASSEX),
    CS_CLASSDC, window_proc,
    0L,
    0L,
    GetModuleHandle(0),
    NULL,
    NULL,
    NULL,
    NULL,
    "Bad UI Demo",
    NULL
  };

  RegisterClassEx(&wc);

  const auto hwnd = ::CreateWindow(
    wc.lpszClassName,
    "Bad UI Demo",
    WS_OVERLAPPEDWINDOW,
    0,
    0,
    1920,
    1080,
    NULL,
    NULL,
    wc.hInstance,
    NULL
  );

  // Setup test input
  //begin_input_thread();

  // Show the window
  ::ShowWindow(hwnd, SW_SHOWDEFAULT);
  ::UpdateWindow(hwnd);

  // Initialise
  if (!create_device(hwnd))
    return EXIT_FAILURE;

  // setup 
  auto draw_ptr = std::make_shared< ui_dx9_wrapper >();
  draw_ptr->set_device(g_pd3dDevice);
  draw_ptr->set_font("Tahoma", 13, false, false);

  auto color_scheme = demo_create_style();

  // setup a template menu
  g_form = demo_create_ui();

  MSG msg;
  ZeroMemory(&msg, sizeof(msg));
  while (msg.message != WM_QUIT)
  {
    if (PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
    {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
      continue;
    }

    // do get device here
    draw_ptr->set_device(g_pd3dDevice);

    g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
    g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    g_pd3dDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
    const auto clear_color = ui_color(0.26f, 0.27f, 0.32f, 1.00f);

    const auto clear_col_dx = D3DCOLOR_RGBA(
      static_cast<int>(clear_color.m_r * 255.0f),
      static_cast<int>(clear_color.m_g * 255.0f),
      static_cast<int>(clear_color.m_b * 255.0f),
      static_cast<int>(clear_color.m_a * 255.0f)
    );

    g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, clear_col_dx, 1.0f, 0);

    if (g_pd3dDevice->BeginScene() >= 0)
    {
      /// main rendering
      if (g_form->think(color_scheme))
        g_form->render(draw_ptr);

      g_pd3dDevice->EndScene();
    }

    const auto result = g_pd3dDevice->Present(
      nullptr,
      nullptr,
      nullptr,
      nullptr
    );

    /// Handle loss of D3D9 device
    if (result == D3DERR_DEVICELOST && g_pd3dDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
      reset_device();
  }

  return EXIT_SUCCESS;
}

LRESULT CALLBACK window_proc(const HWND hwnd, const UINT message, const WPARAM wParam, const LPARAM lParam)
{
#if 1
  auto win_input = win_input::get_input(message, wParam, lParam);

  if (g_form)
    g_form->input(win_input);
#endif

  return DefWindowProc(hwnd, message, wParam, lParam);
}

// converts event_data into ui_input format
//
ui_input convert_data(event_data* data)
{
  static ui_input ret{};
  ret.handled = false;

  ret.mouse.pos_x = (float)data->cursor_x;
  ret.mouse.pos_y = (float)data->cursor_y;
  ret.mouse.buttons[ui_button_left] = data->key_state[VK_LBUTTON];
  ret.mouse.buttons[ui_button_right] = data->key_state[VK_RBUTTON];
  ret.mouse.buttons[ui_button_middle] = data->key_state[VK_MBUTTON];

  for (int32_t i = 0; i < 256; i++)
  {
    ret.keymap.keys[i].down = data->key_state[i];
  }

  return ret;
}

void set_input(event_data* data)
{
  auto input = convert_data(data);

  // read data
  if (g_form)
    g_form->input(input);
}

void input_thread(void*)
{
  auto events = new Events(&set_input);
  events->run_server("king");
}

void begin_input_thread()
{
  _beginthread(input_thread, 0, nullptr);
}


bool create_device(const HWND window_handle)
{
  if ((g_pD3D = Direct3DCreate9(D3D_SDK_VERSION)) == NULL)
    return false;

  // Create the D3DDevice
  ZeroMemory(&g_d3dpp, sizeof(g_d3dpp));
  g_d3dpp.Windowed = TRUE;
  g_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
  g_d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
  g_d3dpp.EnableAutoDepthStencil = TRUE;
  g_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
  g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;

  if (g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, window_handle, D3DCREATE_HARDWARE_VERTEXPROCESSING, &g_d3dpp, &g_pd3dDevice) < 0)
    return false;

  return true;
}

void cleanup_device()
{
  if (g_pd3dDevice)
  {
    g_pd3dDevice->Release();
    g_pd3dDevice = nullptr;
  }
  if (g_pD3D)
  {
    g_pD3D->Release();
    g_pD3D = nullptr;
  }
}

void reset_device()
{
  auto hr = g_pd3dDevice->Reset(&g_d3dpp);
}

#endif
