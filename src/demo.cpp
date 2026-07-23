#include "demo.h"

std::shared_ptr<ui_style> demo_create_style()
{
  auto style = std::make_shared<ui_style>();
  style->m_accent = ui_color(224, 82, 74, 255);
  style->m_background = ui_color(28, 28, 30, 255);
  style->m_foreground = ui_color(46, 46, 50, 255);
  style->m_text = ui_color(200, 200, 200, 255);
  style->m_border = ui_color(168, 56, 50, 255);
  style->m_control_height = 15.f;
  style->m_padding = 5.f;
  style->m_window_rounding_enabled = false;
  style->m_window_rounding = 0.08f;
  style->m_window_resize_enabled = true;
  style->m_hide_os_cursor = true;
  style->m_group_rounding_enabled = false;
  style->m_group_rounding = 0.025f;
  return style;
}

std::shared_ptr<ui_form> demo_create_ui()
{
  static bool notifications = true;
  static bool compact_mode = false;
  static bool sync_enabled = true;
  static bool scroll_items[18] = {};
  static float volume = 0.5f;
  static float brightness = 0.75f;
  static float nested_value = 0.25f;
  static const char* mode_items[] = { "Browse", "Edit", "Review" };
  static const char* quality_items[] = { "Low", "Medium", "High" };
  static int mode = 0;
  static int quality = 2;
  static ui_color demo_color = ui_color(255, 132, 92, 255);
  static bool grid_a = true;
  static bool grid_b = false;
  static bool grid_c = true;
  static float grid_weight_a = 0.4f;
  static float grid_weight_b = 0.6f;
  static float grid_weight_c = 0.3f;

  auto form = std::make_shared<ui_form>(ui_dimension(30, 30, 800, 400), "Bad UI Demo", 0, false);
  {
    auto controls_tab = std::make_shared<ui_tab>("Controls");
    {
      auto controls_group = std::make_shared<ui_group>(false);
      {
        auto basic_group = std::make_shared<ui_group>(true);
        {
          basic_group->push(std::make_shared<ui_text>("Basic controls"));
          basic_group->push(std::make_shared<ui_checkbox>("Notifications", &notifications));
          basic_group->push(std::make_shared<ui_checkbox>("Compact mode", &compact_mode));
          basic_group->push(std::make_shared<ui_separator>("Levels"));
          basic_group->push(std::make_shared<ui_slider>("Volume", &volume));
          basic_group->push(std::make_shared<ui_slider>("Brightness", &brightness));
          basic_group->push(std::make_shared<ui_separator>());
          basic_group->push(std::make_shared<ui_dropdown>("Mode", mode_items, 3, &mode));
          basic_group->push(std::make_shared<ui_dropdown>("Quality", quality_items, 3, &quality));
          basic_group->push(std::make_shared<ui_button>("Toggle Notifications", [] { notifications = !notifications; }));
        }
        controls_group->push(basic_group);

        controls_group->split();
        auto color_group = std::make_shared<ui_group>(true);
        {
          color_group->push(std::make_shared<ui_text>("Color and actions"));
          color_group->push(std::make_shared<ui_color_picker>("Accent Color", &demo_color));
          color_group->push(std::make_shared<ui_button>("Reset Color", [] { demo_color = ui_color(255, 132, 92, 255); }));
          color_group->push(std::make_shared<ui_checkbox>("Preview enabled", &notifications));
        }
        controls_group->push(color_group);

        auto scroll_group = std::make_shared<ui_group>(true);
        {
          scroll_group->push(std::make_shared<ui_text>("Scrollable demo"));
          scroll_group->push(std::make_shared<ui_checkbox>("List item 01", &scroll_items[0]));
          scroll_group->push(std::make_shared<ui_checkbox>("List item 02", &scroll_items[1]));
          scroll_group->push(std::make_shared<ui_checkbox>("List item 03", &scroll_items[2]));
          scroll_group->push(std::make_shared<ui_checkbox>("List item 04", &scroll_items[3]));
          scroll_group->push(std::make_shared<ui_checkbox>("List item 05", &scroll_items[4]));
          scroll_group->push(std::make_shared<ui_checkbox>("List item 06", &scroll_items[5]));
          scroll_group->push(std::make_shared<ui_checkbox>("List item 07", &scroll_items[6]));
          scroll_group->push(std::make_shared<ui_checkbox>("List item 08", &scroll_items[7]));
          scroll_group->push(std::make_shared<ui_checkbox>("List item 09", &scroll_items[8]));
          scroll_group->push(std::make_shared<ui_checkbox>("List item 10", &scroll_items[9]));
          scroll_group->push(std::make_shared<ui_checkbox>("List item 11", &scroll_items[10]));
          scroll_group->push(std::make_shared<ui_checkbox>("List item 12", &scroll_items[11]));
        }
        controls_group->push(scroll_group);
      }
      controls_tab->push(controls_group);
    }
    form->push(controls_tab);

    auto layout_tab = std::make_shared<ui_tab>("Layout");
    {
      auto layout_group = std::make_shared<ui_group>(true);
      {
        layout_group->push(std::make_shared<ui_text>("Column one"));
        layout_group->push(std::make_shared<ui_checkbox>("Sync enabled", &sync_enabled));
        layout_group->push(std::make_shared<ui_slider>("Nested value", &nested_value));

        auto nested_tab1 = std::make_shared<ui_tab>("Details");
        {
          nested_tab1->push(std::make_shared<ui_text>("Nested tabs stay in flow"));
          nested_tab1->push(std::make_shared<ui_checkbox>("Use cached data", &compact_mode));
          nested_tab1->push(std::make_shared<ui_button>("Flip Sync", [] { sync_enabled = !sync_enabled; }));
        }
        layout_group->push(nested_tab1);

        auto nested_tab2 = std::make_shared<ui_tab>("Status");
        {
          nested_tab2->push(std::make_shared<ui_text>("Secondary nested panel"));
          nested_tab2->push(std::make_shared<ui_checkbox>("Show hints", &notifications));
        }
        layout_group->push(nested_tab2);

        layout_group->split();
        layout_group->push(std::make_shared<ui_text>("Third column"));
        layout_group->push(std::make_shared<ui_checkbox>("Pinned panel", &notifications));
        layout_group->push(std::make_shared<ui_checkbox>("Rounded window", &compact_mode));
        layout_group->push(std::make_shared<ui_button>("Enable Sync", [] { sync_enabled = true; }));
      }
      layout_tab->push(layout_group);
    }
    form->push(layout_tab);

    auto grid_tab = std::make_shared<ui_tab>("Grid");
    {
      // Three separate visible groups laid out side by side by an invisible
      // wrapper (two splits). Use it to confirm the outer margins and every
      // gutter are one padding, exactly like the two-column tabs.
      auto grid_group = std::make_shared<ui_group>(false);
      {
        auto grid_group_a = std::make_shared<ui_group>(true);
        {
          grid_group_a->push(std::make_shared<ui_text>("Column A"));
          grid_group_a->push(std::make_shared<ui_checkbox>("Enable A", &grid_a));
          grid_group_a->push(std::make_shared<ui_slider>("Weight A", &grid_weight_a));
          grid_group_a->push(std::make_shared<ui_dropdown>("Mode", mode_items, 3, &mode));
        }
        grid_group->push(grid_group_a);

        grid_group->split();
        auto grid_group_b = std::make_shared<ui_group>(true);
        {
          grid_group_b->push(std::make_shared<ui_text>("Column B"));
          grid_group_b->push(std::make_shared<ui_checkbox>("Enable B", &grid_b));
          grid_group_b->push(std::make_shared<ui_slider>("Weight B", &grid_weight_b));
          grid_group_b->push(std::make_shared<ui_button>("Reset B", [] { grid_weight_b = 0.5f; }));
        }
        grid_group->push(grid_group_b);

        grid_group->split();
        auto grid_group_c = std::make_shared<ui_group>(true);
        {
          grid_group_c->push(std::make_shared<ui_text>("Column C"));
          grid_group_c->push(std::make_shared<ui_checkbox>("Enable C", &grid_c));
          grid_group_c->push(std::make_shared<ui_slider>("Weight C", &grid_weight_c));
          grid_group_c->push(std::make_shared<ui_dropdown>("Quality", quality_items, 3, &quality));
        }
        grid_group->push(grid_group_c);

        grid_group->split();
        auto grid_group_d = std::make_shared<ui_group>(true);
        {
            grid_group_d->push(std::make_shared<ui_text>("Column C"));
            grid_group_d->push(std::make_shared<ui_checkbox>("Enable C", &grid_c));
            grid_group_d->push(std::make_shared<ui_slider>("Weight C", &grid_weight_c));
            grid_group_d->push(std::make_shared<ui_dropdown>("Quality", quality_items, 3, &quality));
        }
        grid_group->push(grid_group_d);

      }
      grid_tab->push(grid_group);
    }
    form->push(grid_tab);

    auto colors_tab = std::make_shared<ui_tab>("Colors");
    {
      colors_tab->push(std::make_shared<ui_text>("Shared color state"));
      colors_tab->push(std::make_shared<ui_color_picker>("Accent Color", &demo_color));
      colors_tab->push(std::make_shared<ui_checkbox>("Preview enabled", &notifications));
    }
    form->push(colors_tab);
  }

  return form;
}
