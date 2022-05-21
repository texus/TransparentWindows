#include <SFML/Graphics.hpp>

#if defined (SFML_SYSTEM_WINDOWS)
    #include <windows.h>

    bool setShape(HWND hWnd, const sf::Image& image)
    {
        const sf::Uint8* pixelData = image.getPixelsPtr();

        // Create a region with the size of the entire window
        HRGN hRegion = CreateRectRgn(0, 0, image.getSize().x, image.getSize().y);

        // Loop over the pixels in the image and for each pixel where the alpha component equals 0,
        // we will remove that pixel from the region.
        // As an optimization, we will combine adjacent transparent pixels on the same row and
        // remove them together, instead of using "CreateRectRgn(x, y, x+1, y+1)" to define
        // a region for each transparent pixel individually.
        bool transparentPixelFound = false;
        unsigned int rectLeft = 0;
        for (unsigned int y = 0; y < image.getSize().y; y++)
        {
            for (unsigned int x = 0; x < image.getSize().x; x++)
            {
                const bool isTransparentPixel = (pixelData[y * image.getSize().x * 4 + x * 4 + 3] == 0);
                if (isTransparentPixel && !transparentPixelFound)
                {
                    transparentPixelFound = true;
                    rectLeft = x;
                }
                else if (!isTransparentPixel && transparentPixelFound)
                {
                    HRGN hRegionPixel = CreateRectRgn(rectLeft, y, x, y+1);
                    CombineRgn(hRegion, hRegion, hRegionPixel, RGN_XOR);
                    DeleteObject(hRegionPixel);
                    transparentPixelFound = false;
                }
            }

            if (transparentPixelFound)
            {
                HRGN hRegionPixel = CreateRectRgn(rectLeft, y, image.getSize().x, y+1);
                CombineRgn(hRegion, hRegion, hRegionPixel, RGN_XOR);
                DeleteObject(hRegionPixel);
                transparentPixelFound = false;
            }
        }

        SetWindowRgn(hWnd, hRegion, true);
        DeleteObject(hRegion);
        return true;
    }

    bool setTransparency(HWND hWnd, unsigned char alpha)
    {
        SetWindowLong(hWnd, GWL_EXSTYLE, GetWindowLong(hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
        SetLayeredWindowAttributes(hWnd, 0, alpha, LWA_ALPHA);
        return true;
    }

#elif defined (SFML_SYSTEM_LINUX)
    #include <X11/Xatom.h>
    #include <X11/extensions/shape.h>

    bool setShape(Window wnd, const sf::Image& image)
    {
        Display* display = XOpenDisplay(NULL);

        // Setting the window shape requires the XShape extension
        int event_base;
        int error_base;
        if (!XShapeQueryExtension(display, &event_base, &error_base))
        {
            XCloseDisplay(display);
            return false;
        }

        const sf::Uint8* pixelData = image.getPixelsPtr();

        // Create a black and white pixmap that has the size of the window
        Pixmap pixmap = XCreatePixmap(display, wnd, image.getSize().x, image.getSize().y, 1);
        GC gc = XCreateGC(display, pixmap, 0, NULL);

        // Make the entire pixmap white
        XSetForeground(display, gc, 1);
        XFillRectangle(display, pixmap, gc, 0, 0, image.getSize().x, image.getSize().y);

        // Loop over the pixels in the image and change the color of the pixmap to black
        // for each pixel where the alpha component equals 0.
        // As an optimization, we will combine adjacent transparent pixels on the same row and
        // draw them together, instead of calling "XFillRectangle(display, pixmap, gc, x, y, 1, 1)"
        // for each transparent pixel individually.
        XSetForeground(display, gc, 0);
        bool transparentPixelFound = false;
        unsigned int rectLeft = 0;
        for (unsigned int y = 0; y < image.getSize().y; y++)
        {
            for (unsigned int x = 0; x < image.getSize().x; x++)
            {
                const bool isTransparentPixel = (pixelData[y * image.getSize().x * 4 + x * 4 + 3] == 0);
                if (isTransparentPixel && !transparentPixelFound)
                {
                    transparentPixelFound = true;
                    rectLeft = x;
                }
                else if (!isTransparentPixel && transparentPixelFound)
                {
                    XFillRectangle(display, pixmap, gc, rectLeft, y, x - rectLeft, 1);
                    transparentPixelFound = false;
                }
            }

            if (transparentPixelFound)
            {
                XFillRectangle(display, pixmap, gc, rectLeft, y, image.getSize().x - rectLeft, 1);
                transparentPixelFound = false;
            }
        }

        // Use the black and white pixmap to define the shape of the window. All pixels that are
        // white will be kept, while all black pixels will be clipped from the window.
        XShapeCombineMask(display, wnd, ShapeBounding, 0, 0, pixmap, ShapeSet);

        // Free resources
        XFreeGC(display, gc);
        XFreePixmap(display, pixmap);
        XFlush(display);
        XCloseDisplay(display);
        return true;
    }

    bool setTransparency(Window wnd, unsigned char alpha)
    {
        Display* display = XOpenDisplay(NULL);
        unsigned long opacity = (0xffffffff / 0xff) * alpha;
        Atom property = XInternAtom(display, "_NET_WM_WINDOW_OPACITY", false);
        if (property != None)
        {
            XChangeProperty(display, wnd, property, XA_CARDINAL, 32, PropModeReplace, (unsigned char*)&opacity, 1);
            XFlush(display);
            XCloseDisplay(display);
            return true;
        }
        else
        {
            XCloseDisplay(display);
            return false;
        }
    }

    #undef None // None conflicts with SFML
#elif defined (SFML_SYSTEM_MACOS)
    bool setShape(sf::WindowHandle handle, const sf::Image& image);
    bool setTransparency(sf::WindowHandle handle, unsigned char alpha);
#else
    bool setShape(sf::WindowHandle handle, const sf::Image& image)
    {
        return false;
    }

    bool setTransparency(sf::WindowHandle handle, unsigned char alpha)
    {
        return false;
    }
#endif

int main()
{
    // Change this to the wanted transparency
    const unsigned char opacity = 185;

    // Load an image with transparent parts that will define the shape of the window
    sf::Image backgroundImage;
    backgroundImage.loadFromFile("image.png");

    // Create the window and center it on the screen
    sf::RenderWindow window(sf::VideoMode(backgroundImage.getSize().x, backgroundImage.getSize().y, 32), "Transparent Window", sf::Style::None);
    window.setPosition(sf::Vector2i((sf::VideoMode::getDesktopMode().width - backgroundImage.getSize().x) / 2,
                                    (sf::VideoMode::getDesktopMode().height - backgroundImage.getSize().y) / 2));

    // These functions return false on an unsupported OS or when it is not supported on linux (e.g. display doesn't support shape extention)
    setShape(window.getSystemHandle(), backgroundImage);
    setTransparency(window.getSystemHandle(), opacity);

    // We will also draw the image on the window instead of just showing an empty window with the wanted shape
    sf::Texture backgroundTexture;
    sf::Sprite backgroundSprite;
    backgroundTexture.loadFromImage(backgroundImage);
    backgroundSprite.setTexture(backgroundTexture);

    // Main loop to display the image while the window is open (pressing the escape key to close the window)
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed || (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape))
                window.close();
        }

        window.clear(sf::Color::Transparent);
        window.draw(backgroundSprite);
        window.display();
    }

    return 0;
}
