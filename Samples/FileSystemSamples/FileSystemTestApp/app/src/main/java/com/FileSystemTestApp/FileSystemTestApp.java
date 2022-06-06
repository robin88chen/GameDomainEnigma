package com.FileSystemTestApp;

import android.os.Bundle;
import lib.bridge.enigma.BridgeActivity;
import android.widget.TextView;
import android.widget.LinearLayout;
import android.view.ViewGroup.LayoutParams;
import android.widget.Button;
import android.view.View;
import android.widget.ScrollView;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.BufferedOutputStream;
import java.io.FileOutputStream;
import java.io.IOException;

public class FileSystemTestApp extends BridgeActivity
{
	boolean m_isStart = false;
    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        initBridgeCallback();
        super.onCreate(savedInstanceState);
        initAdditionalMountPath();

        copyAssetToFolder("msjh.ttf");
		copyAssetToFolder("mb7013.max");
		copyAssetToFolder("hga0010101.tga");

        LinearLayout layout = new LinearLayout(this);
		layout.setOrientation(LinearLayout.VERTICAL);
        layout.setLayoutParams(new LayoutParams(LayoutParams.FILL_PARENT, LayoutParams.FILL_PARENT));

        TextView filepathView = new TextView(this);
        LayoutParams lparams = new LayoutParams(LayoutParams.WRAP_CONTENT, LayoutParams.WRAP_CONTENT);
        filepathView.setLayoutParams(lparams);
        filepathView.setText(getDataFilePath());
        layout.addView(filepathView);

        Button startButton = new Button(this);
		startButton.setText("start");
		startButton.setOnClickListener(new View.OnClickListener() {
             public void onClick(View v) {
				if (!m_isStart)
				{
					startStdioTest();
					m_isStart = true;
					//m_update.start();
				}
             }
        });
		layout.addView(startButton);

		Button stopButton = new Button(this);
		stopButton.setText("stop");
		stopButton.setOnClickListener(new View.OnClickListener() {
             public void onClick(View v) {
				if (m_isStart)
				{
					m_isStart = false;
					/*try {
						m_update.join(10);
					} catch (InterruptedException e) {
					}*/
					stopStdioTest();
				}
             }
        });
		layout.addView(stopButton);

        setContentView(layout);
    }
    public native void initBridgeCallback();
    public native void initAdditionalMountPath();
	public native String getDataFilePath();
	public native String startStdioTest();
    public native void stopStdioTest();

	private void copyAssetToFolder(String asset_name)
	{
		try 
		{
            InputStream stream = this.getAssets().open(asset_name);
            OutputStream output = new BufferedOutputStream(new FileOutputStream(this.getFilesDir() + "/" + asset_name));

            byte data[] = new byte[1024];
            int count;

            while((count = stream.read(data)) != -1)
            {
                output.write(data, 0, count);
            }

            output.flush();
            output.close();
            stream.close();
		}
		catch(IOException e)
		{
			e.printStackTrace();
		}
	}
    /* This is the static constructor used to load the
     * library when the class is
     * loaded.
     */
    static {
        System.loadLibrary("FileSystemTest_Jni");
    }
}
