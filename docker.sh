#!/bin/sh

export $(cat /usr/src/capbot/.env | xargs)
/usr/src/capbot/build/capbot