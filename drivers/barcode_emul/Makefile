#
# makefile for Barcode Emulator
#
ifdef CONFIG_SEC_PRODUCT_8930
ifdef CONFIG_SEC_FPGA_442
obj-$(CONFIG_SEC_FPGA_442)	+= barcode_emul_ice4_8930_442.o
else
obj-$(CONFIG_SEC_FPGA)	+= barcode_emul_ice4_8930.o
endif
else
obj-$(CONFIG_SEC_FPGA)	+= barcode_emul_ice4.o
endif
