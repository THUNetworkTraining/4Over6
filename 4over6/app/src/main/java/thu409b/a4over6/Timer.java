package thu409b.a4over6;

import android.content.Intent;
import android.net.VpnService;
import android.os.Environment;
import android.util.Log;

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;

/**
 * Created by Administrator on 2017/4/16 0016.
 */
public class Timer {
    public static void runTimer() {
        int readlen = -1;
        byte[] readBuf = new byte[1024];
        while(true) {
            //MainActivity.handler.post(MainActivity.runnable);
            if(!MainActivity.flag) {
                File file = new File(MainActivity.extDir, "IPPipe");
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
                    MainActivity.IPv4Addr = intToIP(byteArrayToInt(readBuf, 0));
                    for(int i = 0; i < 3; i++) {
                        MainActivity.dns[i] = intToIP(byteArrayToInt(readBuf, (2 + i)*4));
                    }
                    MainActivity.handler.post(MainActivity.runnable0);
                    //Log.i("4over6", "frontend " + "get ip from back end "+MainActivity.IPv4Addr);
                    return;
                }
            } else {
                if (!MainActivity.flag1) {
                    Log.i("4over6","frontend " +  "wait tnu");
                    if (MyVpnService.tnu != null) {
                        Log.i("4over6", "ready to write tnu");
                        int fd = MyVpnService.tnu.getFd();
                        Log.i("4over6", "tnu = "+Integer.toString(fd));
                        intTobytes(fd, readBuf);
                        File file = new File(MainActivity.extDir, "tnuPipe");
                        try {
                            FileOutputStream fileOutputStream = new FileOutputStream(file);
                            BufferedOutputStream out = new BufferedOutputStream(fileOutputStream);
                            out.write(readBuf, 0, 4);
                            out.flush();
                            out.close();
                        } catch (Exception e) {
                            e.printStackTrace();
                        }
                        MainActivity.flag1 = true;
                    }
                } else {
                    File file = new File(MainActivity.extDir, "flowPipe");
                    try {
                        FileInputStream fileInputStream = new FileInputStream(file);
                        BufferedInputStream in = new BufferedInputStream(fileInputStream);
                        readlen = in.read(readBuf);
                    } catch (Exception e) {
                        e.printStackTrace();
                    }
                    if (readlen > 0) {
                        //Log.i("4over6","front end read len = " + Integer.toString(readlen));
                        MainActivity.readFlow = byteArrayToLong(readBuf, 0);
                        MainActivity.writeFlow = byteArrayToLong(readBuf, 8);
                        MainActivity.readTimes = byteArrayToLong(readBuf, 16);
                        MainActivity.writeTimes = byteArrayToLong(readBuf, 24);
                        MainActivity.handler.post(MainActivity.runnable);
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
    public static int byteArrayToInt(byte[] b,int a) {
        return   b[a + 3] & 0xFF |
                (b[a + 2] & 0xFF) << 8 |
                (b[a + 1] & 0xFF) << 16 |
                (b[a + 0] & 0xFF) << 24;
    }
    public static void intTobytes(int a,byte[] b) {
        b[3] = (byte)(a >> 24);
        b[2] = (byte)((a & 0xff0000) >> 16);
        b[1] = (byte)((a & 0xff00) >> 8);
        b[0] = (byte)(a & 0xff);
    }
    public static String intToIP(int a) {
        return Integer.toString((a >> 24)&0x000000ff) + "."
                 + Integer.toString((a & 0xff0000) >> 16) + "."
                 + Integer.toString((a & 0xff00) >> 8) +  "."
                 + Integer.toString(a & 0xff);
    }
    public static long byteArrayToLong(byte[] bb, int a) {
        return ((((long) bb[a + 7] & 0xff) << 56)
                | (((long) bb[a + 6] & 0xff) << 48)
                | (((long) bb[a + 5] & 0xff) << 40)
                | (((long) bb[a + 4] & 0xff) << 32)
                | (((long) bb[a + 3] & 0xff) << 24)
                | (((long) bb[a + 2] & 0xff) << 16)
                | (((long) bb[a + 1] & 0xff) << 8)
                | ((long) bb[a] & 0xff));
    }
}
