#include "ui_parent.h"

ui_parent::ui_parent()
{
  m_children = {};
}

void ui_parent::add_child(std::shared_ptr<ui_object> object)
{
  m_children.push_back(object);
}

std::vector<std::shared_ptr<ui_object>> ui_parent::get_children()
{
  return m_children;
}

void ui_parent::handle_relocations(std::shared_ptr<ui_style> style_ptr)
{
  // Relocate all its children with the initial default first
  //
  auto dimensions = get_dimensions();
  auto dynamic_height = dimensions.m_h - style_ptr->m_padding;
  auto dynamic_y = get_is_col() ? dimensions.m_y + style_ptr->m_padding :
    dimensions.m_y + style_ptr->m_control_height + style_ptr->m_padding;
  auto dynamic_x = get_is_tab() ? get_parent_dimensions().m_x : get_is_col() ?
    dimensions.m_x : dimensions.m_x + style_ptr->m_padding;
  auto dynamic_objects = 0;
  auto hidden_objects = 0;
  auto dynamic_width = get_is_tab() ? get_parent_dimensions().m_w : dimensions.m_w;

  // Predetermine children height and width
  //
  bool was_last_col = false;

  for (auto child : get_children())
  {
    child->set_parent_dimensions(get_is_tab() ? get_parent_dimensions() : dimensions);

    if (child->get_dynamic())
    {
      if (child->get_is_tab())
        hidden_objects += 1;
      else
        dynamic_objects += 1;
    }
    else
    {
      dynamic_height -= (style_ptr->m_control_height + style_ptr->m_padding);
    }
  }

  if (dynamic_objects > 0)
    dynamic_height /= static_cast<float>(dynamic_objects);

  if (hidden_objects > 0)
  {
    dynamic_height -= (style_ptr->m_control_height + style_ptr->m_padding);
    dynamic_width /= static_cast<float>(hidden_objects);
  }

  auto ignore_index = 0;
  auto best_tab_pos = 0;

  // Set children height and width
  //
  for (auto child : get_children())
  {
    child->set_dimensions(ui_dimension(
        child->get_is_tab() ? (this->get_is_tab() ? get_parent_dimensions().m_x : dimensions.m_x)
        + (dynamic_width * static_cast<float>(ignore_index)) : dynamic_x,
        child->get_is_tab() ? (best_tab_pos != 0) ? best_tab_pos : dynamic_y : dynamic_y,
        child->get_is_tab() ? dynamic_width : (this->get_is_tab() ? get_parent_dimensions().m_w : dimensions.m_w),
        child->get_dynamic() ? dynamic_height : style_ptr->m_control_height
      ));

    if (child->get_is_tab())
    {
      ignore_index += 1;

      // tab autofill
      // move dynamic_y to the tabs height
      //
      if (child->get_autofill() && child->get_selected())
      {
        auto parent_obj = std::dynamic_pointer_cast<ui_parent>(child);
        for (auto sub_child : parent_obj->get_children())
        {
          dynamic_y += sub_child->get_dimensions().m_h + (style_ptr->m_padding * 2) + style_ptr->m_control_height;
        }
      }

      best_tab_pos = (int)dynamic_y;

      if (child->get_selected())
        dynamic_y += dynamic_height;
    }
    else
    {
      // handle groups, and relocate its children
      //
      if (child->get_is_group())
      {
        // Give it the dynamic height
        //
        if (child->get_autofill())
        {
          // calculate the group's childrens height, then set its height
          //
          auto parent_obj = std::dynamic_pointer_cast<ui_parent>(child);
          auto children_size = parent_obj->get_children().size();
          auto height_per_child = style_ptr->m_padding + style_ptr->m_control_height;
          auto autofill_height = height_per_child * children_size;
          dynamic_y += autofill_height;

          auto child_dimensions = child->get_dimensions();
          child_dimensions.m_h = autofill_height;

          child->set_dimensions(child_dimensions);
        }
        else
        {
          dynamic_y += dynamic_height - style_ptr->m_control_height;
        }
      }
      else
      {
        // for all other children, set height as a regular control
        //
        if (!child->get_dynamic())
          dynamic_y += style_ptr->m_padding + style_ptr->m_control_height;

        // set x padding for non auto-filling children (aka all controls)
        //
        auto dim = child->get_dimensions();
        dim.m_x += style_ptr->m_padding;
        child->set_dimensions(dim);
      }
    }
  }

  int selected_tabs = 0;

  // Scope the selected tabs
  //
  for (auto children : get_children())
  {
    if (children->get_is_tab() && children->get_selected())
      selected_tabs += 1;
  }

  // Get the first tab to true
  // If the select tab changed, invalidate the last selected
  //
  auto index = 0;

  for (auto child : get_children())
  {
    if (child->get_is_tab())
    {
      index += 1;

      if (selected_tabs)
      {
        if (selected_tabs > 1 && child->get_selected())
        {
          if (m_last_tab == index)
            child->set_selected(false);
        }
      }
      else
      {
        child->set_selected(true);
        m_last_tab = index;
      }
    }
  }

  index = 0;

  // Update the newly selected tab
  for (auto child : get_children())
  {
    if (child->get_is_tab())
    {
      index += 1;

      if (child->get_selected() && selected_tabs > 1)
        m_last_tab = index;
    }
  }
}
