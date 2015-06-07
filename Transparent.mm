#include <SFML/Graphics.hpp>
#import <Cocoa/Cocoa.h>

@implementation NSOpenGLView (Opaque)
-(BOOL)isOpaque
{
    return NO;
}
@end

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