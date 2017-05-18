package com.example.jareld.wfdcamera_server;

import android.util.Log;

/*
 *  @项目名：  SampleFFpeg 
 *  @包名：    com.example.lyc.sampleffpeg
 *  @文件名:   VideoPlayer
 *  @创建者:   LYC2
 *  @创建时间:  2017/1/13 11:23
 *  @描述：    TODO
 */
public class VideoPlayer {

    private static final String TAG ="HYVideoPlayer" ;

    static {
        Log.d(TAG, "[lyc] static initializer: load videoplay lib");
        System.loadLibrary("videoplay");

    }

    public static native int play(Object surface);

    public static native void jni_ffmpeg_push(String intput , String output);
    public static native void jni_ffmpeg_hecv_set_configuration(String intput , String output);
    public static native void jni_ffmpeg_hecv_set_buffer(int[] buffer);


    public static native int jni_ffmpeg_setVersion();
}
