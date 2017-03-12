#unblurring module
LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE := unblurring
LOCAL_SRC_FILES := UnblurringCore.cpp
LOCAL_LDFLAGS += -ljnigraphics
LOCAL_CFLAGS += -fexceptions
LOCAL_LDLIBS := -llog -lz
include $(BUILD_SHARED_LIBRARY)


