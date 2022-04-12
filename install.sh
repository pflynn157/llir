#!/bin/bash

if [[ ! -d ./build ]]; then
    echo "Error: Please build the library!"
    exit 1
fi

echo "Installing..."

sudo install build/src/libllir.so /usr/lib
sudo install build/compiler/llircc /usr/bin

if [[ ! -d /usr/include/llir ]] ; then
    sudo mkdir -p /usr/include/llir
    sudo mkdir -p /usr/include/llir/amd64
fi

sudo cp src/*.hpp /usr/include/llir
sudo cp src/amd64/*.hpp /usr/include/llir/amd64

sudo ldconfig

echo ""
echo "Done"
echo ""

