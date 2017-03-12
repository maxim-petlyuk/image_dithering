package com.onix.pictureclarify;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.Build;
import android.os.Bundle;
import android.support.annotation.RequiresApi;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.MotionEvent;
import android.view.View;
import android.widget.ImageView;
import android.widget.Toast;

import com.onix.pictureclarify.core.IProcessImageListener;
import com.onix.pictureclarify.core.NativeCore;

public class MainActivity extends AppCompatActivity implements IProcessImageListener {

    private final static float TOUCH_TRESHOLD = 2000;
    private final static String TAG = "ONTOUCH";

    private NativeCore mNativeCore;
    private Bitmap mBitmap;
    private boolean isPointerDetect = false;

    @RequiresApi(api = Build.VERSION_CODES.KITKAT)
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        mNativeCore = new NativeCore();

        final ImageView ivTestImage = (ImageView) findViewById(R.id.iv_test);
        ivTestImage.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View view, MotionEvent motionEvent) {
                float xStart = 0, yStart = 0, xDelta, yDelta;

                switch (motionEvent.getAction()) {
                    case MotionEvent.ACTION_DOWN:
                        if (!isPointerDetect) {
                            return false;
                        }

                        isPointerDetect = true;
                        xStart = motionEvent.getX();
                        yStart = motionEvent.getY();
                        break;

                    case MotionEvent.ACTION_MOVE:
                        xDelta = Math.abs(motionEvent.getX() - xStart);
                        yDelta = Math.abs(motionEvent.getY() - yStart);
                        if (xDelta > TOUCH_TRESHOLD || yDelta > TOUCH_TRESHOLD) {
                            Log.e(TAG, "action move");
                            xStart = motionEvent.getX();
                            yStart = motionEvent.getY();
                        }
                        break;

                    case MotionEvent.ACTION_UP:
                        isPointerDetect = false;
                        break;
                }
                return true;
            }
        });

        Bitmap newBitmap = BitmapFactory.decodeResource(getResources(), R.drawable.ic_test);
        mBitmap = Bitmap.createScaledBitmap(newBitmap, 512, 512, false);

        mNativeCore.setImageListener(this);
        mNativeCore.storeBitmap(mBitmap);

        findViewById(R.id.btn_get_bitmap).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                mBitmap = mNativeCore.getExistBitmap();

                if (mBitmap != null)
                    ivTestImage.setImageBitmap(mBitmap);
            }
        });

        findViewById(R.id.btn_pixelize).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                boolean result = mNativeCore.pixelize(mBitmap);

                if (mBitmap != null && result)
                    ivTestImage.setImageBitmap(mBitmap);
            }
        });

        findViewById(R.id.btn_reset).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                mNativeCore.freePixelizeBuffer();
            }
        });
    }

    @Override
    public void onFrameEnd() {
        Toast.makeText(this, "That`s all!", Toast.LENGTH_SHORT).show();
    }
}