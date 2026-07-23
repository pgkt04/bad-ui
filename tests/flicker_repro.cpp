// Sustained 1px wiggle at both hysteresis boundaries: after one settle frame
// the probe width must never change again.
#include "../src/ui.h"
#include <cstdio>

static ui_input g_input = {};

int main()
{
  auto style = std::make_shared<ui_style>();
  style->m_control_height = 15.f;
  style->m_padding = 5.f;

  static bool checks[6] = {};

  auto form = std::make_shared<ui_form>(ui_dimension(0, 0, 400, 300), "t", 0, false);
  auto tab = std::make_shared<ui_tab>("tab");
  auto group = std::make_shared<ui_group>(true);
  auto probe = std::make_shared<ui_checkbox>("probe row", &checks[0]);

  group->push(probe);
  for (int i = 1; i < 6; i++)
    group->push(std::make_shared<ui_checkbox>("row", &checks[i]));

  tab->push(group);
  form->push(tab);

  auto frame = [&](float h)
  {
    auto dim = form->get_dimensions();
    dim.m_h = h;
    form->set_dimensions(dim);
    g_input.handled = false;
    form->input(g_input);
    form->think(style);
    return probe->get_dimensions().m_w;
  };

  auto failures = 0;

  float boundaries[] = { 171.f, 172.f, 186.f, 187.f };

  for (auto base : boundaries)
  {
    frame(base);
    frame(base + 1.f);
    auto settled = frame(base); // settle one full cycle

    for (int i = 0; i < 20; i++)
    {
      auto w = frame((i & 1) ? base + 1.f : base);

      if (w != settled)
      {
        std::printf("FAIL: sustained flicker at h=%.1f: %.1f != %.1f\n", base, w, settled);
        failures += 1;
        break;
      }
    }
  }

  std::printf(failures ? "FAILED\n" : "PASS: no sustained width flicker\n");
  return failures ? 1 : 0;
}
