#!/bin/bash

export $(cat .env | xargs)
# -g -O0 for debugging with gdb
{
    find ./src -name *.cpp | xargs echo
    g++ -std=c++17 -I include -Wall $(find ./src -name *.cpp) -ldpp -o bot
} && {
    ./bot
}