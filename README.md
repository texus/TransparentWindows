Transparent Windows
===================

This repository contains an example of how to create non-rectangular semi-transparent windows on multiple operating systems with SFML (which is only used to load the image and display the result).
It loads an image with transparent parts, uses this image to set the shape of the window and draws the image on the window. The code also contains an opacity variable to set the opacity of the window.

The code works for Windows, Linux and Mac OS X.

Feel free to send pull requests to improve the code.


### Compiling

Linux:

    g++ Regions.cpp -lsfml-graphics -lsfml-window -lsfml-system -lX11 -lXext


Mac OS X:

    clang++ Regions.cpp Regions.mm -framework sfml-graphics -framework sfml-window -framework sfml-system -framework Cocoa

    In order to make the window non-rectangular a change inside SFML is needed. An isOpaque function which just returns NO has to be added to SFOpenGLView.


Windows:

    No special libraries are needed for the transparency. You should just compile Regions.cpp and link to sfml the way you do it for your own programs.