package thu409b.a4over6;

import android.net.VpnService;
import android.os.ParcelFileDescriptor;
import android.util.Log;

/**
 * Created by Administrator on 2017/4/17 0017.
 */
public class MyVpnService extends VpnService {
    public static ParcelFileDescriptor tnu;

    @Override
    public void onCreate() {
        super.onCreate();
        Builder builder = new Builder();
        builder.setMtu(5000);
        Log.d("vpn","v4addr "+MainActivity.IPv4Addr);
        builder.addAddress(MainActivity.IPv4Addr,32);
        builder.addRoute("0.0.0.0",0);
        for(int i = 0; i < 3; i ++) {
            builder.addDnsServer(MainActivity.dns[i]);
        }
        builder.setSession("thu409bVPN");
        tnu = builder.establish();
        Log.i("4over6","frontend " + Integer.toString(tnu.getFd()));
    }
}
