#!/bin/bash

# This script applies new themes for the user without them having to run initial setup all over again.

# Copy the etc directory to the ucrt64 environment
echo "Copying etc directory to the ucrt64 environment..."
cp -r etc/* /ucrt64/etc/
if [ $? -ne 0 ]; then
    echo "Failed to copy etc directory."
    exit 1
fi

# Copy the theme directory into the share/themes directory in the ucrt64 environment
echo "Copying theme directory to the ucrt64 environment..."
cp -r themes/* /ucrt64/share/themes/
if [ $? -ne 0 ]; then
    echo "Failed to copy theme directory."
    exit 1
fi

echo "Theme has been applied!"