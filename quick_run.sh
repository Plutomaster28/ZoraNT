#!/bin/bash

# Check if the executable exists
if [ -f "./ZoraNT.exe" ]; then
    echo "Executable found. Running ZoraNT..."
    ./ZoraNT.exe
else
    echo "Executable not found. Please make sure ZoraNT.exe is in the current directory."
    exit 1
fi
