
package com.DrawPrimitiveWithTextureApp;
import android.os.Bundle;
import lib.bridge.enigma.ApplicationBridgeActivity;

public class DrawPrimitiveWithTextureApp extends ApplicationBridgeActivity
{
    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        initBridgeCallback();
        super.onCreate(savedInstanceState);
        initAdditionalMountPath();
    }
    public native void initBridgeCallback();
    public native void initAdditionalMountPath();
    /* This is the static constructor used to load the
     * 'ParticlePrimitiveTest' library when the class is
     * loaded.
     */
    static {
        System.loadLibrary("DrawPrimitiveWithTextureApp_Jni");
    }
}
