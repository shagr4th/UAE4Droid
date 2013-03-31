LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := uae2

CG_SUBDIRS := \
src

LOCAL_C_INCLUDES := $(LOCAL_PATH)/include $(LOCAL_PATH)/vkbd $(LOCAL_PATH)/gp2x $(LOCAL_PATH)/gp2x/menu $(LOCAL_PATH)/../sdl/include

LOCAL_CFLAGS := $(foreach D, $(CG_SUBDIRS), -I$(CG_SRCDIR)/$(D)) \
				-I$(LOCAL_PATH)/../sdl/include \
				-I$(LOCAL_PATH)/../sdl_mixer \


LOCAL_CFLAGS     += -Wno-psabi
LOCAL_CFLAGS	 += -DSDL_JAVA_PACKAGE_PATH=$(SDL_JAVA_PACKAGE_PATH) -DANDROID -DOS_ANDROID
LOCAL_CFLAGS	 += -fomit-frame-pointer -Wno-unused -Wno-format -DUSE_SDL -DGCCCONSTFUNC="__attribute__((const))" -DUSE_UNDERSCORE -fno-exceptions -DUNALIGNED_PROFITABLE -DOPTIMIZED_FLAGS -DSHM_SUPPORT_LINKS=0 -DOS_WITHOUT_MEMORY_MANAGEMENT

LOCAL_SRC_FILES  := audio.cpp autoconf.cpp blitfunc.cpp blittable.cpp blitter.cpp cfgfile.cpp cia.cpp savedisk.cpp savestate.cpp compiler.cpp custom.cpp disk.cpp drawing.cpp ersatz.cpp expansion.cpp filesys.cpp fsdb.cpp fsdb_unix.cpp fsusage.cpp gfxutil.cpp hardfile.cpp keybuf.cpp main.cpp memory.cpp missing.cpp native2amiga.cpp gui.cpp od-joy.cpp scsi-none.cpp sdlgfx.cpp writelog.cpp zfile.cpp vkbd/vkbd.cpp sound_android.cpp

LOCAL_ARM_MODE := arm

LOCAL_CFLAGS +=  -O3 
LOCAL_CFLAGS += -fstrict-aliasing -fexpensive-optimizations
LOCAL_CFLAGS += -fweb -frename-registers -fomit-frame-pointer
LOCAL_CFLAGS += -falign-functions=32 -falign-loops -falign-labels -falign-jumps
LOCAL_CFLAGS += -finline -finline-functions -fno-builtin
ifeq ($(TARGET_ARCH),arm)
LOCAL_CFLAGS += -mstructure-size-boundary=32
endif

LOCAL_CFLAGS += -DUSE_AUTOCONFIG
LOCAL_CFLAGS += -DUSE_ZFILE
LOCAL_CFLAGS += -DFAME_INTERRUPTS_PATCH
# Turrican3 becomes unstable if this is not enabled
LOCAL_CFLAGS += -DSAFE_MEMORY_ACCESS

#Fame:
LOCAL_CFLAGS += -DUSE_FAME_CORE -DUSE_FAME_CORE_C -DFAME_IRQ_CLOCKING -DFAME_CHECK_BRANCHES -DFAME_EMULATE_TRACE -DFAME_DIRECT_MAPPING -DFAME_BYPASS_TAS_WRITEBACK -DFAME_ACCURATE_TIMING -DFAME_GLOBAL_CONTEXT -DFAME_FETCHBITS=8 -DFAME_DATABITS=8 -DFAME_NO_RESTORE_PC_MASKED_BITS

LOCAL_SRC_FILES  += m68k/fame/famec.cpp m68k/fame/m68k_intrf.cpp m68k/m68k_cmn_intrf.cpp

LOCAL_STATIC_LIBRARIES :=  sdl  

LOCAL_LDLIBS	+= -llog -lz

include $(BUILD_SHARED_LIBRARY)
