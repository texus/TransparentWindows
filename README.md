Transparent Windows
===================

This repository contains an example of how to create non-rectangular semi-transparent windows on multiple operating systems with SFML (which is only used to load the image and display the result).
It loads an image with transparent parts, uses this image to set the shape of the window and draws the image on the window. The code also contains an opacity variable to set the opacity of the window.

The code works for Windows, Linux and Mac OS X.

Feel free to send pull requests to improve the code.


### Compiling

Linux:

    g++ Transparent.cpp -lsfml-graphics -lsfml-window -lsfml-system -lX11 -lXext


Mac OS X:

    clang++ Transparent.cpp Transparent.mm -framework sfml-graphics -framework sfml-window -framework sfml-system -framework Cocoa


Windows:

    No special libraries are needed for the transparency. You should just compile Transparent.cpp and link to sfml the way you do it for your own programs.


### Translucent windows

This code works by using a fully opaque image and then applying an alpha value over all the visible pixels, it does not support per-pixel alpha. If you want to use a transparent image and set the alpha values of the window like in the image then check the following resources:

- Windows: I wrote an [implementation for Windows](https://gist.github.com/texus/31676aba4ca774b1298e1e15133b8141) using the UpdateLayeredWindow function.

- Linux: I found [this gist](https://gist.github.com/je-so/903479) which can be changed to use per-pixel alpha. Adapting it to use SFML only worked when creating the sf::RenderWindow from the created Xlib Window, but not the other way around. If you manage to get it working on linux with a window created by SFML then please let me know.

- macOS: The code in this repository already supports translucent windows for macOS! You can just replace the image with a transparent one and it will work.
