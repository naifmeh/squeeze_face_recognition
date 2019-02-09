#!/bin/sh

echo SQUEEZEPATH="$( cd "$(dirname "$0")" ; pwd -P )" >> ~/.bashrc
eval $(. ~/.bashrc)
