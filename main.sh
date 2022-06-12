#!/bin/bash

export $(cat .env | xargs)
# -g -O0 for debugging with gdb
{
    find . -name *.cpp | xargs echo
    find . -name *.cpp | xargs clang++ -std=c++17 -o bot -ldpp -Wall -I include
} && {
    ./bot
}