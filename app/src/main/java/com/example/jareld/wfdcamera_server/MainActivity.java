package com.example.jareld.wfdcamera_server;

import android.Manifest;
import android.app.ProgressDialog;
import android.content.DialogInterface;
import android.content.IntentFilter;
import android.content.pm.PackageManager;
import android.media.MediaPlayer;
import android.net.wifi.WpsInfo;
import android.net.wifi.p2p.WifiP2pConfig;
import android.net.wifi.p2p.WifiP2pDevice;
import android.net.wifi.p2p.WifiP2pDeviceList;
import android.net.wifi.p2p.WifiP2pInfo;
import android.net.wifi.p2p.WifiP2pManager;
import android.os.AsyncTask;
import android.os.Bundle;
import android.os.Handler;
import android.support.annotation.NonNull;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.ContextCompat;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.LinearLayoutManager;
import android.support.v7.widget.RecyclerView;
import android.util.Log;
import android.view.KeyEvent;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;

import java.util.ArrayList;
import java.util.Collection;
import java.util.HashMap;
import java.util.List;

import rx.Subscription;
import rx.functions.Action1;

public class MainActivity
        extends AppCompatActivity
        implements View.OnClickListener, SurfaceHolder.Callback
{
    private static final String TAG = "ServerActivity";
    private RecyclerView                          mRcyc_devices;
    private Button                                mBtn_stop_connect;
    private Button                                mBtn_stop_search;
    private Button                                mBtn_search;
    private IntentFilter                          mFilter;
    private WifiP2pManager                        mManager;
    private WifiP2pManager.Channel                mChannel;
    private WifiDerectBroadcastReceiver           mReceiver;
    private WifiP2pManager.PeerListListener       mPeerListListener;
    private WifiP2pManager.ConnectionInfoListener mConnectionInfoListener;
    private List<HashMap<String, String>>         mPeerLists;
    private WifiP2pInfo                           mInfo;
    private Button                                mBtn_become_server;
    private boolean isServer = true;
    private String beforeConnectDevice;
    private static final int MY_WRITE_EXTERNAL_STORAGE = 123;
    private Button mBtn_recevier_data;
    private ProgressDialog discoverProgressDialog   = null;
    private ProgressDialog connectingProgressDialog = null;
    private Subscription mSubscription;
    private TextView     mTv_recevie_file;
    private   Collection<WifiP2pDevice> mDeviceList  ;
    private static SurfaceHolder mSurfaceHolder;
    private static SurfaceView mSurfaceView;
    private View mLayout;
    private CameraDataReciverTask mCameraDataReciverTask;

    private void initView() {
        mRcyc_devices = (RecyclerView) findViewById(R.id.devices_server);
        mBtn_search = (Button) findViewById(R.id.search_device_server);
        mBtn_stop_connect = (Button) findViewById(R.id.stop_connect_server);
        mBtn_stop_search = (Button) findViewById(R.id.stop_search_device_server);
        mBtn_become_server = (Button) findViewById(R.id.become_server);
        mBtn_recevier_data = (Button) findViewById(R.id.recevie_data);
        mTv_recevie_file = (TextView) findViewById(R.id.tv_trans_file_server);
        mSurfaceView = (SurfaceView) findViewById(R.id.surface_view);
        mSurfaceHolder = mSurfaceView.getHolder();
        mSurfaceHolder.addCallback(this);
    }

    public static void playVideo(){
        mSurfaceView.setVisibility(View.VISIBLE);
        Thread thread = new Thread(){
            @Override
            public void run() {
                super.run();
                Log.d(TAG, "[lyc] run: before playVideo");
                VideoPlayer.play(mSurfaceHolder.getSurface());
                Log.d(TAG, "[lyc] run: after playVideo");
                RxBus.getInstance().post(new UserEvent(1 , "stopvideo"));
            }
        };
        thread.setPriority(Thread.MAX_PRIORITY);  // park.xu 20170524
        thread.start();
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        getSupportActionBar().hide();
        getWindow().getDecorView()
                   .setSystemUiVisibility(View.SYSTEM_UI_FLAG_HIDE_NAVIGATION);
        mLayout = getLayoutInflater().from(this)
                                     .inflate(R.layout.activity_main, null);
        //mLayout.setSystemUiVisibility(View.SYSTEM_UI_FLAG_HIDE_NAVIGATION | View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY);
        this.getWindow()
            .setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,
                      WindowManager.LayoutParams.FLAG_FULLSCREEN);
        setContentView(mLayout);
        checKStoragePermission();
        initView();
        initFilter();
        initReceiver();
        initEvent();
        initRxBus();

        mLayout.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Log.d(TAG, "onClick: layout" + mLayout.getSystemUiVisibility());
                if (mLayout.getSystemUiVisibility() == View.SYSTEM_UI_FLAG_VISIBLE) {
                    mLayout.setSystemUiVisibility(View.SYSTEM_UI_FLAG_HIDE_NAVIGATION | View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY);
                } else {
                    mLayout.setSystemUiVisibility(View.SYSTEM_UI_FLAG_VISIBLE);
                }
            }
        });
        initSufaceView();
        //初始化的时候吧服务器

        mManager.createGroup(mChannel, new WifiP2pManager.ActionListener() {
            @Override
            public void onSuccess() {

            }

            @Override
            public void onFailure(int reason) {

            }
        });
        //每一次都去 断开连接一下
    mManager.discoverPeers(mChannel, new WifiP2pManager.ActionListener() {
        @Override
        public void onSuccess() {

        }

        @Override
        public void onFailure(int reason) {

        }
    });
    }

    private void checKStoragePermission() {
        if (ContextCompat.checkSelfPermission(this,
                                              Manifest.permission.WRITE_EXTERNAL_STORAGE)
                != PackageManager.PERMISSION_GRANTED && ContextCompat.checkSelfPermission(this,
                                                                                          Manifest.permission.CAMERA)
                != PackageManager.PERMISSION_GRANTED) {
            ActivityCompat.requestPermissions(this,
                                              new String[]{Manifest.permission.WRITE_EXTERNAL_STORAGE,Manifest.permission.CAMERA},
                                              MY_WRITE_EXTERNAL_STORAGE);
        }else{

        }
    }

    @Override
    public void onRequestPermissionsResult(int requestCode,
                                           @NonNull String[] permissions,
                                           @NonNull int[] grantResults)
    {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);
        //处理回调
        switch (requestCode){
            case MY_WRITE_EXTERNAL_STORAGE:
                if(grantResults.length > 0 && grantResults[0]==PackageManager.PERMISSION_GRANTED && grantResults[1]==PackageManager.PERMISSION_GRANTED){
                }else{
                    finish();
                }
                break;
        }
    }

    private void initSufaceView() {
        String name = Thread.currentThread().getName();
        Log.d(TAG,"Thread name:" + name);
        VideoPlayer.jni_ffmpeg_setVersion();
    }

    String preTransFileName = null;

    private void initRxBus() {
        mSubscription = RxBus.getInstance()
                             .toObserverable(UserEvent.class)
                             .subscribe(new Action1<UserEvent>() {
                                 @Override
                                 public void call(final UserEvent userEvent) {
                                     long fileLength = 0;
                                     Log.d(TAG, "[lyc] call: receive message:" + userEvent.getName());

                                     switch (userEvent.getName()) {
                                         case "disconnect":
                                             handler.post(new Runnable() {
                                                 @Override
                                                 public void run() {
                                                     mBtn_recevier_data.setText("服务器状态:服务器已经被移除");
                                                     mBtn_search.setText("点击创建服务器");
                                                     mBtn_search.setClickable(true);
                                                 }
                                             });
                                             break;
                                         case "hasReceived":
                                             handler.post(new Runnable() {
                                                 @Override
                                                 public void run() {
                                                     if (userEvent.getFileName() != null) {
                                                         mTv_recevie_file.setText("传输文件的状态：传输完成:" + "文件为" + userEvent.getFileName());
                                                         preTransFileName = userEvent.getFileName();
                                                     } else {
                                                         mTv_recevie_file.setText("传输文件的状态：传输完成:");
                                                     }
                                                 }
                                             });
                                             break;
                                         case "serverReceiving":
                                             handler.post(new Runnable() {
                                                 @Override
                                                 public void run() {
                                                     mTv_recevie_file.setText("传输文件的状态：接受文件中");
                                                     //重新执行一次等待的任务
                                                 }
                                             });
                                             break;
                                         case "waitingConnect":
                                             handler.post(new Runnable() {
                                                 @Override
                                                 public void run() {
                                                     if (preTransFileName != null) {
                                                         mTv_recevie_file.setText(
                                                                 "传输文件的状态：等待连接中(上一次传输完成文件为：" + preTransFileName + ")");
                                                     } else {
                                                         mTv_recevie_file.setText("传输文件的状态：等待连接中");

                                                     }
                                                 }
                                             });
                                             break;
                                         case "stopvideo":
                                             handler.post(new Runnable() {
                                                 @Override
                                                 public void run() {
                                                     Log.d(TAG, "[lyc] stopvideo - set view gone");
                                                     mSurfaceView.setVisibility(View.GONE);
                                                 }
                                             });

                                            // park.xu 20170524   When stop video, we must cancel data receiver task.
                                            {
                                                Log.d(TAG, "mCameraDataReciverTask status:" + mCameraDataReciverTask.getStatus());
                                                if(mCameraDataReciverTask != null && mCameraDataReciverTask.getStatus() == AsyncTask.Status.RUNNING){
                                                    mCameraDataReciverTask.cancel(true);
                                                }
                                            }

                                             mManager.cancelConnect(mChannel, new WifiP2pManager.ActionListener() {
                                                 @Override
                                                 public void onSuccess() {
                                                    Log.d(TAG, "cancelConnect success");
                                                 }

                                                 @Override
                                                 public void onFailure(int reason) {
                                                     Log.d(TAG, "cancelConnect failure");
                                                 }
                                             });

                                             mManager.createGroup(mChannel, new WifiP2pManager.ActionListener() {
                                                 @Override
                                                 public void onSuccess() {
                                                     Log.d(TAG, "createGroup success");
                                                 }

                                                 @Override
                                                 public void onFailure(int reason) {
                                                     Log.d(TAG, "createGroup failure");
                                                 }
                                             });
                                             break;
                                     }
                                 }
                             });
    }

    private void initReceiver() {
        mManager = (WifiP2pManager) getSystemService(WIFI_P2P_SERVICE);
        mChannel = mManager.initialize(this, getMainLooper(), null);
        mPeerLists = new ArrayList<HashMap<String, String>>();
        //这个是申请到列表后的回调
        mPeerListListener = new WifiP2pManager.PeerListListener() {
            @Override
            public void onPeersAvailable(WifiP2pDeviceList peers) {
                Log.d(TAG, "onPeersAvailable" + peers.getDeviceList().size());
                if (mPeerLists != null) {
                    mPeerLists.clear();
                }

                if (discoverProgressDialog != null && discoverProgressDialog.isShowing()) {
                    discoverProgressDialog.dismiss();
                }

                //这个是申请到列表后的回调
                mDeviceList = peers.getDeviceList();
                for (WifiP2pDevice wifiP2pDevice : mDeviceList) {
                    HashMap<String, String> map = new HashMap<>();
                    map.put("name", wifiP2pDevice.deviceName);
                    map.put("address", wifiP2pDevice.deviceAddress);
                    mPeerLists.add(map);
                }

                DeviceAdapter adapter = new DeviceAdapter(mPeerLists, isServer);
                mRcyc_devices.setAdapter(adapter);
                mRcyc_devices.setLayoutManager(new LinearLayoutManager(MainActivity.this));
                adapter.setOnItemButtonClickConnectListener(new DeviceAdapter.ItemButtonClickConnectListener() {
                    @Override
                    public void onItemButtonClickConnectListener(int position) {
                        //条目被点击的时候  需要连接
                        //                        LogUtils.logInfo(TAG,
                        //                                         "onItemButtonClickConnectListener",
                        //                                         mPeerLists.get(position)
                        //                                                   .get("name"));
                        //                        createConnet(mPeerLists.get(position)
                        //                                               .get("name"),
                        //                                     mPeerLists.get(position)
                        //                                               .get("address"));
                    }
                });
            }
        };

        //这是申请连接后的回调
        mConnectionInfoListener = new WifiP2pManager.ConnectionInfoListener() {
            @Override
            public void onConnectionInfoAvailable(WifiP2pInfo info) {
                //这是申请连接后的回调
                mInfo = info;

                Log.d(TAG, "[lyc] onConnectionInfoAvailable"  + mInfo.groupOwnerAddress + "info.isGroupOwner" + info.isGroupOwner);

                ///192.168.49.1info.isGroupOwnertrue
                if (info.groupFormed && info.isGroupOwner) {

                    //说明是服务端
                    Log.d(TAG, "[lyc] onConnectionInfoAvailable: This is server, accept data");

                    if (mDeviceList == null) {
                        Log.d(TAG, "onConnectionInfoAvailable:mDeviceList == null ");
                        mBtn_recevier_data.setText("服务器状态:已经创建好服务器（等待连接）:" + info.groupOwnerAddress);
                    } else {
                        Log.d(TAG, "[lyc] onConnectionInfoAvailable: "+mDeviceList.size());
                        for (WifiP2pDevice wifiP2pDevice : mDeviceList) {

                            if (wifiP2pDevice.status == WifiP2pDevice.CONNECTED) {
                                beforeConnectDevice = wifiP2pDevice.deviceName;
                                //说明这个连接到了：
                                mBtn_recevier_data.setText("服务器状态:连接成功，服务器（IP）:"  + "申请连接设备：" + wifiP2pDevice.deviceName);
                                Log.d(TAG,
                                      "[lyc] onConnectionInfoAvailable: " + wifiP2pDevice.deviceName + "::" + wifiP2pDevice.status);
                            }
                        }

                                handler.postDelayed(new Runnable() {
                                    @Override
                                    public void run() {
                                        mBtn_recevier_data.performClick();
                                    }
                                }, 500);

                        //到了这里说明  一个也没有连接上
                        mBtn_recevier_data.setText("服务器状态:已经创建好服务器（等待连接）:" + info.groupOwnerAddress);
                    }

                    mBtn_search.setText("服务器已经创建");
                    mBtn_search.setClickable(false);
                    {  // The asyncTask just running once, so we must create a new task again when it is finished.
                        if((mCameraDataReciverTask != null) && (mCameraDataReciverTask.getStatus() != AsyncTask.Status.RUNNING)) {
                            Log.d(TAG, "closeServerScket");
                            mCameraDataReciverTask.colseServerSocket();
                            mCameraDataReciverTask = null;
                        }

                        if(mCameraDataReciverTask == null) {
                            mCameraDataReciverTask = new CameraDataReciverTask();
                            mCameraDataReciverTask.executeOnExecutor(AsyncTask.THREAD_POOL_EXECUTOR);
                        }
                        Log.d(TAG, "[lyc] onConnectionInfoAvailable: mCameraDataReciverTask status" + mCameraDataReciverTask.getStatus());
                    }
                /*
                    if(mCameraDataReciverTask == null) {
                        mCameraDataReciverTask = new CameraDataReciverTask();

                        mCameraDataReciverTask.executeOnExecutor(AsyncTask.THREAD_POOL_EXECUTOR);
                        Log.d(TAG, "[lyc] onConnectionInfoAvailable: mCameraDataReciverTask == null");

                    }else{
                        Log.d(TAG, "[lyc] onConnectionInfoAvailable: mCameraDataReciverTask status"+mCameraDataReciverTask.getStatus());

                        if(mCameraDataReciverTask.getStatus() == AsyncTask.Status.RUNNING){
                            Log.d(TAG, "[lyc] mCameraDataReceiverTask status:RUNNING");

                        }

                    }
                */
                } else if (info.groupFormed) {
                    //说明是客户端
                    Log.d(TAG, "[lyc] onConnectionInfoAvailable: This is client, send data");
                }
            }
        };
        mReceiver = new WifiDerectBroadcastReceiver(mManager,
                                                    mChannel,
                                                    this,
                                                    mPeerListListener,
                                                    mConnectionInfoListener);
    }

    private void createConnet(String name, final String address) {
        if (connectingProgressDialog != null && connectingProgressDialog.isShowing()) {
            connectingProgressDialog.dismiss();
        }
        connectingProgressDialog = ProgressDialog.show(this, "连接设备", "连接中:" + address, true, true,
                                                       // cancellable
                                                       new DialogInterface.OnCancelListener() {
                                                           @Override
                                                           public void onCancel(DialogInterface dialog)
                                                           {
                                                               mBtn_stop_connect.performClick();
                                                           }
                                                       });

        WifiP2pConfig config = new WifiP2pConfig();
        config.deviceAddress = address;
        config.wps.setup = WpsInfo.PBC;
        config.groupOwnerIntent = 15;
        mManager.connect(mChannel, config, new WifiP2pManager.ActionListener() {
            @Override
            public void onSuccess() {
                if (connectingProgressDialog != null && connectingProgressDialog.isShowing()) {
                    connectingProgressDialog.dismiss();
                }
                Toast.makeText(MainActivity.this, "连接成功", Toast.LENGTH_SHORT).show();
                mBtn_recevier_data.setText("主动接受数据:连接到了P2P:" + address);
                Log.d(TAG, "onSuccess: ");
            }

            @Override
            public void onFailure(int reason) {
                Log.d(TAG, "onFailure: ");
                Toast.makeText(MainActivity.this, "连接失败", Toast.LENGTH_SHORT).show();
            }
        });
    }

    private void initFilter() {
        mFilter = new IntentFilter();
        mFilter.addAction(WifiP2pManager.WIFI_P2P_STATE_CHANGED_ACTION);
        mFilter.addAction(WifiP2pManager.WIFI_P2P_PEERS_CHANGED_ACTION);
        mFilter.addAction(WifiP2pManager.WIFI_P2P_CONNECTION_CHANGED_ACTION);
        mFilter.addAction(WifiP2pManager.WIFI_P2P_DISCOVERY_CHANGED_ACTION);
        mFilter.addAction(WifiP2pManager.WIFI_P2P_THIS_DEVICE_CHANGED_ACTION);
    }

    private void initEvent() {
        mBtn_stop_connect.setOnClickListener(this);
        mBtn_search.setOnClickListener(this);
        mBtn_stop_search.setOnClickListener(this);
        mBtn_become_server.setOnClickListener(this);
        mBtn_recevier_data.setOnClickListener(this);
        mBtn_recevier_data.setText("主动接受数据:尚未连接到P2P");
    }


    @Override
    protected void onResume() {
        super.onResume();
        registerReceiver(mReceiver, mFilter);

    }

    @Override
    public void onPause() {
        super.onPause();

    }

    @Override
    public void onBackPressed() {
        super.onBackPressed();
        mManager.removeGroup(mChannel, new WifiP2pManager.ActionListener() {
            @Override
            public void onSuccess() {

            }

            @Override
            public void onFailure(int reason) {

            }
        });
        finish();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        Log.d(TAG, "onDestroy: zheli youmeiyou ");
        unregisterReceiver(mReceiver);
        if(mCameraDataReciverTask != null){
            mCameraDataReciverTask.colseServerSocket();
        }
    }

    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.search_device_server:
                if (discoverProgressDialog != null && discoverProgressDialog.isShowing()) {
                    discoverProgressDialog.dismiss();

                }
                mManager.createGroup(mChannel, new WifiP2pManager.ActionListener() {
                    @Override
                    public void onSuccess() {

                    }

                    @Override
                    public void onFailure(int reason) {

                    }
                });
            mManager.discoverPeers(mChannel, new WifiP2pManager.ActionListener() {
                @Override
                public void onSuccess() {

                }

                @Override
                public void onFailure(int reason) {

                }
            });
                break;
            case R.id.stop_search_device_server:
                    MainActivity.playVideo();  // ?

                break;
            case R.id.stop_connect_server:
                mManager.removeGroup(mChannel, new WifiP2pManager.ActionListener() {
                    @Override
                    public void onSuccess() {

                    }

                    @Override
                    public void onFailure(int reason) {
                    }
                });
                break;
            case R.id.become_server:
                break;
            case R.id.recevie_data:
                //检查一下状态
                for (WifiP2pDevice wifiP2pDevice : mDeviceList) {
                    if (wifiP2pDevice.deviceName.equals(beforeConnectDevice)) {

                        if(wifiP2pDevice.status == WifiP2pDevice.CONNECTED){
                            return;
                        }else{
                            mBtn_recevier_data.setText("服务器状态:已经创建好服务器（等待连接）:" + mInfo.groupOwnerAddress);
                            return;
                        }
                    }
                }
                break;
            default:
                break;
        }
    }

    Handler  handler  = new Handler();
    Runnable runnable = new Runnable() {

        @Override
        public void run() {
            // handler自带方法实现定时器
            try {

                handler.postDelayed(this, 1000 * 60);

                mBtn_search.performClick();

            } catch (Exception e) {
                // TODO Auto-generated catch block
                e.printStackTrace();
                Log.d(TAG, "run(): Timmer is wrong");
            }
        }
    };

    MediaPlayer mp = new MediaPlayer();

    @Override
    public void surfaceCreated(SurfaceHolder holder) {

    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {

    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {

    }

    private class SurfaceCallback
            implements SurfaceHolder.Callback
    {
        public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
        }

        public void surfaceCreated(SurfaceHolder holder) {

        }

        public void surfaceDestroyed(SurfaceHolder holder) {
            // 界面销毁，即将跳转到另外一个界面
            if (mp.isPlaying()) {
                mp.stop();
            }
        }

    }

    public boolean onError(MediaPlayer mp, int what, int extra) {

        return false;
    }


    public class SureDevicesStatus
            implements Runnable
    {
        private WifiP2pDevice mWifiDevice;
        private WifiP2pInfo   mWifiInfo;

        public SureDevicesStatus() {}

        public SureDevicesStatus(WifiP2pDevice wifidevice, WifiP2pInfo wifiInfo) {
            this.mWifiDevice = wifidevice;
            this.mWifiInfo = wifiInfo;
        }

        @Override
        public void run() {
            Log.d(TAG, "Check the status of device's change: status=" + mWifiDevice.status);
            if (mWifiDevice.status == WifiP2pDevice.CONNECTED) {
                return;
            } else {
                mBtn_recevier_data.setText("服务器状态:已经创建好服务器（等待连接）:" + mWifiInfo.groupOwnerAddress);
                //这个状态如果做了改变就是已经离开了
            }
        }
    }

    @Override
    public boolean onKeyDown(int keyCode, KeyEvent event) {

        return super.onKeyDown(keyCode, event);
    }
}
