LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := uaecyclone

CG_SUBDIRS := \
src \

LOCAL_C_INCLUDES := $(LOCAL_PATH)/include $(LOCAL_PATH)/vkbd $(LOCAL_PATH)/../sdl/include

LOCAL_CFLAGS := $(foreach D, $(CG_SUBDIRS), -I$(CG_SRCDIR)/$(D)) \
				-I$(LOCAL_PATH)/../sdl/include \
				-I$(LOCAL_PATH)/../sdl_mixer \

# -DUSE_FAME_CORE_C 
# m68k/fame/famec.cpp m68k/fame/m68k_intrf.cpp
# -DDEBUG_M68K 

# -DDEBUG_CUSTOM -DDEBUG_INTERRUPTS -DDEBUG_UAE4ALL -DDEBUG_GFX 

# -fomit-frame-pointer -Wno-unused -Wno-format -DUSE_UNDERSCORE -fno-exceptions -DUNALIGNED_PROFITABLE -DOPTIMIZED_FLAGS -DSHM_SUPPORT_LINKS=0 

#  -DUSE_ZFILE -DNO_SOUND -DDEBUG_SOUND -DDEBUG_UAE4ALL 

#  -DUSE_CYCLONE_CORE -DUSE_CYCLONE_MEMHANDLERS 

LOCAL_CFLAGS     += -Wno-psabi
LOCAL_CFLAGS	 += -DSDL_JAVA_PACKAGE_PATH=$(SDL_JAVA_PACKAGE_PATH) -DUSE_SDL -DOS_WITHOUT_MEMORY_MANAGEMENT -DANDROID -DOS_ANDROID -DFAME_INTERRUPTS_PATCH 
LOCAL_CFLAGS     += -DUSE_FAME_CORE -DUSE_CYCLONE_CORE -DUSE_CYCLONE_MEMHANDLERS 

# m68k/m68k_cmn_intrf.cpp  m68k/cyclone/cyclone.S m68k/cyclone/m68k_intrf.cpp m68k/cyclone/memhandlers.S
# m68k/fame/famec.cpp m68k/fame/m68k_intrf.cpp

LOCAL_SRC_FILES  := audio.cpp autoconf.cpp blitfunc.cpp blittable.cpp blitter.cpp cia.cpp custom.cpp disk.cpp drawing.cpp ersatz.cpp gfxutil.cpp keybuf.cpp main.cpp md-support.cpp memory.cpp missing.cpp od-joy.cpp savedisk.cpp sdlgfx.cpp sound_android.cpp writelog.cpp zfile.cpp gui.cpp vkbd/vkbd.cpp
ifeq ($(TARGET_ARCH),arm)
LOCAL_SRC_FILES += memcpy.S memset.S
endif
LOCAL_SRC_FILES  += m68k/cyclone/m68k_intrf.cpp m68k/m68k_cmn_intrf.cpp  
ifeq ($(TARGET_ARCH),arm)
LOCAL_SRC_FILES += m68k/cyclone/cyclone.S m68k/cyclone/memhandlers.S
endif
#LOCAL_SRC_FILES  += m68k/uae/newcpu.cpp m68k/uae/readcpu.cpp m68k/uae/cpudefs.cpp m68k/uae/fpp.cpp m68k/uae/cpustbl.cpp m68k/uae/cpuemu.cpp

LOCAL_ARM_MODE := arm

#LOCAL_CFLAGS	 +=  -ffast-math
#LOCAL_CFLAGS	 +=  -O3
#LOCAL_CFLAGS	 +=  -fstrict-aliasing -mstructure-size-boundary=32 -fexpensive-optimizations
#LOCAL_CFLAGS	 +=  -fweb -frename-registers -fomit-frame-pointer
#LOCAL_CFLAGS	 +=  -falign-functions=32 -falign-loops -falign-labels -falign-jumps
#LOCAL_CFLAGS	 +=  -finline -finline-functions -fno-builtin

# sdl_mixer tremor
LOCAL_STATIC_LIBRARIES :=  sdl

LOCAL_LDLIBS	+= -llog -lz

include $(BUILD_SHARED_LIBRARY)
