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
  return style;
}

std::shared_ptr<ui_form> demo_create_ui()
{
  static bool selected = false;
  static float tab2_slider_value = 0.5f;
  static float subtab_slider_value = 0.25f;
  static const char* dropdown_items[] = { "First", "Second", "Third" };
  static int tab2_dropdown_value = 0;
  static int subtab_dropdown_value = 1;
  static ui_color demo_color = ui_color(128, 64, 255, 255);

  auto form = std::make_shared<ui_form>(ui_dimension(30, 30, 800, 400), "Title", 0, false);
  {
    auto tab = std::make_shared<ui_tab>("Tab 1");
    {
      tab->push(std::make_shared<ui_checkbox>("Checkbox 2", &selected));
      tab->push(std::make_shared<ui_checkbox>("Checkbox 3", &selected));
    }
    form->push(tab);

    auto tab2 = std::make_shared<ui_tab>("Tab 2");
    {
      auto group1 = std::make_shared<ui_group>(true);
      {
        group1->push(std::make_shared<ui_checkbox>("Checkbox 5", &selected));
        group1->push(std::make_shared<ui_slider>("Tab 2 Slider", &tab2_slider_value));
        group1->push(std::make_shared<ui_dropdown>("Tab 2 Mode", dropdown_items, 3, &tab2_dropdown_value));
        group1->push(std::make_shared<ui_button>("Toggle Checks", [] { selected = !selected; }));

        auto tab_group1 = std::make_shared<ui_tab>("Tab 1");
        {
          tab_group1->push(std::make_shared<ui_checkbox>("Checkbox 2", &selected));
          tab_group1->push(std::make_shared<ui_slider>("Sub Slider", &subtab_slider_value));
          tab_group1->push(std::make_shared<ui_dropdown>("Sub Mode", dropdown_items, 3, &subtab_dropdown_value));
          tab_group1->push(std::make_shared<ui_button>("Sub Button", [] { selected = !selected; }));
          tab_group1->push(std::make_shared<ui_checkbox>("Checkbox 2", &selected));
        }
        group1->push(tab_group1);

        auto tab_group2 = std::make_shared<ui_tab>("Tab 2");
        {
          tab_group2->push(std::make_shared<ui_checkbox>("Checkbox 2", &selected));
        }
        group1->push(tab_group2);

        group1->split();
        group1->push(std::make_shared<ui_checkbox>("Checkbox 5", &selected));
        group1->push(std::make_shared<ui_checkbox>("Checkbox 6", &selected));
        group1->push(std::make_shared<ui_checkbox>("Checkbox 7", &selected));
        group1->split();
        group1->push(std::make_shared<ui_checkbox>("Checkbox 8", &selected));
        group1->push(std::make_shared<ui_checkbox>("Checkbox 9", &selected));
        group1->push(std::make_shared<ui_checkbox>("Checkbox 10", &selected));

        auto group3 = std::make_shared<ui_group>(true);
        {
          group3->push(std::make_shared<ui_group>(true));
          group3->push(std::make_shared<ui_checkbox>("Checkbox 10", &selected));
        }
        group1->push(group3);
        group1->push(std::make_shared<ui_checkbox>("Checkbox 10", &selected));
      }
      tab2->push(group1);
    }
    form->push(tab2);

    // tab 3
    auto colors_tab = std::make_shared<ui_tab>("Colors");
    {
      colors_tab->push(std::make_shared<ui_color_picker>("Demo Color", &demo_color));
      colors_tab->push(std::make_shared<ui_button>("Reset Color", [] { demo_color = ui_color(128, 64, 255, 255); }));
      colors_tab->push(std::make_shared<ui_checkbox>("Overlay Test", &selected));
    }
    form->push(colors_tab);
  }

  return form;
}
