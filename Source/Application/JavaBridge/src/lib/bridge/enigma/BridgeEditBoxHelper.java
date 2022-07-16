/****************************************************************************
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2013-2016 Chukong Technologies Inc.

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
import android.content.res.Resources;
import android.graphics.Color;
import android.graphics.Typeface;
import android.os.Looper;
import android.text.Editable;
import android.text.InputType;
import android.text.TextWatcher;
import android.util.Log;
import android.util.SparseArray;
import android.util.TypedValue;
import android.view.Gravity;
import android.view.KeyEvent;
import android.view.View;
import android.view.inputmethod.EditorInfo;
import android.view.inputmethod.InputMethodManager;
import android.widget.FrameLayout;
import android.widget.TextView;


public class BridgeEditBoxHelper {
    private static final String TAG = "EnigmaAndroid";
    private static ApplicationBridgeActivity mBridgeActivity;
    private static ResizeLayout mFrameLayout;

    private static SparseArray<BridgeEditBox> mEditBoxArray;
    private static int mViewTag = 0;

    //Call native methods
    private static native void editBoxEditingDidBegin(int index);
    public static void __editBoxEditingDidBegin(int index){
        editBoxEditingDidBegin(index);
    }

    private static native void editBoxEditingChanged(int index, String text);
    public static void __editBoxEditingChanged(int index, String text){
        editBoxEditingChanged(index, text);
    }

    private static native void editBoxEditingDidEnd(int index, String text);
    public static void __editBoxEditingDidEnd(int index, String text){
        editBoxEditingDidEnd(index, text);
    }


    public BridgeEditBoxHelper(ResizeLayout layout) {
        BridgeEditBoxHelper.mFrameLayout = layout;

        BridgeEditBoxHelper.mBridgeActivity = (ApplicationBridgeActivity) ApplicationBridgeActivity.getContext();
        BridgeEditBoxHelper.mEditBoxArray = new SparseArray<BridgeEditBox>();
    }

    public static int convertToSP(float point){
        Resources r = mBridgeActivity.getResources();

        int convertedValue = (int)TypedValue.applyDimension(
                TypedValue.COMPLEX_UNIT_SP, point, r.getDisplayMetrics());

        return  convertedValue;

    }

    public static int createEditBox(final int left, final int top, final int width, final int height, final float scaleX) {
        final int index = mViewTag;
        mBridgeActivity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                final BridgeEditBox editBox = new BridgeEditBox(mBridgeActivity);
                editBox.setFocusable(true);
                editBox.setFocusableInTouchMode(true);
                editBox.setInputFlag(5); //kEditBoxInputFlagLowercaseAllCharacters
                editBox.setInputMode(6); //kEditBoxInputModeSingleLine
                editBox.setReturnType(0);  //kKeyboardReturnTypeDefault
                editBox.setHintTextColor(Color.GRAY);
                editBox.setVisibility(View.VISIBLE); //(View.INVISIBLE);
                editBox.setBackgroundColor(Color.BLACK); //(Color.TRANSPARENT);
                editBox.setTextColor(Color.WHITE);
                editBox.setSingleLine();
                editBox.setOpenGLViewScaleX(scaleX);
                Resources r = mBridgeActivity.getResources();
                float density =  r.getDisplayMetrics().density;
                int paddingBottom = (int)(height * 0.33f / density);
                paddingBottom = convertToSP(paddingBottom  - 5 * scaleX / density);
                paddingBottom = paddingBottom / 2;
                int paddingTop = paddingBottom;
                int paddingLeft = (int)(5 * scaleX / density);
                paddingLeft = convertToSP(paddingLeft);

                editBox.setPadding(paddingLeft,paddingTop, 0, paddingBottom);


                FrameLayout.LayoutParams lParams = new FrameLayout.LayoutParams(
                        FrameLayout.LayoutParams.WRAP_CONTENT,
                        FrameLayout.LayoutParams.WRAP_CONTENT);

                lParams.leftMargin = left;
                lParams.topMargin = top;
                lParams.width = width;
                lParams.height = height;
                lParams.gravity = Gravity.TOP | Gravity.LEFT;

                mFrameLayout.addView(editBox, lParams);

                editBox.addTextChangedListener(new TextWatcher() {
                    @Override
                    public void beforeTextChanged(CharSequence s, int start, int count, int after) {
                    }

                    @Override
                    public void onTextChanged(CharSequence s, int start, int before, int count) {
                        //The optimization can't be turn on due to unknown keyboard hide in some custom keyboard
//                        mFrameLayout.setEnableForceDoLayout(false);

                        // Note that we must to copy a string to prevent string content is modified
                        // on UI thread while 's.toString' is invoked at the same time.
                        final String text = new String(s.toString());
                        mBridgeActivity.runOnGLThread(new Runnable() {
                            @Override
                            public void run() {
                                BridgeEditBoxHelper.__editBoxEditingChanged(index, text);
                            }
                        });
                    }

                    @Override
                    public void afterTextChanged(Editable s) {

                    }
                });


                editBox.setOnFocusChangeListener(new View.OnFocusChangeListener() {

                    @Override
                    public void onFocusChange(View v, boolean hasFocus) {
                        if (hasFocus) {
                            mBridgeActivity.runOnGLThread(new Runnable() {
                                @Override
                                public void run() {
                                    BridgeEditBoxHelper.__editBoxEditingDidBegin(index);
                                }
                            });
                            editBox.setSelection(editBox.getText().length());
                            mFrameLayout.setEnableForceDoLayout(true);
                            //mBridgeActivity.getGLSurfaceView().setSoftKeyboardShown(true);
                            Log.d(TAG, "edit box get focus");
                        } else {
                            editBox.setVisibility(View.GONE);
                            // Note that we must to copy a string to prevent string content is modified
                            // on UI thread while 's.toString' is invoked at the same time.
                            final String text = new String(editBox.getText().toString());
                            mBridgeActivity.runOnGLThread(new Runnable() {
                                @Override
                                public void run() {
                                    BridgeEditBoxHelper.__editBoxEditingDidEnd(index, text);
                                }
                            });
                            //mBridgeActivity.hideVirtualButton();
                            mFrameLayout.setEnableForceDoLayout(false);
                            Log.d(TAG, "edit box lose focus");
                        }
                    }
                });

                editBox.setOnKeyListener(new View.OnKeyListener() {
                    public boolean onKey(View v, int keyCode, KeyEvent event) {
                        // If the event is a key-down event on the "enter" button
                        if ((event.getAction() == KeyEvent.ACTION_DOWN) &&
                                (keyCode == KeyEvent.KEYCODE_ENTER)) {
                            //if editbox doesn't support multiline, just hide the keyboard
                            if ((editBox.getInputType() & InputType.TYPE_TEXT_FLAG_MULTI_LINE) != InputType.TYPE_TEXT_FLAG_MULTI_LINE) {
                                BridgeEditBoxHelper.closeKeyboardOnUiThread(index);
                                return true;
                            }
                        }
                        return false;
                    }
                });


                editBox.setOnEditorActionListener(new TextView.OnEditorActionListener() {
                    @Override
                    public boolean onEditorAction(TextView v, int actionId, KeyEvent event) {
                        if (actionId == EditorInfo.IME_ACTION_DONE) {
                            BridgeEditBoxHelper.closeKeyboardOnUiThread(index);
                        }
                        return false;
                    }
                });

                mEditBoxArray.put(index, editBox);
            }
        });
        return mViewTag++;
    }

    public static void removeEditBox(final int index) {
        mBridgeActivity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                BridgeEditBox editBox = mEditBoxArray.get(index);
                if (editBox != null) {
                    mEditBoxArray.remove(index);
                    mFrameLayout.removeView(editBox);
                    Log.e(TAG, "remove EditBox");
                }
            }
        });
    }

    public static void setFont(final int index, final String fontName, final float fontSize){
        mBridgeActivity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                BridgeEditBox editBox = mEditBoxArray.get(index);
                if (editBox != null) {
                    Typeface tf;
                    if (!fontName.isEmpty()) {
                        /*if (fontName.endsWith(".ttf")) {
                            try {
                                tf = Cocos2dxTypefaces.get(mBridgeActivity.getContext(), fontName);
                            } catch (final Exception e) {
                                Log.e("Cocos2dxEditBoxHelper", "error to create ttf type face: "
                                        + fontName);
                                // The file may not find, use system font.
                                tf  =  Typeface.create(fontName, Typeface.NORMAL);
                            }
                        } else {*/
                            tf  =  Typeface.create(fontName, Typeface.NORMAL);
                        //}

                    }else{
                        tf = Typeface.DEFAULT;
                    }
                    // TODO: The font size is not the same across all the android devices...
                    if (fontSize >= 0){
                        float density =  mBridgeActivity.getResources().getDisplayMetrics().density;
//                        Log.e("XXX", "density is " + density);
                        editBox.setTextSize(TypedValue.COMPLEX_UNIT_SP,
                                fontSize / density );
                    }
                    editBox.setTypeface(tf);
                }
            }
        });
    }

    public static void setFontColor(final int index, final int red, final int green, final int blue, final int alpha){
        mBridgeActivity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                BridgeEditBox editBox = mEditBoxArray.get(index);
                if (editBox != null) {
                    editBox.setTextColor(Color.argb(alpha, red, green, blue));
                }
            }
        });
    }

    public static void setPlaceHolderText(final int index, final String text){
        mBridgeActivity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                BridgeEditBox editBox = mEditBoxArray.get(index);
                if (editBox != null) {
                    editBox.setHint(text);
                }
            }
        });
    }

    public static void setPlaceHolderTextColor(final int index, final int red, final int green, final int blue, final int alpha){
        mBridgeActivity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                BridgeEditBox editBox = mEditBoxArray.get(index);
                if (editBox != null) {
                    editBox.setHintTextColor(Color.argb(alpha, red, green, blue));
                }
            }
        });
    }

    public static void setMaxLength(final int index, final int maxLength) {
        mBridgeActivity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                BridgeEditBox editBox = mEditBoxArray.get(index);
                if (editBox != null) {
                    editBox.setMaxLength(maxLength);
                }
            }
        });
    }

    public static void setVisible(final int index, final boolean visible) {
        mBridgeActivity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                BridgeEditBox editBox = mEditBoxArray.get(index);
                if (editBox != null) {
                    editBox.setVisibility(visible ? View.VISIBLE : View.GONE);
                }
            }
        });
    }


    public static void setText(final int index, final String text){
        mBridgeActivity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                BridgeEditBox editBox = mEditBoxArray.get(index);
                if (editBox != null) {
                    editBox.setText(text);
                }
            }
        });
    }

    public static void setReturnType(final int index, final int returnType) {
        mBridgeActivity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                BridgeEditBox editBox = mEditBoxArray.get(index);
                if (editBox != null) {
                    editBox.setReturnType(returnType);
                }
            }
        });
    }

    public static void setInputMode(final int index, final int inputMode) {
        mBridgeActivity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                BridgeEditBox editBox = mEditBoxArray.get(index);
                if (editBox != null) {
                    editBox.setInputMode(inputMode);
                }
            }
        });
    }

    public static void setInputFlag(final int index, final int inputFlag) {
        mBridgeActivity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                BridgeEditBox editBox = mEditBoxArray.get(index);
                if (editBox != null) {
                    editBox.setInputFlag(inputFlag);
                }
            }
        });
    }


    public static void setEditBoxViewRect(final int index, final int left, final int top, final int maxWidth, final int maxHeight) {
        mBridgeActivity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                BridgeEditBox editBox = mEditBoxArray.get(index);
                if (editBox != null) {
                    editBox.setEditBoxViewRect(left, top, maxWidth, maxHeight);
                }
            }
        });
    }



    public static void openKeyboard(final int index) {

        mBridgeActivity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
               openKeyboardOnUiThread(index);
            }
        });
    }

    private static void openKeyboardOnUiThread(int index) {
        if (Looper.myLooper() != Looper.getMainLooper()) {
            Log.e(TAG, "openKeyboardOnUiThread doesn't run on UI thread!");
            return;
        }

        final InputMethodManager imm = (InputMethodManager) mBridgeActivity.getContext().getSystemService(Context.INPUT_METHOD_SERVICE);
        BridgeEditBox editBox = mEditBoxArray.get(index);
        if (null != editBox) {
            editBox.requestFocus();
            imm.showSoftInput(editBox, 0);
            //mBridgeActivity.getGLSurfaceView().setSoftKeyboardShown(true);
        }
    }

    private static void closeKeyboardOnUiThread(int index) {
        if (Looper.myLooper() != Looper.getMainLooper()) {
            Log.e(TAG, "closeKeyboardOnUiThread doesn't run on UI thread!");
            return;
        }
        
        final InputMethodManager imm = (InputMethodManager) mBridgeActivity.getContext().getSystemService(Context.INPUT_METHOD_SERVICE);
        BridgeEditBox editBox = mEditBoxArray.get(index);
        if (null != editBox) {
            imm.hideSoftInputFromWindow(editBox.getWindowToken(), 0);
            //mBridgeActivity.getGLSurfaceView().setSoftKeyboardShown(false);
            mBridgeActivity.getGLSurfaceView().requestFocus();
            // can take effect after GLSurfaceView has focus
            //mBridgeActivity.hideVirtualButton();
        }
    }

    // Note that closeKeyboard will be invoked on GL thread
    public static void closeKeyboard(final int index) {
        mBridgeActivity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                closeKeyboardOnUiThread(index);
            }
        });
    }
}
