#include "ui_parent.h"

ui_parent::ui_parent()
{
  m_children = {};
  m_last_tab = -1;
  m_scroll_offset = 0.f;
  m_content_height = 0.f;
  m_scroll_dragging = false;
  m_scroll_drag_offset = 0.f;
}

void ui_parent::add_child(std::shared_ptr<ui_object> object)
{
  m_children.push_back(object);
}

std::vector<std::shared_ptr<ui_object>> ui_parent::get_children()
{
  return m_children;
}

static float clamp_scroll_value(float value, float max_value)
{
  if (value < 0.f)
    return 0.f;

  if (value > max_value)
    return max_value;

  return value;
}

static ui_dimension get_scroll_viewport(ui_object* object, std::shared_ptr<ui_style> style)
{
  auto dimensions = object->get_dimensions();

  if (object->get_is_tab())
  {
    dimensions.m_y += style->m_control_height;
    dimensions.m_h -= style->m_control_height;
  }

  if (dimensions.m_h < 0.f)
    dimensions.m_h = 0.f;

  return dimensions;
}

static bool intersects_vertical(ui_dimension dimension, ui_dimension viewport)
{
  return dimension.m_y + dimension.m_h >= viewport.m_y && dimension.m_y <= viewport.m_y + viewport.m_h;
}

static bool fits_vertical(ui_dimension dimension, ui_dimension viewport)
{
  return dimension.m_y >= viewport.m_y && dimension.m_y + dimension.m_h <= viewport.m_y + viewport.m_h;
}

static bool is_child_visible(std::shared_ptr<ui_object> child, ui_dimension viewport)
{
  if (child->get_dynamic())
    return intersects_vertical(child->get_dimensions(), viewport);

  return fits_vertical(child->get_dimensions(), viewport);
}

static ui_dimension get_scrollbar_track(ui_dimension viewport)
{
  auto width = 6.f;

  return ui_dimension(
    viewport.m_x + viewport.m_w - width - 2.f,
    viewport.m_y + 2.f,
    width,
    viewport.m_h - 4.f
  );
}

static ui_dimension get_scrollbar_thumb(ui_dimension track, float viewport_height, float content_height, float scroll_offset, float min_height)
{
  auto thumb_h = track.m_h * (viewport_height / content_height);

  if (thumb_h < min_height)
    thumb_h = min_height;

  if (thumb_h > track.m_h)
    thumb_h = track.m_h;

  auto max_scroll = content_height - viewport_height;
  auto thumb_y = track.m_y;

  if (max_scroll > 0.f && track.m_h > thumb_h)
    thumb_y += (track.m_h - thumb_h) * (scroll_offset / max_scroll);

  return ui_dimension(track.m_x, thumb_y, track.m_w, thumb_h);
}

void ui_parent::input_children(ui_input& input)
{
  auto style = get_style();
  auto viewport = style ? get_scroll_viewport(this, style) : get_dimensions();
  auto max_scroll = m_content_height > viewport.m_h ? m_content_height - viewport.m_h : 0.f;

  if (!input.mouse.buttons[ui_button_left])
  {
    m_scroll_dragging = false;
  }
  else if (style && max_scroll > 0.f)
  {
    auto track = get_scrollbar_track(viewport);
    auto thumb = get_scrollbar_thumb(track, viewport.m_h, m_content_height, m_scroll_offset, style->m_control_height);

    if (m_scroll_dragging)
    {
      auto range = track.m_h - thumb.m_h;

      if (range > 0.f)
      {
        auto thumb_y = clamp_scroll_value(input.mouse.pos_y - m_scroll_drag_offset, range + track.m_y);
        if (thumb_y < track.m_y)
          thumb_y = track.m_y;

        m_scroll_offset = clamp_scroll_value(((thumb_y - track.m_y) / range) * max_scroll, max_scroll);
      }

      input.handled = true;
      return;
    }

    if (UI_IN_AREA(input.mouse, thumb))
    {
      m_scroll_dragging = true;
      m_scroll_drag_offset = input.mouse.pos_y - thumb.m_y;
      input.handled = true;
      return;
    }
  }

  if (style && max_scroll > 0.f && input.mouse.wheel_delta != 0.f && UI_IN_AREA(input.mouse, viewport))
  {
    m_scroll_offset = clamp_scroll_value(m_scroll_offset - input.mouse.wheel_delta, max_scroll);
    input.handled = true;
    return;
  }

  for (auto child : get_children())
  {
    if (child->get_render_last() && is_child_visible(child, viewport))
    {
      child->input(input);

      if (input.handled)
        return;
    }
  }

  for (auto child : get_children())
  {
    if (!child->get_render_last() && is_child_visible(child, viewport))
    {
      child->input(input);

      if (input.handled)
        return;
    }
  }
}

