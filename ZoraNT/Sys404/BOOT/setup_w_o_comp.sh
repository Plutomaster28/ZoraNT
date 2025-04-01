#!/bin/bash
# Compiler has been excluded; this script is mostly for testing purposes, such as removing or adding new commands and features.
# This script is intended to be run in a MSYS2 environment on Windows.

# Update the package database and core system packages
echo "Updating package database and core system packages..."
pacman -Syu --noconfirm
if [ $? -ne 0 ]; then
    echo "Failed to update package database and core system packages."
    exit 1
fi

# Install the toolchain and base development packages
echo "Installing toolchain and base development packages..."
pacman -S --noconfirm mingw-w64-ucrt-x86_64-toolchain base-devel
if [ $? -ne 0 ]; then
    echo "Failed to install toolchain and base development packages."
    exit 1
fi

# Install GTK and its dependencies
echo "Installing GTK and its dependencies..."
pacman -S --noconfirm mingw-w64-ucrt-x86_64-gtk3
if [ $? -ne 0 ]; then
    echo "Failed to install GTK and its dependencies."
    exit 1
fi

# Install GStreamer and its plugins
echo "Installing GStreamer and its plugins..."
pacman -S --noconfirm mingw-w64-ucrt-x86_64-gstreamer mingw-w64-ucrt-x86_64-gst-plugins-base mingw-w64-ucrt-x86_64-gst-plugins-good mingw-w64-ucrt-x86_64-gst-plugins-bad mingw-w64-ucrt-x86_64-gst-plugins-ugly
if [ $? -ne 0 ]; then
    echo "Failed to install GStreamer and its plugins."
    exit 1
fi

# Install other necessary libraries
echo "Installing other necessary libraries..."
pacman -S --noconfirm mingw-w64-ucrt-x86_64-glib2 mingw-w64-ucrt-x86_64-gdk-pixbuf2 mingw-w64-ucrt-x86_64-cairo mingw-w64-ucrt-x86_64-pango mingw-w64-ucrt-x86_64-atk git
if [ $? -ne 0 ]; then
    echo "Failed to install other necessary libraries."
    exit 1
fi

# Set the compiler path to use the ucrt64 compiler
export PATH="/ucrt64/bin:$PATH"

# Copy the etc directory to the ucrt64 environment
echo "Copying etc directory to the ucrt64 environment..."
cp -r ../../../ZoraNT/Sys404/etc/* /ucrt64/etc/
if [ $? -ne 0 ]; then
    echo "Failed to copy etc directory."
    exit 1
fi

# Copy the theme directory into the share/themes directory in the ucrt64 environment
echo "Copying theme directory to the ucrt64 environment..."
cp -r ../../../ZoraNT/Sys404/themes/* /ucrt64/share/themes/
if [ $? -ne 0 ]; then
    echo "Failed to copy theme directory."
    exit 1
fi

# Compilation command
echo "Compiling the application..."
gcc -o ../../../ZoraNT.exe ../../../new_desktopmain_nt.c $(pkg-config --cflags --libs gtk+-3.0 gstreamer-1.0 json-glib-1.0)
if [ $? -ne 0 ]; then
    echo "Failed to compile the application."
    exit 1
fi

echo "All required packages have been installed, the directories have been copied, and the application has been compiled."
