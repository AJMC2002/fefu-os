#!/bin/bash
set -e

echo "Updating source code..."
git pull origin main

BUILD_DIR=build
if [ ! -d "$BUILD_DIR" ]; then
    mkdir "$BUILD_DIR"
fi

cd "$BUILD_DIR"
cmake ..
make

echo "Build successful. Running the program..."
./HelloWorld.o
