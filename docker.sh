#!/bin/sh

export $(cat .env | xargs)
{
    find -name *.cpp | xargs g++ -std=c++17 -o bot -ldpp -Wall -I include
} && {
    ./bot
}