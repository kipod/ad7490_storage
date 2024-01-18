#!/bin/bash

# reset pin
gpio mode 0 out
# boot pin
gpio mode 1 out

# push boot button
gpio write 1 0
# push reset button
gpio write 0 0
sleep 1
# release reset button
gpio write 0 1
sleep 2
# release boot button
gpio write 1 1

