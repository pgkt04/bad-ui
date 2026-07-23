#include "ui_group.h"

ui_group::ui_group(bool visible)
{
  set_dynamic(true);
  set_is_group(true);
  // Groups scroll by default: the flag is forwarded to the columns that hold
  // the actual children, and the scrollbar only appears on overflow.
  set_scroll_enabled(true);

  m_visible = visible;
  m_object_count = 0;
  m_init = false;
}

void ui_group::split()
{
  m_splits.push_back(m_object_count);
  // add another group to this and treat it like a tab?
}

void ui_group::push(std::shared_ptr<ui_object> object)
{
  m_object_count += 1;
  m_children.push_back(object);
}

void ui_group::input(ui_input& input)
{
  set_input(input);
  input_children(input, false);
}

bool ui_group::think(std::shared_ptr<ui_style> style_ptr)
{
  set_style(style_ptr);

  auto this_dimension = get_dimensions();
  auto cols = m_splits.size() + 1;

  if (!m_init)
  {
    // create enough columns and add them to our list
    //
    for (size_t i = 0; i < cols; i++)
      m_columns.push_back(std::make_shared<ui_column>(m_visible));

    // Assign every child to a column. m_splits holds the child indices where a
    // new column begins, so a child's column is the number of splits recorded
    // at or before it: children before the first split go to column 0, children
    // between the first and second split go to column 1, and so on. This works
    // for any column count (the previous version skipped the middle column once
    // there were three or more).
    //
    size_t split_index = 0;

    for (size_t i = 0; i < m_children.size(); i++)
    {
      while (split_index < m_splits.size() &&
             static_cast<size_t>(m_splits[split_index]) <= i)
        split_index += 1;

      m_columns[split_index]->push(m_children[i]);
    }

    for (auto col : m_columns)
    {
      col->set_scroll_enabled(get_scroll_enabled());
      add_child(col);
    }

    m_init = true;
  }

  // Horizontal spacing is uniform across both layout styles: every rendered
  // group frame keeps a single `padding` outer margin and a single `padding`
  // gutter between neighbours. Frames are placed on this grid:
  //
  //   frame_x[i] = x + padding + (frame_width + padding) * i
  //
  // How the grid is reached differs by group kind:
  //   * Visible group: its own columns draw the frames, so the columns sit on
  //     the grid directly (child_pad = 0).
  //   * Invisible group: it is a layout rail whose columns each host a visible
  //     child group, and that child insets itself by `padding` on both sides.
  //     We therefore grow each column box by `padding` per side (and shift it
  //     left to match) so the child's own inset lands its frame back on the
  //     grid. Without this compensation two neighbouring child groups would
  //     each add a `padding` margin and double the gap between them.
  auto outer = style_ptr->m_padding;
  auto gutter = style_ptr->m_padding;
  auto col_count = static_cast<float>(cols);
  auto frame_width = (this_dimension.m_w - outer * 2.f - gutter * (col_count - 1.f)) / col_count;
  auto child_pad = m_visible ? 0.f : style_ptr->m_padding;
  int i = 0;
  for (auto child : get_children())
  {
    auto frame_x = this_dimension.m_x + outer + (frame_width + gutter) * static_cast<float>(i);

    auto dim = child->get_dimensions();
    dim.m_x = frame_x - child_pad;
    dim.m_y = this_dimension.m_y;
    dim.m_w = frame_width + child_pad * 2.f;
    dim.m_h = this_dimension.m_h;

    if (dim.m_w < 0.f)
      dim.m_w = 0.f;

    if (dim.m_h < 0.f)
      dim.m_h = 0.f;

    child->set_dimensions(dim);
    child->set_parent_dimensions(this_dimension);

    auto child_parent = std::dynamic_pointer_cast<ui_parent>(child);
    if (child_parent)
      child_parent->set_scroll_enabled(get_scroll_enabled());

    i += 1;
  }

  for (auto child : get_children())
    child->think(style_ptr);

  return true;
}

void ui_group::render(std::shared_ptr<ui_draw> draw_ptr)
{
  render_children(draw_ptr, false);
}
