#
#  Makefile for the kernel Sensor device drivers.
#

# Object files in subdirectories
obj-$(CONFIG_INPUT_YAS_ACCELEROMETER)		+= accelerometer/
obj-$(CONFIG_INPUT_YAS_MAGNETOMETER)		+= geomagnetic/
obj-$(CONFIG_INPUT_YAS_ORIENTATION)		+= orientation/
obj-$(CONFIG_SENSORS_K3DH)		+= accelerometer_k2dh/
obj-y	+= optical/
obj-$(CONFIG_SENSORS_CORE)		+= sensors_core.o
obj-$(CONFIG_GRIP_SENSOR)		+= grip/