#include <jni.h>
#include <string>
#include <string.h>
#include <android/log.h>
#include <android/bitmap.h>
#include <unistd.h>
#include <stdio.h>
#include "models/JniBitmap.cpp"
#include "models/ARGB.cpp"
#include "models/Node.cpp"
#include "UnblurringCore.h"
#include "cmath"

extern "C"
{

    static const int CHILD_NODE_CREATED = 1;
    static const int CHILD_NODE_DOES_NOT_CREATED = -1;

    JNIEXPORT jobject JNICALL Java_com_onix_pictureclarify_core_NativeCore_jniStoreBitmapData(JNIEnv * env, jobject obj, jobject bitmap) {

        AndroidBitmapInfo bitmapInfo;
        uint32_t* storedBitmapPixels = NULL;

        int ret;
        if ((ret = AndroidBitmap_getInfo(env, bitmap, &bitmapInfo)) < 0) {
            LOGE("AndroidBitmap_getInfo() failed ! error=%d", ret);
            return NULL;
        }

        if (bitmapInfo.format != ANDROID_BITMAP_FORMAT_RGBA_8888) {
            LOGE("Bitmap format is not RGBA_8888!");
            return NULL;
        }

        void* bitmapPixels;
        if ((ret = AndroidBitmap_lockPixels(env, bitmap, &bitmapPixels)) < 0) {
            LOGE("AndroidBitmap_lockPixels() failed ! error=%d", ret);
            return NULL;
        }

        uint32_t* src = (uint32_t*) bitmapPixels;

        int pixelsCount = bitmapInfo.height * bitmapInfo.width;
        storedBitmapPixels = new uint32_t[pixelsCount];

        memcpy(storedBitmapPixels, src, sizeof(uint32_t) * pixelsCount);

        AndroidBitmap_unlockPixels(env, bitmap);

        JniBitmap *jniBitmap = new JniBitmap();
        jniBitmap->setBitmapInfo(bitmapInfo);
        jniBitmap->setStoredPixels(storedBitmapPixels);

        return env->NewDirectByteBuffer(jniBitmap, 0);
    }

    JNIEXPORT void JNICALL Java_com_onix_pictureclarify_core_NativeCore_jniFreeBitmap(JNIEnv * env, jobject obj, jobject bitmap) {
        JniBitmap* jniBitmap = (JniBitmap*) env->GetDirectBufferAddress(bitmap);

        if (jniBitmap->getStoredPixels() == NULL) {
	        return;
	    }

        delete[] jniBitmap->getStoredPixels();
        jniBitmap->setStoredPixels(NULL);

        delete jniBitmap;
    }

    JNIEXPORT void JNICALL Java_com_onix_pictureclarify_core_NativeCore_jniFreePixelizeBuffer(JNIEnv * env, jobject obj, jobject nodeBuffer) {
        Node* pixelizeBuffer = (Node*) env->GetDirectBufferAddress(nodeBuffer);

        if (pixelizeBuffer->hasChildren()) {
            removeNodeChilds(pixelizeBuffer);
        }

        delete pixelizeBuffer;
    }

    void removeNodeChilds(Node* node) {
        for (Node* pixelizeBuffer : node->getChildNodeList()) {
            if (pixelizeBuffer->hasChildren()) {
                removeNodeChilds(pixelizeBuffer);
            }

            delete pixelizeBuffer;
        }
    }

    JNIEXPORT void JNICALL Java_com_onix_pictureclarify_core_NativeCore_jniPixelizeBitmap(JNIEnv * env, jobject obj,
        jobject bitmap, jobject originalImagebuffer, jobject nodeBuffer) {
        JniBitmap* jniBitmap = (JniBitmap*) env->GetDirectBufferAddress(originalImagebuffer);

        int ret;
        void* pixels;

        if ((ret = AndroidBitmap_lockPixels(env, bitmap, &pixels)) < 0) {
            LOGE("AndroidBitmap_lockPixels() failed ! error=%d", ret);
        }

        uint32_t* bitmapPixels = (uint32_t*) pixels;

        Node* rootNode = (Node*) env->GetDirectBufferAddress(nodeBuffer);
        drawPixels(rootNode, bitmapPixels, jniBitmap->getStoredPixels(), jniBitmap->getBitmapInfo().width);

        AndroidBitmap_unlockPixels(env, bitmap);
    }

    void drawPixels(Node* node, uint32_t* bitmapPixels, uint32_t* originalImagePixels, int rootWidth) {
        for(Node* childNode : node->getChildNodeList()) {
             if (childNode->hasChildren()) {
                drawPixels(childNode, bitmapPixels, originalImagePixels, rootWidth);
                continue;
             }

            ARGB* middleARGB = getMiddleARGB(childNode, originalImagePixels, rootWidth);
            for (int y = childNode->getTop(); y < childNode->getBottom(); y++) {
                for (int x = childNode->getLeft(); x < childNode->getRight(); x++) {
                    uint32_t ditherColor = childNode->isRenderAvailable()
                        ? ARGB::convertArgbToInt(middleARGB) : originalImagePixels[x * rootWidth + y];

                    bitmapPixels[x * rootWidth + y] = ditherColor;
                }
            }
            delete middleARGB;
        }
    }

    ARGB* getMiddleARGB(Node* childNode, uint32_t* originalImagePixels, int rootWidth) {
        long sumAlphaChannel, sumRedChannel = 0, sumGreenChannel = 0, sumBlueChannel = 0;
        for (int y = childNode->getTop(); y < childNode->getBottom(); y++) {
            for (int x = childNode->getLeft(); x < childNode->getRight(); x++) {
                ARGB* argbFromPixel = ARGB::convertIntToArgb(originalImagePixels[x * rootWidth + y]);

                sumAlphaChannel += pow(argbFromPixel->getAlpha(), 2);
                sumRedChannel += pow(argbFromPixel->getRed(), 2);
                sumGreenChannel += pow(argbFromPixel->getGreen(), 2);
                sumBlueChannel += pow(argbFromPixel->getBlue(), 2);

                delete argbFromPixel;
            }
        }

        int pixelsCount = childNode->getWidth() * childNode->getHeight();

        ARGB* middleARGB = ARGB::createARGB(
            sqrt(sumAlphaChannel / pixelsCount),
            sqrt(sumRedChannel / pixelsCount),
            sqrt(sumGreenChannel / pixelsCount),
            sqrt(sumBlueChannel / pixelsCount));

        return middleARGB;
    }

    JNIEXPORT jobject JNICALL Java_com_onix_pictureclarify_core_NativeCore_jniCreateRootNode(JNIEnv * env,
            jobject obj, jobject bitmapBuffer) {
        JniBitmap* jniBitmap = (JniBitmap*) env->GetDirectBufferAddress(bitmapBuffer);

        Node* rootNode = new Node();
        rootNode->setLocation(0, jniBitmap->getBitmapInfo().width, 0, jniBitmap->getBitmapInfo().height);
        createChildrenForNode(rootNode);

        return env->NewDirectByteBuffer(rootNode, 0);
    }

    JNIEXPORT bool JNICALL Java_com_onix_pictureclarify_core_NativeCore_jniCreateChildrenForNode(JNIEnv * env, jobject obj, jobject nodeBuffer) {
        Node* existNode = (Node*) env->GetDirectBufferAddress(nodeBuffer);

        int flagNodeCreated = CHILD_NODE_DOES_NOT_CREATED;
        processChildrenForNode(existNode, &flagNodeCreated);

        return flagNodeCreated == CHILD_NODE_CREATED;
    }

    void processChildrenForNode(Node* node, int* flagNodeCreated) {
        for (Node* childNode : node->getChildNodeList()) {
            if (childNode->hasChildren()) {
                processChildrenForNode(childNode, flagNodeCreated);
                continue;
            }

            if (childNode->isRenderAvailable()) {
                *flagNodeCreated = CHILD_NODE_CREATED;
                createChildrenForNode(childNode);
            }
        }
    }

    void createChildrenForNode(Node* node) {
        int xLeft, xRight, yTop, yBottom, xCenter, yCenter;
        xLeft = node->getLeft();
        xRight = node->getRight();
        yTop = node->getTop();
        yBottom = node->getBottom();

        Node* leftTopNode = Node::createRootNode(xLeft, xLeft + node->getWidth() / 2,
            yTop, yTop + node->getHeight() / 2);
        Node* leftBottomNode = Node::createRootNode(xLeft, xLeft + node->getWidth() / 2,
            yTop + node->getHeight() / 2, yTop + node->getHeight());
        Node* rightTopNode = Node::createRootNode(xLeft + node->getWidth() / 2, xLeft + node->getWidth(),
            yTop, yTop + node->getHeight() / 2);
        Node* rightBottomNode = Node::createRootNode(xLeft + node->getWidth() / 2, xLeft + node->getWidth(),
            yTop + node->getHeight() / 2, yTop + node->getHeight());

        node->setRightTopNode(rightTopNode);
        node->setRightBottomNode(rightBottomNode);
        node->setLeftTopNode(leftTopNode);
        node->setLeftBottomNode(leftBottomNode);
    }

    JNIEXPORT jobject JNICALL Java_com_onix_pictureclarify_core_NativeCore_jniGetBitmapFromStoredBitmapData(
        JNIEnv * env, jobject obj, jobject bitmap) {
        JniBitmap* jniBitmap = (JniBitmap*) env->GetDirectBufferAddress(bitmap);

        if (jniBitmap->getStoredPixels() == NULL) {
    	    LOGD("no bitmap data was stored. returning null...");
    	    return NULL;
    	}

        jclass bitmapCls = env->FindClass("android/graphics/Bitmap");
        jmethodID createBitmapFunction = env->GetStaticMethodID(
            bitmapCls,
    	    "createBitmap",
    	    "(IILandroid/graphics/Bitmap$Config;)Landroid/graphics/Bitmap;");

        jstring configName = env->NewStringUTF("ARGB_8888");
        jclass bitmapConfigClass = env->FindClass("android/graphics/Bitmap$Config");

        jmethodID valueOfBitmapConfigFunction = env->GetStaticMethodID(
    	    bitmapConfigClass,
    	    "valueOf",
    	    "(Ljava/lang/String;)Landroid/graphics/Bitmap$Config;");

        jobject bitmapConfig = env->CallStaticObjectMethod(bitmapConfigClass, valueOfBitmapConfigFunction, configName);

        jobject newBitmap = env->CallStaticObjectMethod(
            bitmapCls,
    	    createBitmapFunction,
    	    jniBitmap->getBitmapInfo().width,
    	    jniBitmap->getBitmapInfo().height,
    	    bitmapConfig);

        int ret;
        void* bitmapPixels;
        if ((ret = AndroidBitmap_lockPixels(env, newBitmap, &bitmapPixels)) < 0) {
    	    LOGE("AndroidBitmap_lockPixels() failed ! error=%d", ret);
    	    return NULL;
    	}

        uint32_t* newBitmapPixels = (uint32_t*) bitmapPixels;
        int pixelsCount = jniBitmap->getBitmapInfo().height * jniBitmap->getBitmapInfo().width;

        memcpy(newBitmapPixels, jniBitmap->getStoredPixels(), sizeof(uint32_t) * pixelsCount);
        AndroidBitmap_unlockPixels(env, newBitmap);

        return newBitmap;
    }
}
