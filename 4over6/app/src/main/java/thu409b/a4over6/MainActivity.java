package thu409b.a4over6;

import android.os.Handler;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.widget.TextView;

public class MainActivity extends AppCompatActivity {

    //read ip(false) or traffic
    public static boolean flag = false;
    public static TextView ti;
    public static Handler handler;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        //test information shower
        ti = (TextView)findViewById(R.id.testInfo);
        ti.setText("nothing");

        //check wifi connection
        if(!CheckWifi.isWifiConnected(this)) {
            ti.setText("please connect to a ipv6 wifi");
        }
        else {
            //get ipv6 address
            String hostIp = GetIP.getIpv6Address();
            if(hostIp == null) {
                ti.setText("can not get ipv6 address");
            }
            else {
                ti.setText("ipv6 address: " + hostIp);
            }
        }

    // Example of a call to a native method
    TextView tv = (TextView) findViewById(R.id.sample_text);
    tv.setText(runBackEnd(this.getFilesDir().toString()));

        //timer output shower
        TextView to = (TextView)findViewById(R.id.timerOutput);
        to.setText("please start timer");

        //run timer
        handler = new Handler();
        Thread thread = new Thread(new Runnable() {
            @Override
            public void run() {
                Timer.runTimer();
            }
        });
        thread.start();

        ti.setText("flag is " + flag);
    }

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public native String runBackEnd(String curDir);

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }

    public static Runnable runnable = new Runnable() {
        private int runTime = 0;
        @Override
        public void run() {
            runTime ++;
            ti.setText("timer run " + runTime + "s");
        }
    };
}
