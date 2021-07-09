/// Helper definitions for custom classes for kitto ui
///

#pragma once

class ui_dimension
{
public:
  float m_x, m_y, m_w, m_h;

  ui_dimension()
  {
    m_x = 0.f;
    m_y = 0.f;
    m_w = 0.f;
    m_h = 0.f;
  }

  ui_dimension(float x, float y, float w, float h)
  {
    m_x = x;
    m_y = y;
    m_w = w;
    m_h = h;
  }

  float& operator[](int i)
  {
    return ((float*)this)[i];
  }

  ui_dimension& operator=(ui_dimension d)
  {
    m_x = d.m_x;
    m_y = d.m_y;
    m_w = d.m_w;
    m_h = d.m_h;
    return *this;
  }

  ui_dimension& operator+=(ui_dimension d)
  {
    m_x += d.m_x;
    m_y += d.m_y;
    m_w += d.m_w;
    m_h += d.m_h;
    return *this;
  }

  ui_dimension& operator-=(ui_dimension d)
  {
    m_x -= d.m_x;
    m_y -= d.m_y;
    m_w -= d.m_w;
    m_h -= d.m_h;
    return *this;
  }

  ui_dimension& operator/=(ui_dimension d)
  {
    m_x /= d.m_x;
    m_y /= d.m_y;
    m_w /= d.m_w;
    m_h /= d.m_h;
    return *this;
  }

  ui_dimension& operator*=(ui_dimension d)
  {
    m_x *= d.m_x;
    m_y *= d.m_y;
    m_w *= d.m_w;
    m_h *= d.m_h;
    return *this;
  }

  ui_dimension operator+(ui_dimension d)
  {
    return ui_dimension(m_x + d.m_x, m_y + d.m_y, m_w + d.m_w, m_h + d.m_h);
  }

  ui_dimension operator-(ui_dimension d)
  {
    return ui_dimension(m_x - d.m_x, m_y - d.m_y, m_w - d.m_y, m_h - d.m_h);
  }

  ui_dimension operator /(ui_dimension d)
  {
    return ui_dimension(m_x / d.m_x, m_y / d.m_y, m_w / d.m_w, m_h / d.m_h);
  }

  ui_dimension operator *(ui_dimension d)
  {
    return ui_dimension(m_x * d.m_x, m_y * d.m_y, m_w * d.m_w, m_h * d.m_h);
  }
};

// maps unsigned 8 bits/channel to D3DCOLOR
// #define D3DCOLOR_ARGB(a,r,g,b) \
//    ((D3DCOLOR)((((a)&0xff)<<24)|(((r)&0xff)<<16)|(((g)&0xff)<<8)|((b)&0xff)))

class ui_color
{
public:
  float m_r, m_g, m_b, m_a;

  ui_color()
  {
    m_r = 0;
    m_g = 0;
    m_b = 0;
    m_a = 0;
  }

  ui_color(float r, float g, float b, float a)
  {
    m_r = r;
    m_g = g;
    m_b = b;
    m_a = a;
  }

  float& operator[](int i)
  {
    return ((float*)this)[i];
  }

  ui_color& operator=(ui_color c)
  {
    m_r = c.m_r;
    m_g = c.m_g;
    m_b = c.m_b;
    m_a = c.m_a;
    return *this;
  }

  ui_color& operator+=(ui_color c)
  {
    m_r += c.m_r;
    m_g += c.m_g;
    m_b += c.m_b;
    m_a += c.m_a;
    return *this;
  }

  ui_color& operator-=(ui_color c)
  {
    m_r -= c.m_r;
    m_g -= c.m_g;
    m_b -= c.m_b;
    m_a -= c.m_a;
    return *this;
  }

  ui_color& operator/=(ui_color c)
  {
    m_r /= c.m_r;
    m_g /= c.m_g;
    m_b /= c.m_b;
    m_a /= c.m_a;
    return *this;
  }

  ui_color& operator*=(ui_color c)
  {
    m_r *= c.m_r;
    m_g *= c.m_g;
    m_b *= c.m_b;
    m_a *= c.m_a;
    return *this;
  }

  ui_color operator+(ui_color c)
  {
    return ui_color(m_r + c.m_r, m_g + c.m_g, m_b + c.m_b, m_a + c.m_a);
  }

  ui_color operator-(ui_color c)
  {
    return ui_color(m_r - c.m_r, m_g - c.m_g, m_b - c.m_b, m_a - c.m_a);
  }

  ui_color operator /(ui_color c)
  {
    return ui_color(m_r / c.m_r, m_g / c.m_g, m_b / c.m_b, m_a / c.m_a);
  }

  ui_color operator *(ui_color c)
  {
    return ui_color(m_r * c.m_r, m_g * c.m_g, m_b * c.m_b, m_a * c.m_a);
  }

  unsigned long get_rgba()
  {
    return (unsigned long)((((int)m_r & 0xff) << 24) | (((int)m_g & 0xff) << 16) | (((int)m_b & 0xff) << 8) | ((int)m_a & 0xff));
  }

  unsigned long get_argb()
  {
    return (unsigned long)((((int)m_a & 0xff) << 24) | (((int)m_r & 0xff) << 16) | (((int)m_g & 0xff) << 8) | ((int)m_b & 0xff));
  }

};

enum ui_buttons
{
  ui_button_left,
  ui_button_middle,
  ui_button_right,
  ui_button_double,
  ui_button_max
};

struct ui_mouse
{
  bool buttons[ui_button_max];
  float pos_x;
  float pos_y;
};

struct ui_keys
{
  bool down;
};

struct ui_keymap
{
  ui_keys	keys[256];
};

struct ui_input
{
  ui_mouse mouse;
  ui_keymap keymap;
};

#define UI_IN_AREA(mouse, dimension) (mouse.pos_x > dimension.m_x) \
&& (mouse.pos_y > dimension.m_y) \
&& (mouse.pos_x < (dimension.m_x + dimension.m_w))\
 && (mouse.pos_y < (dimension.m_y + dimension.m_h))