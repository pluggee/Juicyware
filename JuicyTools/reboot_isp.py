#!/usr/bin/python

""" This script toggles the reset pin """

import RPi.GPIO as GPIO
from time import sleep


GPIO.setmode(GPIO.BOARD)

# pin numbers are in BCM notation
# BCM17 is connector pin 11
RSTpin = 11
# BCM27 is connector pin 13
ISPpin = 13

#GPIO.cleanup()
GPIO.setup(RSTpin, GPIO.OUT)
GPIO.setup(ISPpin, GPIO.OUT)

GPIO.output(RSTpin, GPIO.HIGH)
GPIO.output(ISPpin, GPIO.LOW)
sleep(0.1)
GPIO.output(RSTpin, GPIO.LOW)
sleep(0.1)
GPIO.output(ISPpin, GPIO.LOW)
sleep(0.1)
GPIO.output(RSTpin, GPIO.HIGH)
sleep(0.1)
GPIO.output(ISPpin, GPIO.HIGH)
sleep(0.1)

GPIO.setup(RSTpin, GPIO.IN)
GPIO.setup(ISPpin, GPIO.IN)
