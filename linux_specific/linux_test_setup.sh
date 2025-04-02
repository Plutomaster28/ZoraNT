#!/bin/bash

# Update the package database and upgrade the system
echo "Updating package database and upgrading the system..."
sudo apt update && sudo apt upgrade -y
if [ $? -ne 0 ]; then
    echo "Failed to update package database and upgrade the system."
    exit 1
fi

# Install the compiler and development tools
echo "Installing the necessary compiler and development tools..."
sudo apt install -y build-essential gcc
if [ $? -ne 0 ]; then
    echo "Failed to install the compiler and development tools."
    exit 1
fi

# Install GTK and its dependencies
echo "Installing GTK and its dependencies..."
sudo apt install -y libgtk-3-dev
if [ $? -ne 0 ]; then
    echo "Failed to install GTK and its dependencies."
    exit 1
fi

# Install GStreamer and its plugins
echo "Installing GStreamer and its plugins..."
sudo apt install -y gstreamer1.0-tools gstreamer1.0-plugins-base gstreamer1.0-plugins-good gstreamer1.0-plugins-bad gstreamer1.0-plugins-ugly
if [ $? -ne 0 ]; then
    echo "Failed to install GStreamer and its plugins."
    exit 1
fi

# Install other necessary libraries
echo "Installing other necessary libraries..."
sudo apt install -y libglib2.0-dev libgdk-pixbuf2.0-dev libcairo2-dev libpango1.0-dev libatk1.0-dev git
if [ $? -ne 0 ]; then
    echo "Failed to install other necessary libraries."
    exit 1
fi

# Copy the etc directory to the system's /etc directory
echo "Copying etc directory to the system's /etc directory..."
sudo cp -r ./ZoraNT/Sys404/etc/* /etc/
if [ $? -ne 0 ]; then
    echo "Failed to copy etc directory."
    exit 1
fi

# Copy the theme directory into the system's /usr/share/themes directory
echo "Copying theme directory to the system's /usr/share/themes directory..."
sudo cp -r ./ZoraNT/Sys404/themes/* /usr/share/themes/
if [ $? -ne 0 ]; then
    echo "Failed to copy theme directory."
    exit 1
fi

# Compilation command
echo "Compiling the application..."
gcc -o ZoraNT64 new_desktopmain_nt.c $(pkg-config --cflags --libs gtk+-3.0 gstreamer-1.0 json-glib-1.0)
if [ $? -ne 0 ]; then
    echo "Failed to compile the application."
    exit 1
fi

echo "All required packages have been installed, the directories have been copied, and the application has been compiled."