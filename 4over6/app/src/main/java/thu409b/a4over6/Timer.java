package thu409b.a4over6;

import android.os.Environment;
import java.io.BufferedInputStream;
import java.io.File;
import java.io.FileInputStream;

/**
 * Created by Administrator on 2017/4/16 0016.
 */
public class Timer {
    public static void runTimer() {
        File extDir = Environment.getExternalStorageDirectory();
        File file = new File(extDir, "cmd_pipe");
        int readlen = -1;
        byte[] readBuf = new byte[1024];
        while(true) {
            MainActivity.handler.post(MainActivity.runnable);
            if(!MainActivity.flag) {
                try {
                    FileInputStream fileInputStream = new FileInputStream(file);
                    BufferedInputStream in = new BufferedInputStream(fileInputStream);
                    readlen = in.read(readBuf);
                    in.close();
                }
                catch (Exception e) {
                    e.printStackTrace();
                }
                if(readlen > 0) {
                    //check ip in readBuf todo
                    if(true) {

                    }
                }
            }
            try{
                Thread.sleep(1000);
            }
            catch (Exception e) {
                e.printStackTrace();
            }
        }
    }
}
