/*
 * Copyright (C) 2009 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
package lib.bridge.enigma;

import android.app.Activity;
import android.content.res.Resources;
import android.content.res.AssetManager;
import android.os.Bundle;
import android.content.Context;
import android.view.ViewGroup;


public class ApplicationBridgeActivity extends BridgeActivity
{
	static private ApplicationBridgeActivity sContext = null;
	private BridgeGLSurfaceView m_BridgeSurfaceView;
	private ResizeLayout m_FrameLayout;
	private BridgeEditBoxHelper m_EditBoxHelper;
	/** Called when the activity is first created. */
	@Override
	public void onCreate(Bundle savedInstanceState)
	{
		super.onCreate(savedInstanceState);
		sContext = this;
		onBridgeCreate();
		initSurfaceView();
        // FrameLayout
        ViewGroup.LayoutParams framelayout_params =
            new ViewGroup.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT,
                                       ViewGroup.LayoutParams.MATCH_PARENT);

        m_FrameLayout = new ResizeLayout(this);

        m_FrameLayout.setLayoutParams(framelayout_params);

        // EditText layout
        ViewGroup.LayoutParams edittext_layout_params =
            new ViewGroup.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT,
                                       ViewGroup.LayoutParams.WRAP_CONTENT);
        BridgeEditBox edittext = new BridgeEditBox(this);
        edittext.setLayoutParams(edittext_layout_params);


        m_FrameLayout.addView(edittext);

        // ...add to FrameLayout
        m_FrameLayout.addView(m_BridgeSurfaceView);

        // Set framelayout as the content view
        setContentView(m_FrameLayout);

		m_EditBoxHelper = new BridgeEditBoxHelper(m_FrameLayout);
		//setContentView(m_BridgeSurfaceView);
	}

    @Override
    protected void onDestroy() {
		onBridgeDestroy();
        super.onDestroy();
    }

	public native void onBridgeCreate();
	public native void onBridgeDestroy();
    private native int[] getGLContextAttrs();

	public void initSurfaceView()
	{
		m_BridgeSurfaceView = new BridgeGLSurfaceView(this);
		int[] configAttr = getGLContextAttrs();
        BridgeEGLConfigChooser chooser = new BridgeEGLConfigChooser(configAttr);
        m_BridgeSurfaceView.setEGLConfigChooser(chooser);
		ApplicationBridgeRenderer renderer = new ApplicationBridgeRenderer();
		renderer.setConfigChooser(chooser);
		m_BridgeSurfaceView.setGraphicBridgeRenderer(renderer);
	}
    public BridgeGLSurfaceView getGLSurfaceView(){
        return  m_BridgeSurfaceView;
    }
    public static Context getContext() {
        return sContext;
    }
    public void runOnGLThread(final Runnable pRunnable) {
        m_BridgeSurfaceView.queueEvent(pRunnable);
    }
}
