#!/bin/sh

#installing dlib
wget https://github.com/davisking/dlib/releases/download/v18.16/dlib-18.16.tar.bz2

mkdir -p ~/src
tar xf dlib-18.16.tar.bz2
mv dlib-18.16.tar.bz2 ~/src
cv ~/src
cd dlib-18.16/python_examples
mkdir build
cd build
cmake ../../tools/python
cmake --build . --config Release
sudo cp dlib.so /usr/local/lib/python2.7/dist-packages

#torch
git clone https://github.com/torch/distro.git ~/torch --recursive
cd ~/torch; bash install-deps;
./install.sh

source ~/.bashrc

for NAME in dpnn nn optim optnet csvigo cutorch cunn fblualib torchx tds; do luarocks install $NAME; done

