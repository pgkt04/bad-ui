#include "demo.h"

std::shared_ptr<ui_style> demo_create_style()
{
  auto style = std::make_shared<ui_style>();
  style->m_accent = ui_color(255, 255, 255, 255);
  style->m_background = ui_color(5, 5, 5, 255);
  style->m_foreground = ui_color(0x2d, 0x30, 0x35, 255);
  style->m_text = ui_color(255, 255, 255, 255);
  style->m_control_height = 15.f;
  style->m_padding = 5.f;
  style->m_window_rounding_enabled = true;
  style->m_window_rounding = 0.08f;
  style->m_window_resize_enabled = true;
  style->m_hide_os_cursor = true;
  style->m_group_rounding_enabled = true;
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
  static ui_color demo_color = ui_color(128, 64, 255, 255);

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
          basic_group->push(std::make_shared<ui_slider>("Volume", &volume));
          basic_group->push(std::make_shared<ui_slider>("Brightness", &brightness));
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
          color_group->push(std::make_shared<ui_button>("Reset Color", [] { demo_color = ui_color(128, 64, 255, 255); }));
          color_group->push(std::make_shared<ui_checkbox>("Preview enabled", &notifications));
        }
        controls_group->push(color_group);

        auto scroll_group = std::make_shared<ui_group>(true);
        scroll_group->set_scroll_enabled(true);
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
