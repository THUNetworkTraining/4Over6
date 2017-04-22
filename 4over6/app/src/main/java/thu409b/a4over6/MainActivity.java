package thu409b.a4over6;

import android.content.Intent;
import android.graphics.Color;
import android.net.VpnService;
import android.os.Environment;
import android.os.Handler;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.support.v7.view.menu.MenuAdapter;
import android.util.Log;
import android.widget.LinearLayout;
import android.widget.TextView;

import org.achartengine.GraphicalView;
import org.achartengine.chart.PointStyle;
import org.achartengine.model.TimeSeries;
import org.achartengine.model.XYMultipleSeriesDataset;
import org.achartengine.renderer.XYMultipleSeriesRenderer;
import org.achartengine.renderer.XYSeriesRenderer;
import org.json.JSONObject;

import java.io.BufferedOutputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.util.Date;

import org.achartengine.ChartFactory;

public class MainActivity extends AppCompatActivity {

    //read ip(false) or traffic
    public static boolean flag = false;
    public static boolean flag1 = false;
    private TextView ti;
    public static Handler handler;
    public static File extDir;
    public static String IPv4Addr;
    public static String[] dns;
    public static long readFlow, writeFlow, readTimes, writeTimes = 0;
    private static GraphicalView chart;
    private static XYMultipleSeriesDataset dataset1;
    public static TimeSeries series1;
    private static long addX;
    private static long addY = 0;
    private static final int N = 30;
    private static Date[] xcache = new Date[N];
    private static long[] ycache = new long[N];
    private static TextView tv_ipv4Addr, tv_writeTimes, tv_writeFlow, tv_readIimes, tv_readFlow;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        dns = new String[3];
        //test information shower
        ti = (TextView)findViewById(R.id.testInfo);
        ti.setText("");

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
                //ti.setText("ipv6 address: " + hostIp);
                TextView tv_ipv6Addr = (TextView)findViewById(R.id.tv_ipv6Addr);
                tv_ipv6Addr.setText(hostIp);
            }
        }
        tv_ipv4Addr = (TextView)findViewById(R.id.tv_ipv4Addr);
        tv_writeTimes = (TextView)findViewById(R.id.tv_writeTimes);
        tv_writeFlow = (TextView)findViewById(R.id.tv_writeFlow);
        tv_readIimes = (TextView)findViewById(R.id.tv_readTimes);
        tv_readFlow = (TextView)findViewById(R.id.tv_readFlow);

        //setup ip/tnu/flow pipe
        extDir = Environment.getExternalStorageDirectory();
        try {
            File file = new File(extDir, "IPPipe");
            file.delete();
            file.createNewFile();
            file = new File(extDir, "tnuPipe");
            file.delete();
            file.createNewFile();
            file = new File(extDir, "flowPipe");
            file.delete();
            file.createNewFile();
            file = new File(extDir, "settings.json");
            file.createNewFile();
            JSONObject jsonObject = new JSONObject();
            jsonObject.put("addr", "2402:f000:1:4417::900");
            //jsonObject.put("addr", "2402:f000:5:8301:c067:9565:1b90:7ec6");
            jsonObject.put("port", 5678);
           //jsonObject.put("port",8888);
            byte[] bytes = jsonObject.toString().getBytes();
            FileOutputStream fileOutputStream = new FileOutputStream(file);
            BufferedOutputStream out = new BufferedOutputStream(fileOutputStream);
            out.write(bytes, 0, bytes.length);
            out.flush();
            out.close();
        }
        catch(Exception e) {
            e.printStackTrace();
        }

    // Example of a call to a native method
    Thread thread0 = new Thread(new Runnable() {
        @Override
        public void run() {
            runBackEnd(extDir.toString());
        }
    });
    thread0.start();

        //run timer
        handler = new Handler();
        Thread thread = new Thread(new Runnable() {
            @Override
            public void run() {
                Timer.runTimer();
            }
        });
        thread.start();
        try {
            thread.join();
        } catch (Exception e) {
            e.printStackTrace();
        }
        Log.i("4over6","frontend " +  "wait for ip");
        Intent intent = VpnService.prepare(this);
        if (intent != null) {
            startActivityForResult(intent, 0);
        } else {
            onActivityResult(0, RESULT_OK, null);
        }

        LinearLayout linearLayout = (LinearLayout)findViewById(R.id.linearLayout);
        chart = ChartFactory.getTimeChartView(this, getDataset(), getRenderer(), "HH:mm:ss");
        linearLayout.addView(chart);
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

    public static Runnable runnable0 = new Runnable() {
        @Override
        public void run() {
            tv_ipv4Addr.setText(MainActivity.IPv4Addr);
        }
    };

    public static Runnable runnable = new Runnable() {
        //private int runTime = 0;
        @Override
        public void run() {
            //runTime ++;
            //ti.setText("timer run " + runTime + "s");
            //ti.setText("get ipv4 address: "+IPv4Addr);
            //ti.setText("write times : " + Long.toString(writeTimes) + "\n");
            //ti.append("write flow : " + Long.toString(writeFlow) + "\n");
            //ti.append("read times : " + Long.toString(readTimes) + "\n");
            //ti.append("read flow : " + Long.toString(readFlow));
            tv_writeTimes.setText(Long.toString(MainActivity.writeTimes));
            tv_writeFlow.setText(Long.toString(MainActivity.writeFlow));
            tv_readIimes.setText(Long.toString(MainActivity.readTimes));
            tv_readFlow.setText(Long.toString(MainActivity.readFlow));
            MainActivity.updateChart();
        }
    };

    @Override
    protected void onActivityResult(int request, int result, Intent data) {
        if (result == RESULT_OK) {
            Intent intent = new Intent(this, MyVpnService.class);
            startService(intent);
            flag = true;
            Thread thread = new Thread(new Runnable() {
                @Override
                public void run() {
                    Timer.runTimer();
                }
            });
            thread.start();
        }
    }

    private XYMultipleSeriesDataset getDataset() {
        dataset1 = new XYMultipleSeriesDataset();
        final int nr = 10;
        long value = new Date().getTime();
        series1 = new TimeSeries("Demo series 1");
        for (int k = 0; k < nr; k++) {
            series1.add(new Date(value+k*1000), 0);
        }
        dataset1.addSeries(series1);
        return dataset1;
    }

    private XYMultipleSeriesRenderer getRenderer() {
        XYMultipleSeriesRenderer renderer = new XYMultipleSeriesRenderer();
        renderer.setChartTitle("实时流量");//标题
        renderer.setChartTitleTextSize(20);
        renderer.setXTitle("时间");    //x轴说明
        renderer.setAxisTitleTextSize(16);
        renderer.setAxesColor(Color.BLACK);
        renderer.setLabelsTextSize(15);    //数轴刻度字体大小
        renderer.setLabelsColor(Color.BLACK);
        renderer.setLegendTextSize(15);    //曲线说明
        renderer.setXLabelsColor(Color.BLACK);
        renderer.setYLabelsColor(0,Color.BLACK);
        renderer.setShowLegend(false);
        renderer.setMargins(new int[] {20, 30, 100, 0});
        XYSeriesRenderer r = new XYSeriesRenderer();
        r.setColor(Color.BLUE);
        r.setChartValuesTextSize(15);
        r.setChartValuesSpacing(3);
        r.setPointStyle(PointStyle.CIRCLE);
        r.setFillBelowLine(true);
        r.setFillBelowLineColor(Color.WHITE);
        r.setFillPoints(true);
        renderer.addSeriesRenderer(r);
        renderer.setMarginsColor(Color.WHITE);
        renderer.setPanEnabled(false,false);
        renderer.setShowGrid(true);
        //renderer.setYAxisMax(50);
        //renderer.setYAxisMin(-30);
        renderer.setInScroll(true);  //调整大小
        return renderer;
    }

    public static void updateChart() {
        int length = series1.getItemCount();
        if(length >= N) length = N;
        addY = readFlow;
        addX = new Date().getTime();

        //将前面的点放入缓存
        for (int i = 0; i < length; i++) {
            xcache[i] =  new Date((long)series1.getX(i));
            ycache[i] = (int) series1.getY(i);
        }
        series1.clear();
        series1.add(new Date(addX), addY);
        for (int k = 0; k < length; k++) {
            series1.add(xcache[k], ycache[k]);
        }
        //在数据集中添加新的点集
        dataset1.removeSeries(series1);
        dataset1.addSeries(series1);
        //曲线更新
        chart.invalidate();
    }
}
