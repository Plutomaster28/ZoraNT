#!/bin/bash

# This function will install libraries that provide extra features to Zora but are not essential.
echo "Installing 3D Graphics Library..."
pacman -S --noconfirm mingw-w64-ucrt-x86_64-glfw mingw-w64-ucrt-x86_64-glew mingw-w64-ucrt-x86_64-glm mingw-w64-ucrt-x86_64-freeglut
if [ $? -ne 0 ]; then
    echo "Failed to install OpenGL and its plugins."
    exit 1
fi

echo "Compiling complimentary demo..."
gcc -o test_demo_3D.exe tst3D.c -lfreeglut -lglu32 -lopengl32
if [ $? -ne 0 ]; then
    echo "Failed to compile the application."
    exit 1
fi

echo "All done!"