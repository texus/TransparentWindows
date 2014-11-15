#include <SFML/Graphics.hpp>

#if defined (SFML_SYSTEM_WINDOWS)
    #include <windows.h>
#elif defined (SFML_SYSTEM_LINUX)
    #include <X11/Xlib.h>
    #include <X11/Xregion.h>
    #include <X11/extensions/shape.h>
    #undef None
#endif

int main()
{
	sf::Image backgroundImage;
	backgroundImage.loadFromFile("image.png");

	sf::RenderWindow window(sf::VideoMode(400, 300), "TGUI window", sf::Style::None);
	window.setPosition(sf::Vector2i((sf::VideoMode::getDesktopMode().width - backgroundImage.getSize().x) / 2,
	                                (sf::VideoMode::getDesktopMode().height - backgroundImage.getSize().y) / 2));

	sf::Texture backgroundTexture;
	sf::Sprite backgroundSprite;
	backgroundTexture.loadFromImage(backgroundImage);
	backgroundSprite.setTexture(backgroundTexture);

    bool fallback = true;
	const sf::Uint8* pixelData = backgroundImage.getPixelsPtr();

#if defined (SFML_SYSTEM_WINDOWS)
    fallback = false;

	HWND hWnd = window.getSystemHandle();
	HRGN hRegion = CreateRectRgn(0, 0, backgroundImage.getSize().x, backgroundImage.getSize().y);

	for (unsigned int y = 0; y < backgroundImage.getSize().y; y++) {
		for (unsigned int x = 0; x < backgroundImage.getSize().x; x++) {
		   if (pixelData[y * backgroundImage.getSize().x * 4 + x * 4 + 3] == 0) {
			 HRGN hRegionDest = CreateRectRgn(0, 0, 1, 1);
			 HRGN hRegionPixel = CreateRectRgn(x, y, x+1, y+1);
			 CombineRgn(hRegionDest, hRegion, hRegionPixel, RGN_XOR);
			 DeleteObject(hRegion);
			 DeleteObject(hRegionPixel);
			 hRegion = hRegionDest;
		   }
		}
	}
	SetWindowRgn(hWnd, hRegion, true);
	DeleteObject(hRegion);
#elif defined (SFML_SYSTEM_LINUX)
    Window wnd = window.getSystemHandle();
    Display* display = XOpenDisplay(NULL);

    int event_base;
    int error_base;
    if (XShapeQueryExtension(display, &event_base, &error_base))
    {
        fallback = false;

        Pixmap pixmap = XCreatePixmap(display, wnd, backgroundImage.getSize().x, backgroundImage.getSize().y, 1);
        GC gc = XCreateGC(display, pixmap, 0, NULL);

	    XSetForeground(display, gc, 1);
	    XFillRectangle(display, pixmap, gc, 0, 0, backgroundImage.getSize().x, backgroundImage.getSize().y);
	    XSetForeground(display, gc, 0);

	    for (unsigned int y = 0; y < backgroundImage.getSize().y; y++) {
		    for (unsigned int x = 0; x < backgroundImage.getSize().x; x++) {
		        if (pixelData[y * backgroundImage.getSize().x * 4 + x * 4 + 3] == 0) {
		            XFillRectangle(display, pixmap, gc, x, y, 1, 1);
		        }
		    }
		}

		XShapeCombineMask(display, wnd, ShapeBounding, 0, 0, pixmap, ShapeSet);

	    XFreePixmap(display, pixmap);
	    XFreeGC(display, gc);

	    XFlush(display);
    }
#endif

    // Execute this code on unsupported OS or when linux display doesn't support shape extention
    if (fallback)
    {
        // Should something happen when the window can't get the desired shape?
    }

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();
		window.draw(backgroundSprite);
        window.display();
    }

    return 0;
}

