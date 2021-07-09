#include "ui_group.h"

ui_group::ui_group(bool visible)
{
  set_dynamic(true);
  set_is_group(true);

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

  for (auto child : get_children())
    child->input(input);
}

bool ui_group::think(std::shared_ptr<ui_style> style_ptr)
{
  set_style(style_ptr);

  auto this_dimension = get_dimensions();
  auto cols = m_splits.size() + 1;
  auto col_width = this_dimension.m_w / cols;

  if (!m_init)
  {
    // create enough columns and add them to our list
    //
    for (auto i = 0; i < cols; i++)
      m_columns.push_back(std::make_shared<ui_column>(m_visible));

    // determine where all the splits are and grab each object
    //
    if (cols != 1)
    {
      auto cursor = 0;
      auto split_index = 0;
      auto next_split = 0;

      for (auto i = 0; i < m_children.size(); i++)
      {
        // if we hit the split or we used up all our splits
        //
        bool last_col = i == (m_children.size() - 1);

        if (i == m_splits[next_split] || last_col)
        {
          auto extra = 0;

          if (last_col)
          {
            next_split += 1;
            extra += 1;
          }

          // relocate all children from where we began to where we hit the split
          //
          for (auto j = cursor; j < i + extra; j++)
          {
            m_columns[next_split]->push(m_children[j]);
            cursor += 1;
          }

          // increment to next split if possible
          //
          if (next_split < (m_splits.size() - 1))
            next_split += 1;

          split_index += 1;
        }

        // set all the childrens width to the desired width
        //
        //auto child_dimensions = get_children()[i]->get_dimensions();
        //child_dimensions.m_w = col_width;
        //get_children()[i]->set_dimensions(child_dimensions);

      }
    }
    else
    {
      // just a regular column
      //
      for (auto child : m_children)
        m_columns[0]->push(child);
    }

    for (auto col : m_columns)
      add_child(col);

    m_init = true;
  }

  // manually handle sizing since our handle_relocations doesn't expect this kind of behaviour
  //
  int i = 0;
  for (auto child : get_children())
  {
    auto dim = child->get_dimensions();
    dim.m_x = this_dimension.m_x + col_width * i + style_ptr->m_padding;
    dim.m_y = this_dimension.m_y;
    dim.m_w = col_width - style_ptr->m_padding * 2.f;
    dim.m_h = this_dimension.m_h - (style_ptr->m_padding + style_ptr->m_control_height);
    child->set_dimensions(dim);
    child->set_parent_dimensions(this_dimension);
    i += 1;
  }

  for (auto child : get_children())
    child->think(style_ptr);

  return true;
}

void ui_group::render(std::shared_ptr<ui_draw> draw_ptr)
{
  for (auto child : get_children())
    child->render(draw_ptr);
}
