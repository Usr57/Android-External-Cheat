APP_ABI := arm64-v8a x86_64
APP_PLATFORM := android-24
APP_STL := c++_static
APP_OPTIM := release
APP_CPPFLAGS := -std=c++17 -fno-rtti -fno-exceptions -DNDEBUG -Wall -fpermissive -fpic
APP_LDFLAGS := -llog
APP_THIN_ARCHIVE := true
APP_PIE := true
