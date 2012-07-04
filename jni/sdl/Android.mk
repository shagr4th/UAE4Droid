LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := sdl

ifndef SDL_JAVA_PACKAGE_PATH
$(error Please define SDL_JAVA_PACKAGE_PATH to the path of your Java package with dots replaced with underscores, for example "com_example_SanAngeles")
endif

LOCAL_CFLAGS := -I$(LOCAL_PATH)/include \
	-DSDL_JAVA_PACKAGE_PATH=$(SDL_JAVA_PACKAGE_PATH) \
	-DSDL_CURDIR_PATH=\"$(SDL_CURDIR_PATH)\" \
	-DSDL_TRACKBALL_KEYUP_DELAY=$(SDL_TRACKBALL_KEYUP_DELAY)
	
LOCAL_CFLAGS += -DNO_MALLINFO

ifeq ($(TARGET_ARCH),x86)
LOCAL_CFLAGS += -DLACKS_FCNTL_H -DLACKS_UNISTD_H
endif

SDL_SRCS := \
	src/*.c \
	src/audio/*.c \
	src/cdrom/*.c \
	src/cpuinfo/*.c \
	src/events/*.c \
	src/file/*.c \
	src/joystick/*.c \
	src/stdlib/*.c \
	src/thread/*.c \
	src/timer/*.c \
	src/video/*.c \
	src/audio/android/*.c \
	src/video/android/*.c \
	src/joystick/android/*.c \
	src/cdrom/dummy/*.c \
	src/thread/pthread/*.c \
	src/timer/unix/*.c \
	src/loadso/dummy/*.c \

LOCAL_CPP_EXTENSION := .cpp

ifeq ($(TARGET_ARCH),arm)
LOCAL_ARM_MODE := arm
endif


# Note this simple makefile var substitution, you can find even simpler examples in different Android projects
LOCAL_SRC_FILES := $(foreach F, $(SDL_SRCS), $(addprefix $(dir $(F)),$(notdir $(wildcard $(LOCAL_PATH)/$(F)))))

#-lGLESv1_CM 
LOCAL_LDLIBS := -ldl -llog

include $(BUILD_STATIC_LIBRARY)

