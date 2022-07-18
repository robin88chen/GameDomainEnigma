package lib.bridge.enigma;

import android.opengl.GLSurfaceView;
import javax.microedition.khronos.egl.EGL10;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.egl.EGLDisplay;
import android.util.Log;

public class BridgeEGLConfigChooser implements GLSurfaceView.EGLConfigChooser
{
    protected int[] configAttribs;
	protected int[] selectedAttribs;
    public BridgeEGLConfigChooser(int redSize, int greenSize, int blueSize, int alphaSize, int depthSize, int stencilSize)
    {
        configAttribs = new int[] {redSize, greenSize, blueSize, alphaSize, depthSize, stencilSize};
    }
    public BridgeEGLConfigChooser(int[] attribs)
    {
        configAttribs = attribs;
    }
    
	public int[] getSelectedAttribs() { return selectedAttribs; };
    public EGLConfig selectConfig(EGL10 egl, EGLDisplay display, EGLConfig[] configs, int[] attribs)
    {
        for (EGLConfig config : configs) {
            int d = findConfigAttrib(egl, display, config,
                    EGL10.EGL_DEPTH_SIZE, 0);
            int s = findConfigAttrib(egl, display, config,
                    EGL10.EGL_STENCIL_SIZE, 0);
            if ((d >= attribs[4]) && (s >= attribs[5])) 
			{
                int r = findConfigAttrib(egl, display, config,
                        EGL10.EGL_RED_SIZE, 0);
                int g = findConfigAttrib(egl, display, config,
                         EGL10.EGL_GREEN_SIZE, 0);
                int b = findConfigAttrib(egl, display, config,
                          EGL10.EGL_BLUE_SIZE, 0);
                int a = findConfigAttrib(egl, display, config,
                        EGL10.EGL_ALPHA_SIZE, 0);
                if ((r >= attribs[0]) && (g >= attribs[1])
                    && (b >= attribs[2]) && (a >= attribs[3])) 
				{
					printConfig(egl, display, config);
					selectedAttribs = new int[] { r, g, b, a, d, s };
					return config;
				}
            }
        }
        return null;
    }

    private int findConfigAttrib(EGL10 egl, EGLDisplay display,
            EGLConfig config, int attribute, int defaultValue) {
        int[] value = new int[1];
        if (egl.eglGetConfigAttrib(display, config, attribute, value)) {
            return value[0];
        }
        return defaultValue;
    }
    
    @Override
    public EGLConfig chooseConfig(EGL10 egl, EGLDisplay display) 
    {
        int[] numConfigs = new int[1];
        if(egl.eglGetConfigs(display, null, 0, numConfigs))
        {
            EGLConfig[] configs = new EGLConfig[numConfigs[0]];
            int[] EGLattribs = {
                    EGL10.EGL_RED_SIZE, configAttribs[0], 
                    EGL10.EGL_GREEN_SIZE, configAttribs[1],
                    EGL10.EGL_BLUE_SIZE, configAttribs[2],
                    EGL10.EGL_ALPHA_SIZE, configAttribs[3],
                    EGL10.EGL_DEPTH_SIZE, configAttribs[4],
                    EGL10.EGL_STENCIL_SIZE,configAttribs[5],
                    EGL10.EGL_RENDERABLE_TYPE, 4, //EGL_OPENGL_ES2_BIT
                    EGL10.EGL_NONE
                                };
            int[] choosedConfigNum = new int[1];
            
            egl.eglChooseConfig(display, EGLattribs, configs, numConfigs[0], choosedConfigNum);
            if(choosedConfigNum[0]>0)
            {
                return selectConfig(egl, display, configs, configAttribs);
            }
            else
            {
                int[] defaultEGLattribs = {
                        EGL10.EGL_RED_SIZE, 5, 
                        EGL10.EGL_GREEN_SIZE, 6,
                        EGL10.EGL_BLUE_SIZE, 5,
                        EGL10.EGL_ALPHA_SIZE, 0,
                        EGL10.EGL_DEPTH_SIZE, 0,
                        EGL10.EGL_STENCIL_SIZE,0,
                        EGL10.EGL_RENDERABLE_TYPE, 4, //EGL_OPENGL_ES2_BIT
                        EGL10.EGL_NONE
                                    };
                int[] defaultEGLattribsAlpha = {
                        EGL10.EGL_RED_SIZE, 4, 
                        EGL10.EGL_GREEN_SIZE, 4,
                        EGL10.EGL_BLUE_SIZE, 4,
                        EGL10.EGL_ALPHA_SIZE, 4,
                        EGL10.EGL_DEPTH_SIZE, 0,
                        EGL10.EGL_STENCIL_SIZE,0,
                        EGL10.EGL_RENDERABLE_TYPE, 4, //EGL_OPENGL_ES2_BIT
                        EGL10.EGL_NONE
                                    };
                int[] attribs = null;
                //choose one can use
                if(this.configAttribs[3] == 0)
                {
                    egl.eglChooseConfig(display, defaultEGLattribs, configs, numConfigs[0], choosedConfigNum);
                    attribs = new int[]{5,6,5,0,0,0};
                }
                else
                {
                    egl.eglChooseConfig(display, defaultEGLattribsAlpha, configs, numConfigs[0], choosedConfigNum);
                    attribs = new int[]{4,4,4,4,0,0};
                }
                if(choosedConfigNum[0] > 0)
                {
                    return selectConfig(egl, display, configs, attribs);
                }
                else
                {
                    Log.e("EnigmaBridge", "Can not select an EGLConfig for rendering.");
                    return null;
                }
            }
        }
        Log.e("EnigmaBridge", "Can not select an EGLConfig for rendering.");
        return null;
    }

