#import <Cocoa/Cocoa.h>

#include "demo.h"

#include <algorithm>
#include <cassert>
#include <memory>

static std::shared_ptr<ui_form> g_form = nullptr;
static std::shared_ptr<ui_style> g_style = nullptr;
static ui_input g_input = {};

static CGFloat color_channel(float value)
{
  return static_cast<CGFloat>(std::max(0.0f, std::min(value, 255.0f)) / 255.0f);
}

static NSColor* ns_color(ui_color color)
{
  return [NSColor colorWithCalibratedRed:color_channel(color.m_r)
                                  green:color_channel(color.m_g)
                                   blue:color_channel(color.m_b)
                                  alpha:color_channel(color.m_a)];
}

static NSCursor* hidden_cursor()
{
  static NSCursor* cursor = nil;

  if (!cursor)
  {
    NSImage* image = [[NSImage alloc] initWithSize:NSMakeSize(1.0, 1.0)];
    cursor = [[NSCursor alloc] initWithImage:image hotSpot:NSMakePoint(0.0, 0.0)];
  }

  return cursor;
}

class ui_macos_wrapper : public ui_draw
{
  CGContextRef m_context;
  NSFont* m_font;

public:
  explicit ui_macos_wrapper(CGContextRef context)
  {
    m_context = context;
    m_font = [NSFont fontWithName:@"Tahoma" size:13.0];

    if (!m_font)
      m_font = [NSFont systemFontOfSize:13.0];
  }

  virtual void draw_line(float x0, float y0, float x1, float y1, ui_color color) override
  {
    assert(m_context);

    CGContextSetStrokeColorWithColor(m_context, ns_color(color).CGColor);
    CGContextSetLineWidth(m_context, 1.0);
    CGContextMoveToPoint(m_context, x0, y0);
    CGContextAddLineToPoint(m_context, x1, y1);
    CGContextStrokePath(m_context);
  }

  virtual void draw_rectangle(ui_dimension dimension, ui_color color) override
  {
    assert(m_context);

    CGContextSetFillColorWithColor(m_context, ns_color(color).CGColor);
    CGContextFillRect(m_context, CGRectMake(dimension.m_x, dimension.m_y, dimension.m_w, dimension.m_h));
  }

  virtual void draw_text(const char* text, float x, float y, ui_color color) override
  {
    if (!text)
      return;

    NSString* string = [NSString stringWithUTF8String:text];
    NSDictionary* attributes = @{
      NSFontAttributeName: m_font,
      NSForegroundColorAttributeName: ns_color(color)
    };

    [string drawAtPoint:NSMakePoint(x, y) withAttributes:attributes];
  }
};

static void send_input()
{
  if (g_form)
  {
    g_input.handled = false;
    g_form->input(g_input);
    g_input.mouse.wheel_delta = 0.f;
  }
}

@interface BadUIView : NSView
@end

@implementation BadUIView

- (BOOL)isFlipped
{
  return YES;
}

- (BOOL)acceptsFirstResponder
{
  return YES;
}

- (void)viewDidMoveToWindow
{
  [self.window makeFirstResponder:self];
  [self.window setAcceptsMouseMovedEvents:YES];
}

- (void)resetCursorRects
{
  if (g_style && g_style->m_hide_os_cursor)
    [self addCursorRect:self.bounds cursor:hidden_cursor()];
}

- (void)redraw
{
  [self setNeedsDisplay:YES];
}

- (void)drawRect:(NSRect)dirtyRect
{
  CGContextRef context = [[NSGraphicsContext currentContext] CGContext];
  CGContextSetFillColorWithColor(context, [NSColor colorWithCalibratedRed:0.26 green:0.27 blue:0.32 alpha:1.0].CGColor);
  CGContextFillRect(context, NSRectToCGRect(self.bounds));

  auto draw = std::make_shared<ui_macos_wrapper>(context);
  if (g_form->think(g_style))
    g_form->render(draw);
}

- (void)updateMouse:(NSEvent*)event
{
  NSPoint point = [self convertPoint:event.locationInWindow fromView:nil];
  g_input.mouse.pos_x = point.x;
  g_input.mouse.pos_y = point.y;
  send_input();
}

- (void)mouseMoved:(NSEvent*)event
{
  [self updateMouse:event];
}

- (void)mouseDragged:(NSEvent*)event
{
  [self updateMouse:event];
}

- (void)rightMouseDragged:(NSEvent*)event
{
  [self updateMouse:event];
}

- (void)otherMouseDragged:(NSEvent*)event
{
  [self updateMouse:event];
}

- (void)mouseDown:(NSEvent*)event
{
  g_input.mouse.buttons[ui_button_left] = true;
  [self updateMouse:event];
}

- (void)mouseUp:(NSEvent*)event
{
  g_input.mouse.buttons[ui_button_left] = false;
  [self updateMouse:event];
}

- (void)rightMouseDown:(NSEvent*)event
{
  g_input.mouse.buttons[ui_button_right] = true;
  [self updateMouse:event];
}

- (void)rightMouseUp:(NSEvent*)event
{
  g_input.mouse.buttons[ui_button_right] = false;
  [self updateMouse:event];
}

- (void)otherMouseDown:(NSEvent*)event
{
  g_input.mouse.buttons[ui_button_middle] = true;
  [self updateMouse:event];
}

- (void)otherMouseUp:(NSEvent*)event
{
  g_input.mouse.buttons[ui_button_middle] = false;
  [self updateMouse:event];
}

- (void)scrollWheel:(NSEvent*)event
{
  NSPoint point = [self convertPoint:event.locationInWindow fromView:nil];
  g_input.mouse.pos_x = point.x;
  g_input.mouse.pos_y = point.y;
  g_input.mouse.wheel_delta = static_cast<float>(event.scrollingDeltaY);
  send_input();
}

- (void)keyDown:(NSEvent*)event
{
  if (event.keyCode < 256)
    g_input.keymap.keys[event.keyCode].down = true;
  send_input();
}

- (void)keyUp:(NSEvent*)event
{
  if (event.keyCode < 256)
    g_input.keymap.keys[event.keyCode].down = false;
  send_input();
}

@end

@interface AppDelegate : NSObject <NSApplicationDelegate>
@end

@implementation AppDelegate

- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication*)sender
{
  return YES;
}

@end

int main(int argc, char** argv)
{
  @autoreleasepool
  {
    g_form = demo_create_ui();
    g_style = demo_create_style();

    [NSApplication sharedApplication];
    AppDelegate* delegate = [[AppDelegate alloc] init];
    [NSApp setDelegate:delegate];
    [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];

    NSRect frame = NSMakeRect(0, 0, 1000, 600);
    NSWindow* window = [[NSWindow alloc] initWithContentRect:frame
                                                   styleMask:NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskResizable
                                                     backing:NSBackingStoreBuffered
                                                       defer:NO];
    [window setTitle:@"Bad UI Demo"];
    [window center];

    BadUIView* view = [[BadUIView alloc] initWithFrame:frame];
    [window setContentView:view];
    [window makeKeyAndOrderFront:nil];

    [NSTimer scheduledTimerWithTimeInterval:(1.0 / 60.0)
                                     target:view
                                   selector:@selector(redraw)
                                   userInfo:nil
                                    repeats:YES];

    [NSApp activateIgnoringOtherApps:YES];
    [NSApp run];
  }

  return 0;
}
