#!/bin/bash

export $(cat .env | xargs)
# -g -O0 for debugging with gdb
{
    find . -name *.cpp | xargs echo
    find . -name *.cpp | xargs clang++ -std=c++17 -o bot -Wall -I include -ldpp
} && {
    ./bot
}

# docker
# docker build -t NAME:BRANCH .
# docker run -it NAME:BRANCH