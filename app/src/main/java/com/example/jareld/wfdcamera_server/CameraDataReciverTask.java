package com.example.jareld.wfdcamera_server;

import android.os.AsyncTask;
import android.os.Handler;
import android.os.Message;
import android.util.Log;

import java.io.BufferedOutputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.net.ServerSocket;
import java.net.Socket;

/*
 *  @项目名：  TestWifiDerect 
 *  @包名：    com.example.lyc2.testwifiderect.task
 *  @文件名:   CameraDataReciverTask
 *  @创建者:   LYC2
 *  @创建时间:  2016/11/29 14:55
 *  @描述：    TODO
 */
public class CameraDataReciverTask
        extends AsyncTask<Void, Void, String>
{
    private static final String TAG            = "hyCameraDataReciverTask";
    private static final String SOCKET_ADDRESS = "LOCAL_SOCKET_ADDRESS";
    private static final int STARTVIDEO        = 1;
    private static final int STOPVIDEO         = 2;

    private Socket          mClient;
    private ServerSocket    mServerSocket;

    private  String path =  "/storage/emulated/0/test1.h264";
    private BufferedOutputStream        outputStream;
    private Handler mHandler=new Handler(){
        @Override
        public void handleMessage(Message msg) {
         switch (msg.what){
             case STARTVIDEO:
                 MainActivity.playVideo();
                 break;
             case  STOPVIDEO:
                 Log.d(TAG, "[lyc] handleMessage: send stopsvideo");
                      RxBus.getInstance().post(new UserEvent(1,"stopvideo"));

                 break;
         }
            super.handleMessage(msg);
        }
    };
    public CameraDataReciverTask() {
        createfile();
    }

    private void createfile(){
        File file = new File(path);
        if(file.exists()){
            file.delete();
        }
        try {
            outputStream = new BufferedOutputStream(new FileOutputStream(file));
        } catch (Exception e){
            e.printStackTrace();
        }
    }

    /*
         * (non-Javadoc)
         *
         * @see android.os.AsyncTask#onPostExecute(java.lang.Object)
         */
    @Override
    protected void onPostExecute(String result) {

    }

    /*
     * (non-Javadoc)
     *
     * @see android.os.AsyncTask#onPreExecute()
     */
    @Override
    protected void onPreExecute() {

    }

    @Override
    protected String doInBackground(Void... params) {

        mServerSocket = null;

        try {
            String name = Thread.currentThread()
                                .getName();

            mServerSocket = new ServerSocket(12345);
            long fileLen = 0;
            Log.d(TAG, "[lyc] doInBackground: before accept");

            mClient = mServerSocket.accept();
            Log.d(TAG, "[lyc] doInBackground: after accept");

            mClient.setKeepAlive(true);
            Log.d(TAG, "[lyc] doInBackground:");
            InputStream inputStream = mClient.getInputStream();
            //两秒之后播放文件
            int         len;
            byte[]      bys            = new byte[1024 * 100];

         boolean   isFirst = true;
            while ((len = inputStream.read(bys)) != -1) {
             //   Log.d(TAG, "[lyc] doInBackground: " + isFirst);
                if(isFirst){
                    mHandler.sendEmptyMessageDelayed(STARTVIDEO , 1000);
                    isFirst = false;
                }

                //Log.d(TAG, "[lyc] doInBackground: reading data" + len +"::" + Thread.currentThread().getName());
                outputStream.write(bys , 0 , len);
                fileLen += len;
                Log.d(TAG, "[lyc] doInBackground: write data:" + fileLen);
                outputStream.flush();
            }

            Log.d(TAG, "[lyc] doInBackground: camera connect status:" + mClient.isConnected());
        } catch (Exception e) {
            e.printStackTrace();
            LogUtils.logInfo(TAG, "[lyc] doInBackground", "socket process exception");
        } finally {
            try {
                if (mServerSocket != null) {
                    mServerSocket.close();
                }
                if (mClient != null) {
                    mClient.close();
                }
            } catch (IOException e) {
                e.printStackTrace();
            }
        }


        return null;
    }
    public void colseServerSocket(){
        Log.d(TAG, "[lyc] onConnectionInfoAvailable:  colse socket server");
        if (mServerSocket != null) {
            try {
                mServerSocket.close();
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
        if (mClient != null) {
            try {
                mClient.close();
            } catch (IOException e) {
                e.printStackTrace();
            }
        }

}
    private boolean isInt(float f, int i) {
        if (f - (float) i == 0) {
            //说明是整数
            return true;
        }
        return false;
    }

}