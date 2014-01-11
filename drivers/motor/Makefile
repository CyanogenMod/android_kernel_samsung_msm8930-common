#
#  makefile for tspdrv kernel module and controlled by Haptic PWM
#

obj-$(CONFIG_VIBETONZ) += vibrator.o
obj-$(CONFIG_MOTOR_DRV_MAX77693)	+= max77693_haptic.o

ifeq ($(CONFIG_MOTOR_DRV_TSP5000), y)
vibrator-objs +=tspdrv_TSP5000.o
else
vibrator-objs +=tspdrv.o
endif

# Makefile for Vibrator controlled by PMIC Regulator.
obj-$(CONFIG_MSM_VIBRATOR) += msm_vibrator.o