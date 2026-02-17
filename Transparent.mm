#include <SFML/Graphics.hpp>
#import <Cocoa/Cocoa.h>
@implementation NSOpenGLView (Opaque)
-(BOOL)isOpaque
{
    return NO;
}
@end
// Global variables to hold the window and image state
static const sf::Image* g_windowShapeImage = nullptr;
static NSWindow* g_shapedWindow = nil;
static NSTimer* g_mouseTrackingTimer = nil;

// A helper class to run our 60FPS mouse tracking loop
@interface MouseTracker : NSObject
+ (void)checkMousePosition:(NSTimer*)timer;
@end

@implementation MouseTracker
+ (void)checkMousePosition:(NSTimer*)timer {
    if (!g_shapedWindow || !g_windowShapeImage) return;

    // Get the global mouse location on the screen
    NSPoint mouseLoc = [NSEvent mouseLocation];
    NSRect windowFrame = [g_shapedWindow frame];

    // Calculate the mouse point relative to the bottom-left of the window
    NSPoint localPoint = NSMakePoint(mouseLoc.x - windowFrame.origin.x, mouseLoc.y - windowFrame.origin.y);

    // Flip the Y-axis (macOS maps bottom-to-top, SFML images map top-to-bottom)
    int x = (int)localPoint.x;
    int y = (int)g_windowShapeImage->getSize().y - (int)localPoint.y - 1;

    // Default to transparent if the mouse is outside the window bounds entirely
    bool isTransparent = true; 

    if (x >= 0 && x < (int)g_windowShapeImage->getSize().x &&
        y >= 0 && y < (int)g_windowShapeImage->getSize().y) {

        const sf::Uint8* pixelData = g_windowShapeImage->getPixelsPtr();
        
        // Read the alpha channel (index 3 in RGBA)
        unsigned int index = (y * g_windowShapeImage->getSize().x * 4) + (x * 4) + 3;
        isTransparent = (pixelData[index] == 0);
    }

    // Dynamically tell the OS whether to ignore clicks based on the exact pixel
    [g_shapedWindow setIgnoresMouseEvents:isTransparent];
}
@end

bool setShape(sf::WindowHandle handle, const sf::Image& image)
{
    NSWindow* wnd = (NSWindow*)handle;

    g_windowShapeImage = &image;
    g_shapedWindow = wnd;

    // Make the window background transparent
    GLint opaque = 0;
    [[[wnd contentView] openGLContext] setValues:&opaque forParameter:NSOpenGLCPSurfaceOpacity];
    [wnd setBackgroundColor:[NSColor clearColor]];
    [wnd setOpaque:NO];
    [wnd setHasShadow:NO];

    // Start tracking the mouse position at ~60 times a second
    if (!g_mouseTrackingTimer) {
        g_mouseTrackingTimer = [NSTimer scheduledTimerWithTimeInterval:1.0/60.0
                                                                target:[MouseTracker class]
                                                              selector:@selector(checkMousePosition:)
                                                              userInfo:nil
                                                               repeats:YES];
        
        // Ensure the timer keeps firing even if the user is interacting with standard OS UI
        [[NSRunLoop currentRunLoop] addTimer:g_mouseTrackingTimer forMode:NSRunLoopCommonModes];
    }

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