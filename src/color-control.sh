#!/bin/sh
# Helper to upload a program to all available MCUs.
color-control-compile < $1 | tee /dev/ttyACM* > /dev/null
