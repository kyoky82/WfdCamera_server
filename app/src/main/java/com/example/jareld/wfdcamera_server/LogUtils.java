package com.example.jareld.wfdcamera_server;

import android.util.Log;

/*
 *  @项目名：  TestWifiDerect 
 *  @包名：    com.example.lyc2.testwifiderect.utils
 *  @文件名:   LogUtils
 *  @创建者:   LYC2
 *  @创建时间:  2016/11/24 14:38
 *  @描述：    TODO
 */
public class LogUtils  {
    private static final String TAG = "HY测试:";
public  static  void logInfo(String activity, String method, String info){
    if(true){
        Log.d(TAG+activity, "Method:"+method+"Info:"+info);
    }

}
    public  static void logException(String activity, String ex){
        if(true){
            Log.d(TAG+activity, "logException: "+ex);


        }
    }
    public  static void logSpecialInfo(String activity, String ex){
        if(true){
            Log.d(TAG+activity, "logSpecialInfo: "+ex);
        }
    }


}
