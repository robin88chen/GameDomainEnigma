package lib.bridge.enigma;

import android.content.Context;
import android.content.res.Resources;
import android.os.Build;
import android.os.Handler;
import android.os.SystemClock;
import android.view.MotionEvent;

public class ShoveGestureDetector {
    private static final String TAG = "ShoveGestureDetector";

    public interface OnShoveGestureListener {
        public boolean onShove(ShoveGestureDetector detector);
        public boolean onShoveBegin(ShoveGestureDetector detector);
        public void onShoveEnd(ShoveGestureDetector detector);
    }

    public static class SimpleOnShoveGestureListener implements OnShoveGestureListener {

        public boolean onShove(ShoveGestureDetector detector) {
            return false;
        }

        public boolean onShoveBegin(ShoveGestureDetector detector) {
            return true;
        }

        public void onShoveEnd(ShoveGestureDetector detector) {
            // Intentionally empty
        }
    }

    private final Context mContext;
    private final OnShoveGestureListener mListener;

    private float mCurrX0;
    private float mCurrY0;
    private float mCurrX1;
    private float mCurrY1;
    private float mPrevX0;
    private float mPrevY0;
    private float mPrevX1;
    private float mPrevY1;
    private float mCurrDistX0;
    private float mCurrDistY0;
    private float mCurrDistX1;
    private float mCurrDistY1;
    private long mCurrTime;
    private long mPrevTime;
    private boolean mInProgress;

    private final Handler mHandler;

    public ShoveGestureDetector(Context context, OnShoveGestureListener listener) {
        this(context, listener, null);
    }

    public ShoveGestureDetector(Context context, OnShoveGestureListener listener,
                                Handler handler) {
        mContext = context;
        mListener = listener;
        mHandler = handler;
    }

    public boolean onTouchEvent(MotionEvent event) {

        mCurrTime = event.getEventTime();

        final int action = event.getActionMasked();

        final int count = event.getPointerCount();

        final boolean streamComplete = action == MotionEvent.ACTION_UP ||
                action == MotionEvent.ACTION_CANCEL;

        if (action == MotionEvent.ACTION_DOWN || streamComplete) {
            // Reset any scale in progress with the listener.
            // If it's an ACTION_DOWN we're beginning a new event stream.
            // This means the app probably didn't give us all the events. Shame on it.
            if (mInProgress) {
                mListener.onShoveEnd(this);
                mInProgress = false;
                mPrevX0 = mCurrX0 = 0;
                mPrevY0 = mCurrY0 = 0;
                mPrevX1 = mCurrX1 = 0;
                mPrevY1 = mCurrY1 = 0;
                mCurrDistX0 = mCurrDistX1 = 0;
                mCurrDistY0 = mCurrDistY1 = 0;
            } else if (streamComplete) {
                mInProgress = false;
                mPrevX0 = mCurrX0 = 0;
                mPrevY0 = mCurrY0 = 0;
                mPrevX1 = mCurrX1 = 0;
                mPrevY1 = mCurrY1 = 0;
                mCurrDistX0 = mCurrDistX1 = 0;
                mCurrDistY0 = mCurrDistY1 = 0;
           }

            if (streamComplete) {
                return true;
            }
        }

        final boolean configChanged = action == MotionEvent.ACTION_DOWN ||
                action == MotionEvent.ACTION_POINTER_UP ||
                action == MotionEvent.ACTION_POINTER_DOWN;

        final boolean pointerUp = action == MotionEvent.ACTION_POINTER_UP;
        final int skipIndex = pointerUp ? event.getActionIndex() : -1;

        float cx0 = 0, cy0 = 0, cx1 = 0, cy1 = 0;
        cx0 = event.getX(0);
        cy0 = event.getY(0);
        if (count > 1)
        {
            cx1 = event.getX(1);
            cy1 = event.getY(1);
        }

        // Dispatch begin/end events as needed.
        // If the configuration changes, notify the app to reset its current state by beginning
        // a fresh shove event stream.
        if (mInProgress && configChanged) {
            mListener.onShoveEnd(this);
            mInProgress = false;
        }
        if (configChanged) {
            mPrevX0 = mCurrX0 = cx0;
            mPrevY0 = mCurrY0 = cy0;
            mPrevX1 = mCurrX1 = cx1;
            mPrevY1 = mCurrY1 = cy1;
            mCurrDistX0 = mCurrDistX1 = 0;
            mCurrDistY0 = mCurrDistY1 = 0;
        }

        if (action == MotionEvent.ACTION_MOVE) {
            mCurrX0 = cx0;
            mCurrX1 = cx1;
            mCurrY0 = cy0;
            mCurrY1 = cy1;
            mCurrDistX0 = (mCurrX0 - mPrevX0);
            mCurrDistY0 = (mCurrY0 - mPrevY0);
            mCurrDistX1 = (mCurrX1 - mPrevX1);
            mCurrDistY1 = (mCurrY1 - mPrevY1);
            float multiplyX0X1 = mCurrDistX0 * mCurrDistX1;
            float multiplyY0Y1 = mCurrDistY0 * mCurrDistY1;
            boolean updatePrev = true;
            if ((multiplyX0X1 > 0) && (multiplyY0Y1 > 0))
            {
                if (!mInProgress)
                {
                    mInProgress = mListener.onShoveBegin(this);
                }
                else
                {
                    updatePrev = mListener.onShove(this);
                }
            }
            if (updatePrev)
            {
                mPrevX0 = mCurrX0;
                mPrevY0 = mCurrY0;
                mPrevX1 = mCurrX1;
                mPrevY1 = mCurrY1;
                mPrevTime = mCurrTime;
            }
        }        

        return true;
    }

    public boolean isInProgress() {
        return mInProgress;
    }

    public float getFocusX() {
        return (mCurrX0 + mCurrX1) / 2;
    }

    public float getFocusY() {
        return (mCurrY0 + mCurrY1) / 2;
    }

    public float getDistX()
    {
        return (mCurrDistX0 + mCurrDistX1) / 2;
    }

    public float getDistY()
    {
        return (mCurrDistY0 + mCurrDistY1) / 2;
    }

    public long getTimeDelta() {
        return mCurrTime - mPrevTime;
    }

    public long getEventTime() {
        return mCurrTime;
    }
}