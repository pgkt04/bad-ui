#pragma once

#include "ui_defs.h"

#include <cmath>
#include <cstring>
#include <vector>

/// Implementing a text size calculator may come in great use!
/// https://github.com/ocornut/imgui/blob/417ac68f82912ae4291e8e9b05abf38877a64b83/imgui_draw.cpp#L2769
/// ImGui implementation as reference

class ui_draw
{
public:
  virtual ~ui_draw() = default;

  // Draw calls used by all controls. These run against the active clip rect
  // (if any) before reaching the render backend, so partially visible
  // controls get cut off correctly on any backend that can draw a line,
  // a rectangle and text.
  //
  void draw_line(float x0, float y0, float x1, float y1, ui_color color)
  {
    ui_dimension clip;

    if (get_clip(clip) && !clip_line(clip, x0, y0, x1, y1))
      return;

    impl_draw_line(x0, y0, x1, y1, color);
  }

  void draw_rectangle(ui_dimension dimension, ui_color color)
  {
    ui_dimension clip;

    if (get_clip(clip))
    {
      auto x0 = dimension.m_x > clip.m_x ? dimension.m_x : clip.m_x;
      auto y0 = dimension.m_y > clip.m_y ? dimension.m_y : clip.m_y;
      auto x1 = dimension.m_x + dimension.m_w < clip.m_x + clip.m_w ?
        dimension.m_x + dimension.m_w : clip.m_x + clip.m_w;
      auto y1 = dimension.m_y + dimension.m_h < clip.m_y + clip.m_h ?
        dimension.m_y + dimension.m_h : clip.m_y + clip.m_h;

      if (x1 <= x0 || y1 <= y0)
        return;

      dimension = ui_dimension(x0, y0, x1 - x0, y1 - y0);
    }

    impl_draw_rectangle(dimension, color);
  }

  void draw_text(const char* text, float x, float y, ui_color color)
  {
    ui_dimension clip;

    if (get_clip(clip))
    {
      // Text metrics are backend specific, so only cull text that cannot be
      // visible at all. Backends that implement apply_clip cut the partially
      // visible glyphs pixel perfect.
      if (clip.m_w <= 0.f || clip.m_h <= 0.f)
        return;

      if (x >= clip.m_x + clip.m_w || y >= clip.m_y + clip.m_h)
        return;
    }

    impl_draw_text(text, x, y, color);
  }

  // Composite helper: chevron pointing up or down, centered inside the given
  // area. Drawn with lines so it scales with the control instead of relying
  // on the backend font having a nice looking "v" glyph.
  //
  void draw_chevron(ui_dimension area, bool up, ui_color color)
  {
    auto size = area.m_w < area.m_h ? area.m_w : area.m_h;
    auto half_w = size * 0.25f;
    auto half_h = size * 0.14f;
    auto cx = area.m_x + area.m_w * 0.5f;
    auto cy = area.m_y + area.m_h * 0.5f;
    auto tip = up ? cy - half_h : cy + half_h;
    auto base = up ? cy + half_h : cy - half_h;

    draw_line(cx - half_w, base, cx, tip, color);
    draw_line(cx, tip, cx + half_w, base, color);
  }

  // Composite helper: filled circle from 1px horizontal scanline rects, so it
  // needs no backend primitive and clips like everything else. Intended for
  // small radii (node ports, knobs); cost grows linearly with the radius.
  //
  void draw_circle(float cx, float cy, float radius, ui_color color)
  {
    if (radius <= 0.f)
      return;

    // Sample row centers so the disc spans 2r both ways and radii < 1 still
    // draw one row.
    //
    for (auto dy = -radius + 0.5f; dy < radius; dy += 1.f)
    {
      auto half = std::sqrt(radius * radius - dy * dy);

      draw_rectangle(ui_dimension(cx - half, cy + dy - 0.5f, half * 2.f, 1.f), color);
    }
  }

  // Composite helper: cubic bezier flattened into line segments. Segment
  // count follows the control-polygon length (an upper bound on the curve
  // length), so backward-routed curves with close endpoints stay smooth.
  //
  void draw_bezier(float x0, float y0, float cx0, float cy0, float cx1, float cy1, float x1, float y1, ui_color color)
  {
    auto leg = [](float ax, float ay, float bx, float by)
    {
      return std::sqrt((bx - ax) * (bx - ax) + (by - ay) * (by - ay));
    };
    auto length = leg(x0, y0, cx0, cy0) + leg(cx0, cy0, cx1, cy1) + leg(cx1, cy1, x1, y1);
    auto segments = static_cast<int>(length / 8.f);

    if (segments < 8)
      segments = 8;

    if (segments > 32)
      segments = 32;

    auto px = x0;
    auto py = y0;

    for (auto i = 1; i <= segments; i++)
    {
      auto t = static_cast<float>(i) / static_cast<float>(segments);
      auto u = 1.f - t;
      auto x = u * u * u * x0 + 3.f * u * u * t * cx0 + 3.f * u * t * t * cx1 + t * t * t * x1;
      auto y = u * u * u * y0 + 3.f * u * u * t * cy0 + 3.f * u * t * t * cy1 + t * t * t * y1;

      draw_line(px, py, x, y, color);
      px = x;
      py = y;
    }
  }

