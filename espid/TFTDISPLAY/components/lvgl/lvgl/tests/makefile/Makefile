LVGL_PATH := $(CURDIR)/../..
include ../C:\Users\YUVA\Desktop\espid\TFTDISPLAY\components\lvgl\lvgl\lvgl.h\lvgl.mk

CSRCS += test.c
CFLAGS += -DLV_CONF_SKIP=1 -DLV_BUILD_TEST=1 -DLV_USE_DEV_VERSION -I$(LVGL_PATH)/..
COBJS := $(patsubst %.c, %.o, $(CSRCS))

test_file: $(COBJS)
	$(CC) -o $@ $^
