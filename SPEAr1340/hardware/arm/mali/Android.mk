LOCAL_PATH:=$(call my-dir)
LOCAL_MODULE_TAGS := optional
LOCAL_PREBUILT_LIBS := libEGL_mali.so \
			libGLESv1_CM_mali.so \
			libGLESv2_mali.so \
			libMali.so \
			libUMP.so

PRODUCT_COPY_FILES += $(LOCAL_PATH)/egl.cfg:system/lib/egl/egl.cfg

PRODUCT_COPY_FILES += $(LOCAL_PATH)/libGLESv2_mali.so:system/lib/egl/libGLESv2_mali.so
PRODUCT_COPY_FILES += $(LOCAL_PATH)/libGLESv1_CM_mali.so:system/lib/egl/libGLESv1_CM_mali.so
PRODUCT_COPY_FILES += $(LOCAL_PATH)/libEGL_mali.so:system/lib/egl/libEGL_mali.so
PRODUCT_COPY_FILES += $(LOCAL_PATH)/libMali.so:system/lib/libMali.so
PRODUCT_COPY_FILES += $(LOCAL_PATH)/libUMP.so:system/lib/libUMP.so

include $(BUILD_MULTI_PREBUILT)
