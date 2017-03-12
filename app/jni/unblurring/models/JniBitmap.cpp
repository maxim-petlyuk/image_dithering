#include <android/bitmap.h>
#include <stdio.h>
#include <unistd.h>

class JniBitmap {
    private:
        uint32_t* storedBitmapPixels;
        AndroidBitmapInfo bitmapInfo;

    public:
        JniBitmap(){
	        storedBitmapPixels = NULL;
	    }

	    void setStoredPixels(uint32_t* pixels);
	    void setBitmapInfo(AndroidBitmapInfo bitmapInfo);
	    AndroidBitmapInfo getBitmapInfo();
	    uint32_t* getStoredPixels();
};

void JniBitmap::setStoredPixels(uint32_t* pixels) {
    storedBitmapPixels = pixels;
}

void JniBitmap::setBitmapInfo(AndroidBitmapInfo bitmapData) {
    bitmapInfo = bitmapData;
}

AndroidBitmapInfo JniBitmap::getBitmapInfo() {
    return bitmapInfo;
}

uint32_t* JniBitmap::getStoredPixels() {
    return storedBitmapPixels;
}
