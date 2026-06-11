LOCAL_PATH := $(call my-dir)

ifeq ($(TARGET_ARCH_ABI),arm64-v8a)
    include $(CLEAR_VARS)
    LOCAL_MODULE    := dobby
    LOCAL_SRC_FILES := libraries/$(TARGET_ARCH_ABI)/libdobby.a
    include $(PREBUILT_STATIC_LIBRARY)
endif


ifneq ($(filter arm64-v8a x86_64, $(TARGET_ARCH_ABI)),)
    include $(CLEAR_VARS)
    LOCAL_MODULE     := Memory
    LOCAL_CPPFLAGS   += -fms-extensions -fno-rtti -fno-exceptions -fpermissive
    LOCAL_ARM_MODE   := arm
    LOCAL_C_INCLUDES := $(LOCAL_PATH)
    LOCAL_SRC_FILES  := Memory.cpp
    LOCAL_LDLIBS     := -llog -landroid -lGLESv2
    include $(BUILD_SHARED_LIBRARY)
endif


ifeq ($(TARGET_ARCH_ABI),arm64-v8a)
    include $(CLEAR_VARS)
    LOCAL_MODULE     := main
    LOCAL_CFLAGS     := -w -s -Wno-error=format-security -fvisibility=hidden -fpermissive -fexceptions
    LOCAL_CPPFLAGS   := -w -s -Wno-error=format-security -fvisibility=hidden -Werror -std=c++17
    LOCAL_CPPFLAGS   += -Wno-error=c++11-narrowing -fpermissive -Wall -fexceptions
    LOCAL_LDFLAGS    += -Wl,--gc-sections,--strip-all,-llog
    LOCAL_LDLIBS     := -llog -landroid -lEGL -lGLESv2
    LOCAL_ARM_MODE   := arm

    LOCAL_STATIC_LIBRARIES := dobby

    LOCAL_C_INCLUDES := $(LOCAL_PATH)
    LOCAL_SRC_FILES  := Chams.cpp
    include $(BUILD_SHARED_LIBRARY)
endif


ifneq ($(filter arm64-v8a x86_64, $(TARGET_ARCH_ABI)),)
    include $(CLEAR_VARS)
    LOCAL_MODULE     := User
    LOCAL_CFLAGS     := -w -s -Wno-error=format-security -fvisibility=hidden -fpermissive -fexceptions
    LOCAL_CPPFLAGS   := -w -s -Wno-error=format-security -fvisibility=hidden -Werror -std=c++17
    LOCAL_CPPFLAGS   += -Wno-error=c++11-narrowing -fpermissive -Wall -fexceptions
    LOCAL_LDFLAGS    += -Wl,--gc-sections,--strip-all,-llog
    LOCAL_LDLIBS     := -llog -landroid -lEGL -lGLESv2
    LOCAL_ARM_MODE   := arm
    LOCAL_C_INCLUDES := $(LOCAL_PATH)
    LOCAL_SRC_FILES  := Main.cpp
    include $(BUILD_SHARED_LIBRARY)
endif
