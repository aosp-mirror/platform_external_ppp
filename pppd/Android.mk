ifeq ($(TARGET_ARCH),arm)

LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES:= \
	main.c \
	magic.c \
	fsm.c \
	lcp.c \
	ipcp.c \
	upap.c \
	chap-new.c \
	md5.c \
	ccp.c \
	ecp.c \
	ipxcp.c \
	auth.c \
	options.c \
	sys-linux.c \
	md4.c \
	chap_ms.c \
	demand.c \
	utils.c \
	tty.c \
	eap.c \
	chap-md5.c

LOCAL_SHARED_LIBRARIES := \
	libcutils

LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/include

LOCAL_CFLAGS := -DANDROID_CHANGES

LOCAL_MODULE_PATH := $(TARGET_OUT_OPTIONAL_EXECUTABLES)
LOCAL_MODULE_TAGS := eng
LOCAL_MODULE:= pppd

include $(BUILD_EXECUTABLE)

endif
