package thu409b.a4over6;

import android.util.Log;

import java.net.InetAddress;
import java.net.NetworkInterface;
import java.net.SocketException;
import java.util.Enumeration;

/**
 * Created by Administrator on 2017/4/14 0014.
 */
public class GetIP {
    public static String getIpv6Address()
    {
        try
        {
            for (Enumeration<NetworkInterface> en = NetworkInterface.getNetworkInterfaces(); en.hasMoreElements();)
            {
                NetworkInterface intf = en.nextElement();
                for (Enumeration<InetAddress> enumIpAddr = intf.getInetAddresses(); enumIpAddr.hasMoreElements();)
                {
                    InetAddress inetAddress = enumIpAddr.nextElement();
                    if (!inetAddress.isLoopbackAddress())
                    {
                        String s = inetAddress.getHostAddress().toString();
                        String[] t = s.split("%");
                        return t[0];
                    }
                }
            }
        }
        catch (SocketException ex)
        {
            Log.e("get ip error: ", ex.toString());
        }
        return null;
    }
}
