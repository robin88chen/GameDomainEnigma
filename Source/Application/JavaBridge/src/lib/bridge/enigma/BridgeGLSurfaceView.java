/****************************************************************************
Copyright (c) 2010-2011 cocos2d-x.org

http://www.cocos2d-x.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
 ****************************************************************************/
package lib.bridge.enigma;

import android.content.Context;
import android.opengl.GLSurfaceView;
import android.os.Handler;
import android.os.Message;
import android.util.AttributeSet;
import android.util.Log;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.inputmethod.InputMethodManager;
import android.view.GestureDetector;
import android.view.GestureDetector.SimpleOnGestureListener;
import android.view.ScaleGestureDetector;
import android.view.ScaleGestureDetector.SimpleOnScaleGestureListener;
import lib.bridge.enigma.ShoveGestureDetector;
import lib.bridge.enigma.ShoveGestureDetector.SimpleOnShoveGestureListener;

public class BridgeGLSurfaceView extends GLSurfaceView {
    private ApplicationBridgeRenderer m_AppBridgeRenderer;
    private boolean mMultipleTouchEnabled = true;
	private GestureDetector m_GestureDetector;
	private ScaleGestureDetector m_ScaleGestureDetector;
	private ShoveGestureDetector m_ShoveGestureDetector;

    public BridgeGLSurfaceView(final Context context) {
        super(context);

        this.initView();
		m_GestureDetector = new GestureDetector(context, new SimpleOnGestureListener()
		{
			@Override
			public void onLongPress(MotionEvent e)
			{
				nativeGestureLongPress(e.getX(), e.getY());
			}
			@Override
			public void onShowPress(MotionEvent e)
			{
				nativeGestureShowPress(e.getX(), e.getY());
			}
			@Override
			public boolean onDoubleTap(MotionEvent e)
			{
				nativeGestureDoubleTap(e.getX(), e.getY());
				return true;
			}
			@Override
			public boolean onSingleTapConfirmed(MotionEvent e)
			{
				nativeGestureSingleTap(e.getX(), e.getY());
				return true;
			}
			@Override
			public boolean onScroll(MotionEvent e1, MotionEvent e2, 
                float distanceX, float distanceY)
			{
				nativeGestureScroll(e1.getX(), e1.getY(), e2.getX(), e2.getY(), distanceX, distanceY);
				return true;
			}
			@Override
			public boolean onFling(MotionEvent e1, MotionEvent e2, 
                float velocityX, float velocityY)
			{
				nativeGestureFling(e1.getX(), e1.getY(), e2.getX(), e2.getY(), velocityX, velocityY);
				return true;
			}
		});
		m_ScaleGestureDetector = new ScaleGestureDetector(context, new SimpleOnScaleGestureListener()
		{
			@Override
			public boolean onScale(ScaleGestureDetector detector)
			{
				nativeGestureScale(detector.getScaleFactor(), detector.getFocusX(), detector.getFocusY());
				return true;
			}
			@Override
			public boolean onScaleBegin(ScaleGestureDetector detector)
			{
				nativeGestureScaleBegin(detector.getCurrentSpanX(), detector.getCurrentSpanY(),
				 detector.getFocusX(), detector.getFocusY());
				return true;
			}
			@Override
			public void onScaleEnd(ScaleGestureDetector detector)
			{
				nativeGestureScaleEnd(detector.getCurrentSpanX(), detector.getCurrentSpanY(),
				 detector.getFocusX(), detector.getFocusY());
			}
		});
		m_ShoveGestureDetector = new ShoveGestureDetector(context, new SimpleOnShoveGestureListener()
		{
			@Override
			public boolean onShove(ShoveGestureDetector detector)
			{
				nativeGestureShove(detector.getFocusX(), detector.getFocusY(), 
					detector.getDistX(), detector.getDistY());
				return true;
			}
			@Override
			public boolean onShoveBegin(ShoveGestureDetector detector)
			{
				nativeGestureShoveBegin(detector.getFocusX(), detector.getFocusY());
				return true;
			}
			@Override
			public void onShoveEnd(ShoveGestureDetector detector)
			{
				nativeGestureShoveEnd(detector.getFocusX(), detector.getFocusY());
			}
		});
    }

    public BridgeGLSurfaceView(final Context context, final AttributeSet attrs) {
        super(context, attrs);
        
        this.initView();
    }

    protected void initView() {
        setEGLContextClientVersion(2);
    }

    public void setGraphicBridgeRenderer(final ApplicationBridgeRenderer renderer) {
        m_AppBridgeRenderer = renderer;
        setRenderer(renderer);
    }

    @Override
    public boolean onTouchEvent(final MotionEvent event) {
		m_ShoveGestureDetector.onTouchEvent(event);
	    m_ScaleGestureDetector.onTouchEvent(event);
		if ((m_ShoveGestureDetector.isInProgress()) || (m_ScaleGestureDetector.isInProgress())) return true;
	    m_GestureDetector.onTouchEvent(event);
		return true;
    }
	private native void nativeGestureLongPress(final float x, final float y);
	private native void nativeGestureShowPress(final float x, final float y);
	private native void nativeGestureDoubleTap(final float x, final float y);
	private native void nativeGestureSingleTap(final float x, final float y);
	private native void nativeGestureScroll(final float startX, final float startY,
	 final float currentX, final float currentY, final float deltaX, final float deltaY);
	private native void nativeGestureFling(final float startX, final float startY,
		final float currentX, final float currentY, final float velocityX, final float velocityY);
	private native void nativeGestureScale(final float factor, final float focusX, final float focusY);
	private native void nativeGestureScaleBegin(final float spanX, final float spanY, 
		final float focusX, final float focusY);
	private native void nativeGestureScaleEnd(final float spanX, final float spanY, 
		final float focusX, final float focusY);
	private native void nativeGestureShove(final float focusX, final float focusY,
		final float deltaX, final float deltaY);
	private native void nativeGestureShoveBegin(final float focusX, final float focusY);
	private native void nativeGestureShoveEnd(final float focusX, final float focusY);
}
