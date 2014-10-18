LOCAL_PATH:=$(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE:=THGLInstant
LOCAL_SRC_FILES:=GameEngine/Rendering/THDrawing.cpp\
GameEngine/Rendering/THRenderer.cpp\
GameEngine/Rendering/THEffects.cpp\
GameEngine/Rendering/THShadowEffect.cpp\
GameEngine/Rendering/Main.cpp\
GameEngine/Util/THMath.cpp\
GameEngine/DisplayObject/THFrame.cpp\
GameEngine/DisplayObject/THDisplayObject.cpp\
GameEngine/DisplayObject/THChar.cpp\
Callback.cpp
LOCAL_LDLIBS:=-lGLESv2 -lEGL -llog -landroid
LOCAL_STATIC_LIBRARIES:=android_native_app_glue THLibPNG

include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE:=THLibPNG
LOCAL_EXPORT_LDLIBS:=-lz
LOCAL_SRC_FILES:=libpng/png.c\
libpng/pngerror.c\
libpng/pngget.c\
libpng/pngmem.c\
libpng/pngpread.c\
libpng/pngread.c\
libpng/pngrio.c\
libpng/pngrtran.c\
libpng/pngrutil.c\
libpng/pngset.c\
libpng/pngtest.c\
libpng/pngtrans.c\
libpng/pngwio.c\
libpng/pngwrite.c\
libpng/pngwtran.c\
libpng/pngwutil.c
include $(BUILD_STATIC_LIBRARY)

$(call import-module,android/native_app_glue)