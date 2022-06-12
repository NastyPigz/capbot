#!/bin/sh

export $(cat .env | xargs)

./build/capbot