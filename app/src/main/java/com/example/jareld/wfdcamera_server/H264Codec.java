package com.example.jareld.wfdcamera_server;

/*
 *  @项目名：  WfdCamera_server 
 *  @包名：    com.example.jareld.wfdcamera_server
 *  @文件名:   H264Codec
 *  @创建者:   LYC2
 *  @创建时间:  2017/3/28 19:45
 *  @描述：    TODO
 */
public class H264Codec {
    public static native int  InitCodec(byte bInOneFrameOnce);
    public static native void UninitCodec();
    public static native int  H264Decode(byte[] out_bmp565, byte[] pRawData, int nRawDataSize, int []out_4para);

    static {
        try{ System.loadLibrary("H264Codec"); }
        catch(UnsatisfiedLinkError ule){ System.out.println("loadLibrary(H264Codec),"+ule.getMessage()); }
    }
}
