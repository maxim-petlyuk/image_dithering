package com.onix.pictureclarify.core;

import android.graphics.Bitmap;

import java.nio.ByteBuffer;

public class NativeCore {

    static {
        System.loadLibrary("unblurring");
    }

    private ByteBuffer mOriginalImageBuffer;
    private ByteBuffer mNodeBuffer;
    private IProcessImageListener mImageListener;

    private native ByteBuffer jniStoreBitmapData(Bitmap bitmap);

    private native ByteBuffer jniFreeBitmap(ByteBuffer bitmapBuffer);

    private native ByteBuffer jniFreePixelizeBuffer(ByteBuffer nodeBuffer);

    private native Bitmap jniPixelizeBitmap(Bitmap bitmap, ByteBuffer originalImageBuffer, ByteBuffer nodeBuffer);

    private native Bitmap jniGetBitmapFromStoredBitmapData(ByteBuffer bitmapBuffer);

    private native ByteBuffer jniCreateRootNode(ByteBuffer bitmapBuffer);

    private native boolean jniCreateChildrenForNode(ByteBuffer nodeBuffer);

    public void onNativeFrameEnd() {
        if (mImageListener != null) {
            mImageListener.onFrameEnd();
        }
    }

    public void freeStoredBitmap() {
        if (mOriginalImageBuffer != null) {
            jniFreeBitmap(mOriginalImageBuffer);
        }

        mOriginalImageBuffer = null;
    }

    public void freePixelizeBuffer() {
        if (mNodeBuffer != null) {
            jniFreePixelizeBuffer(mNodeBuffer);
        }

        mNodeBuffer = null;
    }

    public void setImageListener(IProcessImageListener imageListener) {
        mImageListener = imageListener;
    }

    public void storeBitmap(final Bitmap bitmap) {
        if (mOriginalImageBuffer != null) {
            jniFreeBitmap(mOriginalImageBuffer);
        }

        mOriginalImageBuffer = jniStoreBitmapData(bitmap);
    }

    public Bitmap getExistBitmap() {
        return jniGetBitmapFromStoredBitmapData(mOriginalImageBuffer);
    }

    public boolean pixelize(Bitmap bitmap) {
        if (mNodeBuffer == null) {
            mNodeBuffer = jniCreateRootNode(mOriginalImageBuffer);
        } else {
            boolean result = jniCreateChildrenForNode(mNodeBuffer);
            if (!result) {
                if (mImageListener != null) {
                    mImageListener.onFrameEnd();
                }
                return false;
            }
        }

        jniPixelizeBitmap(bitmap, mOriginalImageBuffer, mNodeBuffer);

        return true;
    }
}
