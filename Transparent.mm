#include <SFML/Graphics.hpp>
#import <Cocoa/Cocoa.h>

@implementation NSOpenGLView (Opaque)
-(BOOL)isOpaque
{
    return NO;
}
@end

// While this functions will set a custom shape for the window, mouse events may not pass through
// the clipped parts of the window to the windows behind it. If you want this, then you can
// check out https://gist.github.com/iman-zamani/519e241916aeaa0c26e9f4787884f5bf
bool setShape(sf::WindowHandle handle, const sf::Image& image)
{
    NSWindow* wnd = (NSWindow*)handle;

    GLint opaque = 0;
    [[[wnd contentView] openGLContext] setValues:&opaque forParameter:NSOpenGLCPSurfaceOpacity];
    [wnd setBackgroundColor:[NSColor clearColor]];
    [wnd setOpaque:NO];

    return true;
}

bool setTransparency(sf::WindowHandle handle, unsigned char alpha)
{
    NSWindow* wnd = (NSWindow*)handle;

    CGFloat opacity = alpha / 255.0f;
    [wnd setAlphaValue:opacity];
    [wnd setOpaque:NO];

    return true;
}
