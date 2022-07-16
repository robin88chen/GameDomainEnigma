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

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import android.opengl.GLSurfaceView;
import android.opengl.GLES20;
import android.util.Log;

public class ApplicationBridgeRenderer implements GLSurfaceView.Renderer {
	private BridgeEGLConfigChooser m_ConfigChooser;
    @Override
    public void onSurfaceCreated(final GL10 GL10, final EGLConfig EGLConfig) {
		onBridgeSurfaceCreate(0, 0, m_ConfigChooser.getSelectedAttribs());
		// Set the background frame color
        //GLES20.glClearColor(1.0f, 0.0f, 1.0f, 1.0f);
    }

    @Override
    public void onSurfaceChanged(final GL10 GL10, final int width, final int height) {
		onBridgeSurfaceChange(width, height);
        //GLES20.glViewport(0, 0, width, height);
    }

    @Override
    public void onDrawFrame(final GL10 gl) {
		onRendererDrawFrame();
        // Redraw background color
        //GLES20.glClear(GLES20.GL_COLOR_BUFFER_BIT);
    }

	public void setConfigChooser(BridgeEGLConfigChooser chooser)
	{
		m_ConfigChooser = chooser;
	}

	public native void onBridgeSurfaceCreate(int width, int height, int[] config_attrib);
	public native void onBridgeSurfaceChange(int width, int height);
	public native void onRendererDrawFrame();
}
