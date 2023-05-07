
package com.RobinGames.BattleArena;
import android.os.Bundle;
import lib.bridge.enigma.ApplicationBridgeActivity;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.BufferedOutputStream;
import java.io.FileOutputStream;
import java.io.IOException;

public class BattleArenaMain extends ApplicationBridgeActivity
{
    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        copyAssetToFolder("app.cfg");
        initBridgeCallback();
        super.onCreate(savedInstanceState);
        initAdditionalMountPath();
    }
    public native void initBridgeCallback();
    public native void initAdditionalMountPath();
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
    static {
        System.loadLibrary("BattleArena_Jni");
    }
}