void ui_parent::handle_relocations(std::shared_ptr<ui_style> style_ptr)
{
  // Relocate all its children with the initial default first
  //
  auto dimensions = get_dimensions();
  auto viewport = get_scroll_viewport(this, style_ptr);
  auto dynamic_height = dimensions.m_h - style_ptr->m_padding;
  auto dynamic_y = get_is_col() ? dimensions.m_y + style_ptr->m_padding :
    dimensions.m_y + style_ptr->m_control_height + style_ptr->m_padding;
  auto content_start_y = dynamic_y;
  auto dynamic_x = get_is_tab() ? get_parent_dimensions().m_x : get_is_col() ?
    dimensions.m_x : dimensions.m_x + style_ptr->m_padding;
  auto dynamic_objects = 0;
  auto hidden_objects = 0;
  auto dynamic_width = get_is_tab() ? get_parent_dimensions().m_w : dimensions.m_w;

  // Predetermine children height and width
  //
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

  auto max_scroll = m_content_height > viewport.m_h ? m_content_height - viewport.m_h : 0.f;
  m_scroll_offset = clamp_scroll_value(m_scroll_offset, max_scroll);

  auto ignore_index = 0;
  auto tab_y = dynamic_y;
  auto tabs_started = false;

  // Set children height and width
  //
  for (auto child : get_children())
  {
    if (child->get_is_tab() && !tabs_started)
    {
      tab_y = dynamic_y;
      tabs_started = true;
    }

    child->set_dimensions(ui_dimension(
        child->get_is_tab() ? (this->get_is_tab() ? get_parent_dimensions().m_x : dimensions.m_x)
        + (dynamic_width * static_cast<float>(ignore_index)) : dynamic_x,
        (child->get_is_tab() ? tab_y : dynamic_y) - m_scroll_offset,
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

  m_content_height = dynamic_y - content_start_y;

  max_scroll = m_content_height > viewport.m_h ? m_content_height - viewport.m_h : 0.f;
  m_scroll_offset = clamp_scroll_value(m_scroll_offset, max_scroll);

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

void ui_parent::render_children(std::shared_ptr<ui_draw> draw_ptr)
{
  auto style = get_style();
  auto viewport = style ? get_scroll_viewport(this, style) : get_dimensions();

  for (auto child : get_children())
  {
    if (!child->get_render_last() && is_child_visible(child, viewport))
      child->render(draw_ptr);
  }

  for (auto child : get_children())
  {
    if (child->get_render_last() && is_child_visible(child, viewport))
      child->render(draw_ptr);
  }

  render_scrollbar(draw_ptr);
}

void ui_parent::render_scrollbar(std::shared_ptr<ui_draw> draw_ptr)
{
  auto style = get_style();

  if (!style)
    return;

  auto viewport = get_scroll_viewport(this, style);

  if (m_content_height <= viewport.m_h || viewport.m_h <= 0.f)
    return;

  auto track = get_scrollbar_track(viewport);
  auto thumb = get_scrollbar_thumb(track, viewport.m_h, m_content_height, m_scroll_offset, style->m_control_height);

  draw_ptr->draw_rectangle(track, style->m_foreground);
  draw_ptr->draw_rectangle(thumb, style->m_text);
}
