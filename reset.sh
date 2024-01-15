#!/bin/bash

# reset pin
gpio mode 0 out
# boot pin
gpio mode 1 out

# 
gpio write 0 0
sleep 1
gpio write 0 1

