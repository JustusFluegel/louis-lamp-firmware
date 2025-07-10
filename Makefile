all : flash

TARGET:=test-firmware

ADDITIONAL_C_FILES = touch_sense.c brightness_controller.c
TARGET_MCU?=CH32V003
include ../ch32fun/ch32fun/ch32fun.mk

flash : cv_flash
clean : cv_clean
