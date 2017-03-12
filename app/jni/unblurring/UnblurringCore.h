#include <jni.h>

#define  LOG_TAG    "Native unblurring core"
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#define  LOGW(...)  __android_log_print(ANDROID_LOG_WARN,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#define  LOGI(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

extern "C" {

    JNIEXPORT jobject JNICALL Java_com_onix_pictureclarify_core_NativeCore_jniStoreBitmapData(JNIEnv * env,jobject obj, jobject bitmap);

    JNIEXPORT void JNICALL Java_com_onix_pictureclarify_core_NativeCore_jniFreeBitmap(JNIEnv * env, jobject obj, jobject handle);

    JNIEXPORT void JNICALL Java_com_onix_pictureclarify_core_NativeCore_jniFreePixelizeBuffer(JNIEnv * env, jobject obj, jobject nodeBuffer);

    JNIEXPORT void JNICALL Java_com_onix_pictureclarify_core_NativeCore_jniPixelizeBitmap(JNIEnv * env, jobject obj, jobject bitmap,
        jobject originalImagebuffer, jobject nodeBuffer);

    JNIEXPORT jobject JNICALL Java_com_onix_pictureclarify_core_NativeCore_jniGetBitmapFromStoredBitmapData(JNIEnv * env, jobject obj, jobject bitmap);

    JNIEXPORT jobject JNICALL Java_com_onix_pictureclarify_core_NativeCore_jniCreateRootNode(JNIEnv * env, jobject obj, jobject bitmapBuffer);

    JNIEXPORT bool JNICALL Java_com_onix_pictureclarify_core_NativeCore_jniCreateChildrenForNode(JNIEnv * env, jobject obj, jobject nodeBuffer);

    ARGB* getMiddleARGB(Node* childNode, uint32_t* originalImagePixels, int width);

    void createChildrenForNode(Node* node);

    void removeNodeChilds(Node* node);

    void drawPixels(Node* node, uint32_t* bitmapPixels, uint32_t* originalImagePixels, int rootWidth);

   void processChildrenForNode(Node* node, int* flagNodeCreated);
}