  // Text width in pixels. Backends with real font metrics override
  // impl_measure_text; the default is a rough 8px-per-character estimate.
  //
  float measure_text(const char* text)
  {
    if (!text)
      return 0.f;

    return impl_measure_text(text);
  }

  // Clip rects nest; pushed rects are intersected with the active one.
  //
  void push_clip(ui_dimension dimension)
  {
    if (dimension.m_w < 0.f)
      dimension.m_w = 0.f;

    if (dimension.m_h < 0.f)
      dimension.m_h = 0.f;

    ui_dimension active;

    if (get_clip(active))
    {
      auto x0 = dimension.m_x > active.m_x ? dimension.m_x : active.m_x;
      auto y0 = dimension.m_y > active.m_y ? dimension.m_y : active.m_y;
      auto x1 = dimension.m_x + dimension.m_w < active.m_x + active.m_w ?
        dimension.m_x + dimension.m_w : active.m_x + active.m_w;
      auto y1 = dimension.m_y + dimension.m_h < active.m_y + active.m_h ?
        dimension.m_y + dimension.m_h : active.m_y + active.m_h;

      dimension = ui_dimension(x0, y0, x1 > x0 ? x1 - x0 : 0.f, y1 > y0 ? y1 - y0 : 0.f);
    }

    m_clip_stack.push_back(dimension);
    apply_clip(&m_clip_stack.back());
  }

  void pop_clip()
  {
    if (m_clip_stack.empty())
      return;

    m_clip_stack.pop_back();
    apply_clip(m_clip_stack.empty() ? nullptr : &m_clip_stack.back());
  }

protected:
  // Implemented by the render backend.
  //
  virtual void impl_draw_line(float x0, float y0, float x1, float y1, ui_color color) = 0;
  virtual void impl_draw_rectangle(ui_dimension dimension, ui_color color) = 0;
  virtual void impl_draw_text(const char* text, float x, float y, ui_color color) = 0;

  virtual float impl_measure_text(const char* text)
  {
    return static_cast<float>(std::strlen(text)) * 8.f;
  }

  // Optional: backends with hardware clipping (scissor rect, CGContext clip,
  // ImGui PushClipRect, ...) can override this for pixel perfect text
  // clipping. Called with the active clip rect, or nullptr to clear it.
  //
  virtual void apply_clip(const ui_dimension* clip)
  {
    (void)clip;
  }

private:
  bool get_clip(ui_dimension& out)
  {
    if (m_clip_stack.empty())
      return false;

    out = m_clip_stack.back();
    return true;
  }

  // Liang-Barsky helper for line clipping
  //
  static bool clip_line_axis(float p, float q, float& t0, float& t1)
  {
    if (p == 0.f)
      return q >= 0.f;

    auto r = q / p;

    if (p < 0.f)
    {
      if (r > t1)
        return false;

      if (r > t0)
        t0 = r;
    }
    else
    {
      if (r < t0)
        return false;

      if (r < t1)
        t1 = r;
    }

    return true;
  }

  static bool clip_line(ui_dimension clip, float& x0, float& y0, float& x1, float& y1)
  {
    auto t0 = 0.f;
    auto t1 = 1.f;
    auto dx = x1 - x0;
    auto dy = y1 - y0;

    if (!clip_line_axis(-dx, x0 - clip.m_x, t0, t1))
      return false;

    if (!clip_line_axis(dx, clip.m_x + clip.m_w - x0, t0, t1))
      return false;

    if (!clip_line_axis(-dy, y0 - clip.m_y, t0, t1))
      return false;

    if (!clip_line_axis(dy, clip.m_y + clip.m_h - y0, t0, t1))
      return false;

    if (t1 < 1.f)
    {
      x1 = x0 + t1 * dx;
      y1 = y0 + t1 * dy;
    }

    if (t0 > 0.f)
    {
      x0 = x0 + t0 * dx;
      y0 = y0 + t0 * dy;
    }

    return true;
  }

  std::vector<ui_dimension> m_clip_stack;
};
