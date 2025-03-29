#!/bin/bash

# Script to compile ZoraNT

# Set the compiler path to use the ucrt64 compiler
export PATH="/ucrt64/bin:$PATH"

# Variables
OUTPUT="ZoraNT.exe"
SOURCE="new_desktopmain_nt.c"
FLAGS=$(pkg-config --cflags --libs gtk+-3.0 gstreamer-1.0 json-glib-1.0)

# Compilation command
echo "Compiling the application..."
gcc -o "$OUTPUT" "$SOURCE" $FLAGS
if [ $? -ne 0 ]; then
    echo "Failed to compile the application."
    exit 1
else
    echo "Compilation successful! Output: $OUTPUT"
fi