    public void printConfig(EGL10 egl, EGLDisplay display,  EGLConfig config) {  
        int[] attributes = {  
                EGL10.EGL_BUFFER_SIZE,  
                EGL10.EGL_ALPHA_SIZE,  
                EGL10.EGL_BLUE_SIZE,  
                EGL10.EGL_GREEN_SIZE,  
                EGL10.EGL_RED_SIZE,  
                EGL10.EGL_DEPTH_SIZE,  
                EGL10.EGL_STENCIL_SIZE,  
                EGL10.EGL_CONFIG_CAVEAT,  
                EGL10.EGL_CONFIG_ID,  
                EGL10.EGL_LEVEL,  
                EGL10.EGL_MAX_PBUFFER_HEIGHT,  
                EGL10.EGL_MAX_PBUFFER_PIXELS,  
                EGL10.EGL_MAX_PBUFFER_WIDTH,  
                EGL10.EGL_NATIVE_RENDERABLE,  
                EGL10.EGL_NATIVE_VISUAL_ID,  
                EGL10.EGL_NATIVE_VISUAL_TYPE,  
                0x3030, // EGL10.EGL_PRESERVED_RESOURCES,  
                EGL10.EGL_SAMPLES,  
                EGL10.EGL_SAMPLE_BUFFERS,  
                EGL10.EGL_SURFACE_TYPE,  
                EGL10.EGL_TRANSPARENT_TYPE,  
                EGL10.EGL_TRANSPARENT_RED_VALUE,  
                EGL10.EGL_TRANSPARENT_GREEN_VALUE,  
                EGL10.EGL_TRANSPARENT_BLUE_VALUE,  
                0x3039, // EGL10.EGL_BIND_TO_TEXTURE_RGB,  
                0x303A, // EGL10.EGL_BIND_TO_TEXTURE_RGBA,  
                0x303B, // EGL10.EGL_MIN_SWAP_INTERVAL,  
                0x303C, // EGL10.EGL_MAX_SWAP_INTERVAL,  
                EGL10.EGL_LUMINANCE_SIZE,  
                EGL10.EGL_ALPHA_MASK_SIZE,  
                EGL10.EGL_COLOR_BUFFER_TYPE,  
                EGL10.EGL_RENDERABLE_TYPE,  
                0x3042 // EGL10.EGL_CONFORMANT  
        };  
        String[] names = {  
                "EGL_BUFFER_SIZE",  
                "EGL_ALPHA_SIZE",  
                "EGL_BLUE_SIZE",  
                "EGL_GREEN_SIZE",  
                "EGL_RED_SIZE",  
                "EGL_DEPTH_SIZE",  
                "EGL_STENCIL_SIZE",  
                "EGL_CONFIG_CAVEAT",  
                "EGL_CONFIG_ID",  
                "EGL_LEVEL",  
                "EGL_MAX_PBUFFER_HEIGHT",  
                "EGL_MAX_PBUFFER_PIXELS",  
                "EGL_MAX_PBUFFER_WIDTH",  
                "EGL_NATIVE_RENDERABLE",  
                "EGL_NATIVE_VISUAL_ID",  
                "EGL_NATIVE_VISUAL_TYPE",  
                "EGL_PRESERVED_RESOURCES",  
                "EGL_SAMPLES",  
                "EGL_SAMPLE_BUFFERS",  
                "EGL_SURFACE_TYPE",  
                "EGL_TRANSPARENT_TYPE",  
                "EGL_TRANSPARENT_RED_VALUE",  
                "EGL_TRANSPARENT_GREEN_VALUE",  
                "EGL_TRANSPARENT_BLUE_VALUE",  
                "EGL_BIND_TO_TEXTURE_RGB",  
                "EGL_BIND_TO_TEXTURE_RGBA",  
                "EGL_MIN_SWAP_INTERVAL",  
                "EGL_MAX_SWAP_INTERVAL",  
                "EGL_LUMINANCE_SIZE",  
                "EGL_ALPHA_MASK_SIZE",  
                "EGL_COLOR_BUFFER_TYPE",  
                "EGL_RENDERABLE_TYPE",  
                "EGL_CONFORMANT"  
        };  
        int[] value = new int[1];  
        for (int i = 0; i < attributes.length; i++) {  
            int attribute = attributes[i];  
            String name = names[i];  
            if ( egl.eglGetConfigAttrib(display, config, attribute, value)) {  
                Log.d("EnigmaBridge", String.format("  %s: %d\n", name, value[0]));  
            } else {  
                // Log.w(TAG, String.format("  %s: failed\n", name));  
                while (egl.eglGetError() != EGL10.EGL_SUCCESS);  
            }  
        }  
    }  

}
