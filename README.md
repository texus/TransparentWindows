Transparent Windows
===================

This repository contains an example of how to create non-rectangular semi-transparent windows on multiple operating systems with SFML (which is only used to load the image and display the result).
It loads an image with transparent parts, uses this image to set the shape of the window and draws the image on the window. The code also contains an opacity variable to set the opacity of the window.

Currently the code only works on Windows and Linux, but I hope to add OS X in the future.

Feel free to send pull requests to improve the code.


### Compiling

On Linux:

    g++ Regions.cpp -lsfml-graphics -lsfml-window -lsfml-system -lX11 -lXext

On Windows:

    You should link to sfml the way you do it for your own programs. No special libraries are needed for the transparency.
