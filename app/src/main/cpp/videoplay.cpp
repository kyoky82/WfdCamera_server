//
// Created by LYC2 on 2017/1/13.
//


#include <jni.h>
#include <android/log.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include <stdio.h>
#include <sys/time.h>

extern "C" {

//这里为什么要包括在里面  是因为ffmpeg是纯C写的   而我的编译器使用clang（Ｃ＋＋）所以在这里要进行
// 进行一个包括，意思就是按C来编译
#include "include/libavcodec/avcodec.h"
#include "include/libavformat/avformat.h"
#include "include/libswscale/swscale.h"
#include "include/libavutil/imgutils.h"
#include "include/libavutil/time.h"

#include "include/libavutil/opt.h"
//这个“”  的根目录是在 这个cpp文件的根目录

#define  LOG_TAG    "videoplay"
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
uint64_t getCurrentTime() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}
JNIEXPORT jint JNICALL
Java_com_example_jareld_wfdcamera_1server_VideoPlayer_play(JNIEnv *env, jclass type,
                                                           jobject surface) {
//    int64_t thirty_start_time = 0;
//    uint64_t jareld_test_start =getCurrentTime();
//    // surface 就是 从java层穿过来surface对象
//
//    // sd卡中的视频文件地址,可自行修改或者通过jni传入
//    char *file_name = "/storage/emulated/0/test1.h264";
//
//    //注册所有的编解码器，复用/解复用器等等组件。其中调用了avcodec_register_all()注册所有编解码器相关的组件。
//    av_register_all();
//
//    thirty_start_time = av_gettime();
//    LOGD("开始的时间 = %lld ,thirty_start_time = %lld" , jareld_test_start ,thirty_start_time);
//
//    AVFormatContext *pFormatCtx = avformat_alloc_context();
//    //创建AVFormatContext结构体。 AVFormatContext主要存储视音频封装格式中包含的信息
//    /*AVFormatContext结构体 几个重要的参数
//     *                   *AVIOContext *pb：输入数据的缓存
//                         *unsigned int nb_streams：视音频流的个数
//                         *AVStream **streams：视音频流
//                         *char filename[1024]：文件名
//                         *int64_t duration：时长（单位：微秒us，转换为秒需要除以1000000）
//                         *int bit_rate：比特率（单位bps，转换为kbps需要除以1000）
//                         *AVDictionary *metadata：元数据
//
//     */
//
//
//
//    if (avformat_open_input(&pFormatCtx, file_name, NULL, NULL) != 0) {
//        /*avformat_open_input含义：打开媒体的函数中完成下面几个功能
//               输入输出结构体AVIOContext的初始化：有关解协议（http,rtsp,rtmp,mms）的结构体
//                                                   AVIOContext，URLProtocol，URLContext主要存储视音频使用的协议的类型以及状态。URLProtocol存储输入视音频使用的封装格式。每种协议都对应一个URLProtocol结构。（注意：FFMPEG中文件也被当做一种协议“file”）
//               输入数据的协议（例如RTMP，或者file）的识别（通过一套评分机制）:1判断文件名的后缀 2读取文件头的数据进行比对；
//               使用获得最高分的文件协议对应的URLProtocol，通过函数指针的方式，与FFMPEG连接（非专业用词）；
//               剩下的就是调用该URLProtocol的函数进行open,read等操作了
//     */
//        //AVIOContext结构体 是FFMPEG管理输入输出数据的结构体
//        //                  unsigned char *buffer：缓存开始位置
//        //                  int buffer_size：缓存大小（默认32768） 在解码的情况下，buffer用于存储ffmpeg读入的数据。例如打开一个视频文件的时候，先把数据从硬盘读入buffer，然后在送给解码器用于解码。
//        //                  unsigned char *buf_ptr：当前指针读取到的位置
//        //                  unsigned char *buf_end：缓存结束的位置
//        //                  void *opaque：URLContext结构体
//        int err_code = avformat_open_input(&pFormatCtx, file_name, NULL, NULL);
//        char buf[1024];
//        av_strerror(err_code, buf, 1024);
//        LOGD("Couldn't open file %s: %d(%s)", file_name, err_code, buf);
//        LOGD("Couldn't open file:%s\n", file_name);
//        return -1; // Couldn't open file
//    }
//
//
//    if (avformat_find_stream_info(pFormatCtx, NULL) < 0) {
//        // Retrieve（取得） stream information
//        //该函数可以读取一部分视音频数据并且获得一些相关的信息。
//        LOGD("Couldn't find stream information.");
//        return -1;
//    }
//
//    // Find the first video stream
//    int videoStream = -1, i;
//
//    for (i = 0; i < pFormatCtx->nb_streams; i++) {
//        //AVFormatContext：unsigned int nb_streams：视音频流的个数
//        //AVFormatContext：*AVStream **streams：视音频流
//        //其中AVStream是存储每一个视频/音频流信息的结构体。
//        /*      AVStream
//         *       int index：标识该视频/音频流
//         *       AVCodecContext *codec：指向该视频/音频流的AVCodecContext（它们是一一对应的关系）
//         *       AVRational time_base：时基。通过该值可以把PTS，DTS转化为真正的时间。FFMPEG其他结构体中也有这个字段，但是根据我的经验，只有AVStream中的time_base是可用的。PTS*time_base=真正的时间
//         *       int64_t duration：该视频/音频流长度
//         *       AVDictionary *metadata：元数据信息
//         *       AVRational avg_frame_rate：帧率（注：对视频来说，这个挺重要的）
//         *       AVPacket attached_pic：附带的图片。比如说一些MP3，AAC音频文件附带的专辑封面。
//         */
//        if (pFormatCtx->streams[i]->codec->codec_type ==
//            AVMEDIA_TYPE_VIDEO  //一般为编解码器的类型为这个的时候说明是第一个videostream
//            && videoStream < 0) {
//            //streams[i]->codec == AVStream->codec == AVStream->AVCodecContext
//            //AVCodecContext结构体
//            /*       其中AVCodecContext是包含变量较多的结构体（感觉差不多是变量最多的结构体）
//             *       AVCodecContext中很多的参数是编码的时候使用的，而不是解码的时候使用的。
//             *
//             *       enum AVMediaType codec_type：编解码器的类型（视频，音频...）
//             *       struct AVCodec  *codec：采用的解码器AVCodec（H.264,MPEG2...）
//             *       int bit_rate：平均比特率
//             *       uint8_t *extradata; int extradata_size：针对特定编码器包含的附加信息（例如对于H.264解码器来说，存储SPS，PPS等）
//             *       AVRational time_base：根据该参数，可以把PTS转化为实际的时间（单位为秒s）
//             *       int width, height：如果是视频的话，代表宽和高
//             *       int refs：运动估计参考帧的个数（H.264的话会有多帧，MPEG2这类的一般就没有了）
//             *       int sample_rate：采样率（音频）
//             *       int channels：声道数（音频）
//             *       enum AVSampleFormat sample_fmt：采样格式
//             *       int profile：型（H.264里面就有，其他编码标准应该也有）
//             *       int level：级（和profile差不太多）
//             */
//            videoStream = i;
//        }
//    }
//    if (videoStream == -1) {
//        //说明在这倒流里面 是没有视频流的
//        LOGD("Didn't find a video stream.");
//        return -1; // Didn't find a video stream
//    }
//
//
//    AVCodecContext *pCodecCtx = pFormatCtx->streams[videoStream]->codec;
//    // Get a pointer to the codec context for the video stream
//    //获取第一个stream *AVStream **streams：视音频流的编解码器的上下文
//
//
//    // Find the decoder for the video stream
//    //根据这个codec_id来获取编解码结构体的对象  这个codec_id是个枚举类型  有很多
////    enum AVCodecID {
////        AV_CODEC_ID_NONE,
////
////        /* video codecs */
////             AV_CODEC_ID_MPEG1VIDEO,
////             AV_CODEC_ID_MPEG2VIDEO, ///< preferred ID for MPEG-1/2 video decoding
////             #if FF_API_XVMC
////             AV_CODEC_ID_MPEG2VIDEO_XVMC,
////             #endif /* FF_API_XVMC */
////             AV_CODEC_ID_H261,
////             AV_CODEC_ID_H263,
////             AV_CODEC_ID_RV10,
////             AV_CODEC_ID_RV20,
////             AV_CODEC_ID_MJPEG,
////             AV_CODEC_ID_MJPEGB,
////             AV_CODEC_ID_LJPEG,
////             AV_CODEC_ID_SP5X,
////             AV_CODEC_ID_JPEGLS。。。。。。。。。。。。。。
//
//
//
//    AVCodec *pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
//    //      AVCodec结构体：
////     其中AVCodec是存储编解码器信息的结构体。
//    /*
//         下面说一下最主要的几个变量：
//                const char *name：编解码器的名字，比较短
//                const char *long_name：编解码器的名字，全称，比较长
//                enum AVMediaType type：指明了类型，是视频，音频，还是字幕
//                enum AVCodecID id：ID，不重复
//                const AVRational *supported_framerates：支持的帧率（仅视频）
//                const enum AVPixelFormat *pix_fmts：支持的像素格式（仅视频）
//                const int *supported_samplerates：支持的采样率（仅音频）
//                const enum AVSampleFormat *sample_fmts：支持的采样格式（仅音频）
//                const uint64_t *channel_layouts：支持的声道数（仅音频）
//                int priv_data_size：私有数据的大小
//
//     */
//
//    if (pCodec == NULL) {
//        LOGD("Codec not found.");
//        return -1; // Codec not found
//    }
//
//    if ((pCodecCtx, pCodec, NULL) < 0) {
//        LOGD("Could not open codec.");
//        return -1; // Could not open codec
//    }
//
//    // 获取native window 本地窗口
//    ANativeWindow *nativeWindow = ANativeWindow_fromSurface(env, surface);
//
//    // 获取视频宽高
//    int videoWidth = pCodecCtx->width;
//    int videoHeight = pCodecCtx->height;
//
//    // 设置native window的buffer大小,可自动拉伸
//
//    LOGD("videoWidth : %d , videoHeight ：%d", videoWidth, videoHeight);
//    //1920 1080
//    // 将格式  应用到这里面
//    ANativeWindow_setBuffersGeometry(nativeWindow, videoWidth, videoHeight,
//                                     WINDOW_FORMAT_RGBA_8888);
//
//    ANativeWindow_Buffer windowBuffer;
//
//    if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0) {
//        //avcodec_open2含义
//        //该函数用于初始化一个视音频编解码器的AVCodecContext
//        //avctx：需要初始化的AVCodecContext。
//        //codec：输入的AVCodec
//        //options：一些选项。例如使用libx264编码的时候，“preset”，“tune”等都可以通过该参数设置。
//        /*
//            （1）为各种结构体分配内存（通过各种av_malloc()实现）。
//            （2）将输入的AVDictionary形式的选项设置到AVCodecContext。
//            （3）其他一些零零碎碎的检查，比如说检查编解码器是否处于“实验”阶段。
//            （4）如果是编码器，检查输入参数是否符合编码器的要求
//            （5）调用AVCodec的init()初始化具体的解码器。
//            */
//        LOGD("Could not open codec.");
//        return -1; // Could not open codec
//    }
//
//
//    AVFrame *pFrame = av_frame_alloc();
//    // av_frame_alloc含义
//    //  av_frame_alloc(void)函数来分配一个AVFrame结构体。这个函数只是分配AVFrame结构体，
//    // 但data指向的内存并没有分配，需要我们指定。这个内存的大小就是一张特定格式图像所需的大小，
//    // 等于是申请一片内存。
//    /*
//     * AVFrame结构体：
//     * d) 存数据
//            视频的话，每个结构一般是存一帧；音频可能有好几帧
//            解码前数据：AVPacket
//            解码后数据：AVFrame
//        主要的参数：
//
//     *  uint8_t *data[AV_NUM_DATA_POINTERS]：解码后原始数据（对视频来说是YUV，RGB，对音频来说是PCM）
//        int linesize[AV_NUM_DATA_POINTERS]：data中“一行”数据的大小。注意：未必等于图像的宽，一般大于图像的宽。
//        int width, height：视频帧宽和高（1920x1080,1280x720...）
//        int nb_samples：音频的一个AVFrame中可能包含多个音频帧，在此标记包含了几个
//        int format：解码后原始数据类型（YUV420，YUV422，RGB24...）
//        int key_frame：是否是关键帧
//        enum AVPictureType pict_type：帧类型（I,B,P...）
//        AVRational sample_aspect_ratio：宽高比（16:9，4:3...）
//        int64_t pts：显示时间戳
//        int coded_picture_number：编码帧序号
//        int display_picture_number：显示帧序号
//        int8_t *qscale_table：QP表
//        uint8_t *mbskip_table：跳过宏块表
//        int16_t (*motion_val[2])[2]：运动矢量表
//        uint32_t *mb_type：宏块类型表
//        short *dct_coeff：DCT系数，这个没有提取过
//        int8_t *ref_index[2]：运动估计参考帧列表（貌似H.264这种比较新的标准才会涉及到多参考帧）
//        int interlaced_frame：是否是隔行扫描
//        uint8_t motion_subsample_log2：一个宏块中的运动矢量采样个数，取log的
//     *
//     * */
//    // 用于渲染
//    AVFrame *pFrameRGBA = av_frame_alloc();
//    if (pFrameRGBA == NULL || pFrame == NULL) {
//        LOGD("Could not allocate video frame.");
//        return -1;
//    }
//
//    // Determine required buffer size and allocate buffer
//    // 上面的操作是确定所需的缓冲区大小和分配缓冲区
//
//
//    // buffer中数据就是用于渲染的,且格式为RGBA
//
//    //  Return the size in bytes of the amount of data required to store an
//    // image with the given parameters.
//
//    //返回 存储具有给定参数的图像 所需的数据量的 字节大小。
//    int numBytes = av_image_get_buffer_size(AV_PIX_FMT_RGBA, pCodecCtx->width, pCodecCtx->height,
//                                            1);
//
//    LOGD("numBytes = %d", numBytes);
//
//    uint8_t *buffer = (uint8_t *) av_malloc(numBytes * sizeof(uint8_t));
//    //av_malloc()就是简单的封装了系统函数malloc() 申请内存
//    //typedef	unsigned char    == uint8_t  无符号的 一个byte  0-255
//    //该函数并没有为AVFrame的像素数据分配空间。
//    // 因此AVFrame中的像素数据的空间需要自行分配空间，例如使用avpicture_fill()，av_image_fill_arrays()等函数。
//
//
//    av_image_fill_arrays(pFrameRGBA->data, pFrameRGBA->linesize, buffer, AV_PIX_FMT_RGBA,
//                         pCodecCtx->width, pCodecCtx->height, 1);
//    //av_image_fill_arrays含义
//    //自我理解 指针 pFrameRGBA的数据大小 --buffer ,图像格式 --AV_PIX_FMT_RGBA，宽高-pCodecCtx->width, pCodecCtx->height
//    //基于指定的图像参数和提供的数组设置数据指针和行数。
//    //使用指向图像数据缓冲区的src地址填充给定图像的字段。 根据指定的像素格式，
//    // 将设置一个或多个图像数据指针和行大小。 如果指定了平面格式，
//    // 则将设置指向不同图像平面的若干指针，并且不同平面的线大小将存储在lines_sizes数组中。
//    // 使用src == NULL调用以获取src缓冲区所需的大小。
//    //前面两个参数 属于 dst参数
//    // 由于解码出来的帧格式不是RGBA的,在渲染之前需要进行格式转换
//
//    struct SwsContext *sws_ctx = sws_getContext(pCodecCtx->width,    //srcW：源图像的宽
//                                                pCodecCtx->height,   //srcH：源图像的高
//                                                pCodecCtx->pix_fmt,  //srcFormat：源图像的像素格式
//                                                pCodecCtx->width,    //dstW：目标图像的宽
//                                                pCodecCtx->height,   //dstH：目标图像的高
//                                                AV_PIX_FMT_RGBA,     //dstFormat：目标图像的像素格式
//                                                SWS_BILINEAR,        //flags：设定图像拉伸使用的算法
//                                                NULL,
//                                                NULL,
//                                                NULL);
//    /*sws_getContext含义：
//     * 分配并返回一个SwsContext。 您需要使用sws_scale（）执行缩放/转换操作。
//     * 该函数包含以下参数：
//                srcW：源图像的宽
//                srcH：源图像的高
//                srcFormat：源图像的像素格式
//                dstW：目标图像的宽
//                dstH：目标图像的高
//                dstFormat：目标图像的像素格式
//                flags：设定图像拉伸使用的算法
//                成功执行的话返回生成的SwsContext，*/
//
//
//    int frameFinished;
//    AVPacket packet;
//    //其中AVPacket是存储压缩编码数据相关信息的结构体
//    /*
//AVPacket结构体：
//    在AVPacket结构体中，重要的变量有以下几个：
//    uint8_t *data：压缩编码的数据。
//    例如对于H.264来说。1个AVPacket的data通常对应一个NAL。
//    注意：在这里只是对应，而不是一模一样。他们之间有微小的差别：使用FFMPEG类库分离出多媒体文件中的H.264码流
//    因此在使用FFMPEG进行视音频处理的时候，常常可以将得到的AVPacket的data数据直接写成文件，从而得到视音频的码流文件。
//    int   size：data的大小
//    int64_t pts：显示时间戳
//    int64_t dts：解码时间戳
//    int   stream_index：标识该AVPacket所属的视频/音频流。
//    这个结构体虽然比较简单，但是非常的常用。
//
//    */
//    int num = 0;
//    int whileNuM = 0;
//    int frame_index = 0;
//    long start_time;
//    int videostreamNum = 0;
//    long end_time;
//    int pFrameNum = 0;
//    long iFrameStartTime = 0;
//    long iFrame30ndTime = 0;
//    long frameSecond = 0;
//    int xiuzhengZhi = 0;
//    int tenOfTheNumOne = 0;
//    int shijiChaju = 0;
//    int chaju[15];
//    int chajuNum = 0;
//    long thirty_end_time = 0;
//    int thirtyThreeNum = 0;
//    int isThreeNumHandle = 0;
//    int isWhile = 1;
//    // while (1){
//    //avformat_open_input(&pFormatCtx, file_name, NULL, NULL) 这个pFormatCtx跟数据的联系是从这个open_input里面来连接的
//    //
//    int MAX_TIME = 100;
//    int read_time = 0;
//while (read_time <= MAX_TIME) {
//    while (av_read_frame(pFormatCtx, &packet) >= 0) {
////        if(thirty_start_time == 0){
////            thirty_start_time = av_gettime();
////        }
//        //av_read_frame含义：
//        //ffmpeg中的av_read_frame()的作用是读取码流中的音频若干帧或者视频一帧。例如，解码视频的时候，每解码一个视频帧，
//        //需要先调用 av_read_frame()获得一帧视频的压缩数据，然后才能对该数据进行解码（例如H.264中一帧压缩数据通常对应一个NAL）。
//        //通过av_read_packet(***)，读取一个包，需要说明的是此函数必须是包含整数帧的，不存在半帧的情况，
//        // 以ts流为例，是读取一个完整的PES包（一个完整pes包包含若干视频或音频es包），
//        // 读取完毕后，通过av_parser_parse2(***)分析出视频一帧（或音频若干帧），
//        // 返回，下次进入循环的时候，如果上次的数据没有完全取完，则st = s->cur_st;不会是NULL，
//        // 即再此进入av_parser_parse2(***)流程，而不是下面的av_read_packet（**）流程，
//        // 这样就保证了，如果读取一次包含了N帧视频数据（以视频为例），则调用av_read_frame（***）N次都不会去读数据，
//        // 而是返回第一次读取的数据，直到全部解析完毕。
//
//        start_time = av_gettime();
//
//
//        if (packet.pts == AV_NOPTS_VALUE) {
//            LOGD("说明有pts %ld", packet.pts);
//        } else {
//            LOGD("说明没有pts %ld", packet.pts);
//        }
//
//
//        whileNuM++;
//        LOGD("while的次数 %d", whileNuM);
//        if (*(packet.data + 1555) == 15 && *(packet.data + 1666) == 16 &&
//            *(packet.data + 1777) == 17) {
//            LOGD("shibushi frameFinished = %d", frameFinished);
//            LOGD("Jareld ：第四个 = 我写的数据 = %d", *(packet.data + 1888));
//            int a = (int) *(packet.data + 1888);
//            LOGD("Jareld ：第四个 = 我写的数据 = %d a赋值后 = %d", *(packet.data + 1888), a);
//            LOGD("Jareld ：第四个 = 我写的数据 = %d a赋值后 = %d 为什么不出来");
//            if (a >= 2) {
//                LOGD("Jareld ：第四个 = 我写的数据 = %d", *(packet.data + 1888));
//                for (chajuNum = 0; chajuNum < 10; chajuNum++) {
//                    chaju[chajuNum] = (int) *(packet.data + 2000 + chajuNum) * 1000;
//                }
//                chaju[10] = 11;
//                //重置
//                tenOfTheNumOne = 0;
//
//            } else {
//                for (chajuNum = 0; chajuNum < 10; chajuNum++) {
//                    LOGD("Jareld ：进入到*(chaju + 10) = 1for循环");
//                    chaju[chajuNum] = (int) *(packet.data + 2000 + chajuNum) * 1000;
//                }
//                chaju[10] = 1;
//            }
//            int b = (int) *(packet.data + 1111);
//
//            if (b == 22) {
//                chaju[11] = 22;
//                chaju[12] = (int) *(packet.data + 1222);
//                chaju[13] = (int) *(packet.data + 1333);
//                chaju[14] = (int) *(packet.data + 1344);
//                if (isThreeNumHandle == 0) {
//                    //说明要转变
//                    thirty_end_time = av_gettime();
//                    long toNowTimeJieMa = thirty_end_time - thirty_start_time;
//
////
//                    LOGD("目前解碼耗费的时间 = %ld av_gettime() = %ld", toNowTimeJieMa, av_gettime());
//
//                    long toNowTimeBianma =
//                            (chaju[14] * 10000 + chaju[13] * 100 + chaju[12]) * 1000;
//
//                    LOGD("目前编码耗费的时间 = %ld", toNowTimeBianma);
//                    LOGD("目前Jareld_Test耗费的时间(getCurrentTime()) = %lld ，jareld_test_start = %l64d ", getCurrentTime() - jareld_test_start ,jareld_test_start);
//
//
//                    long shijianChaju = toNowTimeBianma - toNowTimeJieMa;
//                    LOGD("目前编解码消耗的差距时间 = %ld", shijianChaju);
//
//                    if (shijianChaju > 0) {
//                        //   编码的时间走向  要  大于   解码的  时间走向     解码  要等待   所以这里要等待
//                        av_usleep(shijianChaju);
//                    } else {
//                        //编码的时间走向  要 小于  解码的   时间走向     解码 需要加快  让下面的休息  少休息
//                        xiuzhengZhi = xiuzhengZhi + shijianChaju;
//                    }
//                    isThreeNumHandle = 1;
//
//
//                }
//
//                LOGD("这里为22的赋值");
//            } else if (b == 33) {
//                LOGD("这里为33的赋值");
//                thirtyThreeNum++;
//                chaju[11] = 33;
//                if (isThreeNumHandle == 1) {
//                    isThreeNumHandle = 0;
//                }
//            };
//            continue;
//        }
//
//        if (packet.stream_index == videoStream) {
//            videostreamNum++;
//            LOGD("videostreamNum的次数%d", videostreamNum);
//
//            // Decode video frame
//            avcodec_decode_video2(pCodecCtx, pFrame, &frameFinished, &packet);
//            //avcodec_decode_video2含义
//            //ffmpeg中的avcodec_decode_video2()的作用是解码一帧视频数据。
//            // 输入一个压缩编码的结构体AVPacket，输出一个解码后的结构体AVFrame
//            // 第三个参数 int got_picture_ptr ：  是这样解释的：如果没有帧可以解压，则为零，否则为非零。
//            // 并不是decode一次就可解码出一帧
//            if (frameFinished) {
//
//                if (pFrame->pts == AV_NOPTS_VALUE) {
//                    LOGD("说明有pts %ld", packet.pts);
//                } else {
//                    LOGD("说明没有pts %ld", packet.pts);
//                }
////                if(pFrame->pts == AV_NOPTS_VALUE){
////                    int64_t a1 =   packet.pts;
////
////                    LOGD("没有时间戳");
////                    int c = pFrame->best_effort_timestamp;
////                    int64_t  a = pFrame->pkt_dts;
////                    int64_t  b = pFrame->pts;
////                    int64_t  d = packet.dts;
////                    int64_t  f= packet.pos;
////                    LOGD("pkt_dts = %d , pts = %d" , a , b);
////                    LOGD("pkt_dts = %ld , pts = %ld ，packet.pts = %ld , pFrame->best_effort_timestamp = %ld ， packet.dts = %ld ,packet.pos = %ld" , a , b , a1 , c , d , f);
////
////                }else{
////                    LOGD("有时间戳");
////                    int c = pFrame->best_effort_timestamp;
////                    int64_t a1 =   packet.pts;
////
////                    int64_t  a = pFrame->pkt_dts;
////                    int64_t  b = pFrame->pts;
////                    LOGD("pkt_dts = %ld , pts = %ld ，packet.pts = %ld , pFrame->best_effort_timestamp = %ld" , a , b , a1 , c);
////
////                }
////                pFrameNum ++ ;
////
////                if(pFrame->key_frame == 1){
////                    LOGD("说明是关键帧 ： %d" , pFrameNum );
////                    if(iFrameStartTime == 0){
////                        //说明第一次进来
////                        iFrameStartTime = getCurrentTime();
////                    }else{
////                        //不是第一次进来
////                        iFrame30ndTime = getCurrentTime();
////
////                    }
////                }
////
////                if (packet.stream_index == videoStream) {
////                    //ffmpeg处理速度很快 需要延迟，减少流媒体的负担
////                    AVRational time_base = pFormatCtx->streams[videoStream]->time_base;
////                    AVRational time_base_q = {1, AV_TIME_BASE};
////                    int64_t pts_time = av_rescale_q(packet.dts, time_base, time_base_q);
////                    int64_t now_time = av_gettime() - start_time;
////                    LOGD("pts_time = %d , now_time = %d ， pFormatCtx->streams[videoStream]->time_base.num = " , pts_time , now_time , pFormatCtx->streams[videoStream]->time_base.num);
////
////                }
//                num++;
//                LOGD("finished的次数%d", num);
//
//                //   LOGD("while  if (frameFinished)");
//                // lock native window buffer
//                //第一个参数 ：ANativeWindow *nativeWindow--有过的操作ANativeWindow *nativeWindow = ANativeWindow_fromSurface(env, surface);
//                //                                                  ANativeWindow_setBuffersGeometry(nativeWindow, videoWidth, videoHeight,WINDOW_FORMAT_RGBA_8888);
//                //第二个参数 ：ANativeWindow_Buffer windowBuffer--没有过操作
//                //第三个参数 ：inOutDirtyBounds ：Lock the window's next drawing surface for writing. 锁定窗口的下一个绘图表面以进行书写。
//                // inOutDirtyBounds is used as an in/out parameter, inOutDirtyBounds用作输入/输出参数，
//                // upon entering the function, it contains the dirty region, that is,在输入函数时，它包含脏区域，
//                // the region the caller intends to redraw. When the function returns, 即调用程序打算重绘的区域。
//                // inOutDirtyBounds is updated with the actual area the caller needs to redraw -- 当函数返回时，inOutDirtyBounds被更新为调用者需要重绘的实际区域 -
//                // this region is often extended by ANativeWindow_lock. 这个区域通常被ANativeWindow_lock扩展。
//
//                //这里可以认为就是锁定描绘的窗口
//                ANativeWindow_lock(nativeWindow, &windowBuffer, 0);
//
//                // 格式转换
//                //它封装了SwsContext中的swscale()（注意这个函数中间没有“_”）
//                //sws_scale()：处理图像数据。 是libswsscale中的sws_scale()方法 ---是用于转换像素的函数
//                sws_scale(sws_ctx, (uint8_t const *const *) pFrame->data,
//                          pFrame->linesize, 0, pCodecCtx->height,
//                          pFrameRGBA->data, pFrameRGBA->linesize);
//
//                //sws_scale含义：个人理解是把yuv格式转换成rgba格式 这里rgba格式由哪里决定
//                //在创建sws_ctx对象的时候有一个参数就是：AV_PIX_FMT_RGBA 像素格式
//                /**
// * Scale the image slice in srcSlice and put the resulting scaled
// * slice in the image in dst. A slice is a sequence of consecutive
// * rows in an image.
// *在src Slice中缩放图像切片，并将生成的缩放切片放在dst中的图像中。 切片是图像中的连续行的序列。
// * Slices have to be provided in sequential order, either in
// * top-bottom or bottom-top order. If slices are provided in
// * non-sequential order the behavior of the function is undefined.
// *必须以顺序的顺序提供切片，以上下或者从下到上的顺序。 如果以非顺序次序提供切片，则函数的行为是未定义的。
// * @param c         the scaling context previously created with
// *                  sws_getContext()
//                    第一个参数是sws的一个对象
// * @param srcSlice  the array containing the pointers to the planes of
// *                  the source slice
//                    原始图像的数据    AVPrame的data：解码后原始数据（对视频来说是YUV，RGB，对音频来说是PCM）
// * @param srcStride the array containing the strides for each plane of
// *                  the source image
//                    data中“一行”数据的大小。注意：未必等于图像的宽，一般大于图像的宽。该数组包含源图像的每个平面的步幅
// * @param srcSliceY the position in the source image of the slice to
// *                  process, that is the number (counted starting from
// *                  zero) in the image of the first row of the slice
//                    要处理的片段的源图像中的位置，即在片段的第一行的图像中的数量（从零开始计数）
// * @param srcSliceH the height of the source slice, that is the number
// *                  of rows in the slice
//                    如果是视频的话，代表宽和高 源切片的高度，即切片中的行数
// * @param dst       the array containing the pointers to the planes of
// *                  the destination image
//                    该数组包含指向目标图像的平面的指针   目标AVPrame的data  解码后原始数据（
// * @param dstStride the array containing the strides for each plane of
// *                  the destination image
//                    该数组包含目的图像的每个平面的步幅
//
//    //注意 这个pFrameRGBA 即 dstPrame做了一个处理
//                         av_image_fill_arrays(pFrameRGBA->data, pFrameRGBA->linesize, buffer, AV_PIX_FMT_RGBA,
//                         pCodecCtx->width, pCodecCtx->height, 1);
//                         这个处理我认为是类似copy的处理
// * @return             the height of the output slice
// */
//                //接下来的这个stride 是为了 满足 解码图像的步幅匹配窗口缓冲区的步幅
//                // 出现花屏的问题：图像伪影的特征表明解码图像的步幅不匹配窗口缓冲区的步幅。 这个地方最可能出现的问题：
//                // 获取stride
//                uint8_t *dst = (uint8_t *) windowBuffer.bits;//获取到屏幕的实际位数 。-654217216
//                LOGD("屏幕的实际位数：windowBuffer.bits dst = %d", dst);
//                // windowBuffer.stride :The number of pixels that a line in the buffer takes in memory.  This may be >= width.
//                //缓冲区中一行占用内存的像素数  一般是要大于等于 width
//                // 一个像素等于是 4个byte  32bit 。
//                int dstStride = windowBuffer.stride * 4;
//                LOGD("目的Strinde 即屏幕windowBuffer.stride * 4：dstStride = %d", dstStride);// 7680
//                //解码后的视频数据  yuv　或者ｒｇｂ
//                uint8_t *src = (pFrameRGBA->data[0]);
//                LOGD("解码后的数据 pFrameRGBA->data[0]：src = %d", src);// 7680
//
//                /*
//                 *
//
//565/5000
//*对于视频，每个图像行的字节大小。
//对于音频，每个平面的字节大小。
//
//  对于音频，只能设置linesize [0]。 对于平面音频，每个通道平面必须具有相同的大小。
//
//  对于视频，线条应该是CPU对齐偏好的倍数，现代桌面CPU为16或32。
//  一些代码需要这样的对齐其他代码可能会更慢，没有正确的对齐，对于其他它没有什么区别。
//
//  @note linesize可能大于可用数据的大小 - 出于性能原因可能存在额外的填充。*/
//                int srcStride = pFrameRGBA->linesize[0];
//                LOGD("srcStride pFrameRGBA->linesize[0]：srcStride = %d", srcStride);// 7680
//
//                // 由于window的stride和帧的stride不同,因此需要逐行复制
//                int h;
//                for (h = 0; h < videoHeight; h++) {
//                    //循环的次数是videoHeight  视频帧画面的高度
//                    // dst ， source ，length
//                    memcpy(dst + h * dstStride, src + h * srcStride, srcStride);
//                    //这里发现这个 等于是把数据一行行铺到屏幕上去
//
//                    //memcpy(dst + h * dstStride, src + h * srcStride, srcStride-1000);
//                    //如果-1000 去操作 发现横屏的时候屏幕右边1000距离的数据是黑的 没有的
//                    //uint8_t *dst = (uint8_t *) windowBuffer.bits相当于屏幕的所有像素点的一个集合的起点的
//                    //一行行去赋值数据
//
//                    //LOGD("while  if (frameFinished)  for (h = 0; h < videoHeight; h++)");
//                }
//                //裸流中没有这两个数据
//                //PTS :解码后视频帧要在什么时候显示出来
//                //DTS ：在送入解码器开始解码的时候标识什么时候开始解码
//                //这里其实可以注释掉，因为我没有用裸流 所以pts和dts是有的
//
//                LOGD("解码的次数 %d", num);
//
//
//
//
//                //這個函數等於是要解鎖 和 post上去 進行渲染  具體實現不管
//                ANativeWindow_unlockAndPost(nativeWindow);
//
//            }
//
//        }
//        if (packet.stream_index == videoStream) {
//            LOGD("第%d帧", frame_index);
//            frame_index++;
//        }
//
//        av_packet_unref(&packet);
//
//
//        if (chaju[10] == 11) {
////            if (chaju[11] == 22) {
////
////                LOGD("进入到11 == 22 = %d", thirtyThreeNum);
////                //那就是是要修正了
////
////                if (thirtyThreeNum  %3 == 0) {
////                    //只处理一次
////                    if (isThreeNumHandle == 0) {
////                        thirty_end_time = av_gettime();
////                        long thirty_chaju_jiema = thirty_end_time - thirty_start_time;
////
////                        int thirty_chaju_bianma = (chaju[12] * 100 + chaju[13] + chaju[14] * 10000)  ;
//////                        int thirty_chaju = thirty_chaju_bianma - thirty_chaju_jiema;
////                        //要处理
////                        LOGD("进入30fps的xiuzheng bianma = %d", thirty_chaju_bianma);
////
////                        LOGD("进入30fps的xiuzheng jiema = %d", thirty_chaju_jiema/1000);
////
////                        LOGD("进入30fps的xiuzheng bianma - jiema  差距 = %d", thirty_chaju_bianma -thirty_chaju_jiema/1000);
////
////                        //再重置
////                        isThreeNumHandle = 1;
////
//////                        thirty_start_time = thirty_end_time;
//////
//////                        if(thirty_chaju >  0 ){
//////                            //编码30fps的耗时 要比 解码30fps的耗时 时间 要多，
//////                            //编码的速度 无法控制  所以要把解码的速度降低
//////                            av_usleep(thirty_chaju);
//////                        }else{
//////                            //编码30fps的耗时 要比 解码30fps的耗时 时间 要少
//////                            xiuzhengZhi = xiuzhengZhi + thirty_chaju;
//////                        }
////
////                    }
////                }
////
////            }
//
//
//            shijiChaju = chaju[tenOfTheNumOne] - (av_gettime() - start_time);
//            if (tenOfTheNumOne > 10) {
//                tenOfTheNumOne = 0;
//            }
//            LOGD("次数11111 = %d , 到unlock的时间111 = %d ,shijiChaju = %d 200000 tenOfTheNumOne = %d",
//                 num, chaju[tenOfTheNumOne], shijiChaju, tenOfTheNumOne);
//
//            //如果有的话 要重置一下参数。
//            //先进入30fps每次的修正
//            if (shijiChaju > 0) {
//                if ((shijiChaju + xiuzhengZhi) > 0) {
//                    //还要休眠
//                    LOGD("实际休眠的时间 = %d", (shijiChaju + xiuzhengZhi));
//                    av_usleep(shijiChaju + xiuzhengZhi);
//                    //重置
//                    xiuzhengZhi = 0;
//                } else {
//                    //小于0  就不修正
//                    //重置一下修正值
//                    xiuzhengZhi = xiuzhengZhi + shijiChaju;
//                }
//            } else {
//                xiuzhengZhi = xiuzhengZhi + shijiChaju;
//            }
//            LOGD("修正值的剩余 = %d", xiuzhengZhi);
//        }
//        tenOfTheNumOne++;
//
//    }
//    LOGD("休息一下");
//av_usleep(300000);
//    read_time++;
//}
//    LOGD("到文件中读取不到数据了 .重启一下");
//        //  av_usleep(50 * 1000);
//        //  LOGD("这里没有了  但是进入了while(1)");
//        //  }
//
//    LOGD("while结束");
//    av_free(buffer);
//    av_free(pFrameRGBA);
//
//    // Free the YUV frame
//    av_free(pFrame);
//
//    // Close the codecs
//    avcodec_close(pCodecCtx);
//
//    // Close the video file
//    avformat_close_input(&pFormatCtx);







    // sd卡中的视频文件地址,可自行修改或者通过jni传入

    char *file_name = "/storage/emulated/0/test1.h264";

    av_register_all();

    AVFormatContext *pFormatCtx = avformat_alloc_context();

    // Open video file
    if (avformat_open_input(&pFormatCtx, file_name, NULL, NULL) != 0) {
        int err_code = avformat_open_input(&pFormatCtx, file_name, NULL, NULL);
        char buf[1024];
        av_strerror(err_code, buf, 1024);
        LOGD("Couldn't open file %s: %d(%s)", file_name, err_code, buf);
        LOGD("Couldn't open file:%s\n", file_name);
        return -1; // Couldn't open file
    }

    // Retrieve stream information
    if (avformat_find_stream_info(pFormatCtx, NULL) < 0) {
        LOGD("Couldn't find stream information.");
        return -1;
    }

    // Find the first video stream
    int videoStream = -1, i;
    for (i = 0; i < pFormatCtx->nb_streams; i++) {
        if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
            //&& videoStream < 0) {
            videoStream = i;
            break;  // park.xu 20170516
        }
    }
    if (videoStream == -1) {
        LOGD("Didn't find a video stream.");
        return -1; // Didn't find a video stream
    }

    // Get a pointer to the codec context for the video stream
    AVCodecContext *pCodecCtx = pFormatCtx->streams[videoStream]->codec;

    // Find the decoder for the video stream
    AVCodec *pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
    if (pCodec == NULL) {
        LOGD("Codec not found.");
        return -1; // Codec not found
    }

    if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0) {
        LOGD("Could not open codec.");
        return -1; // Could not open codec
    }

    // 获取native window
    ANativeWindow *nativeWindow = ANativeWindow_fromSurface(env, surface);

    // 获取视频宽高
    int videoWidth = pCodecCtx->width;
    int videoHeight = pCodecCtx->height;

    // 设置native window的buffer大小,可自动拉伸

    LOGD("videoWidth : %d , videoHeight ：%d", videoWidth, videoHeight);
    //1920 1080
    ANativeWindow_setBuffersGeometry(nativeWindow, videoWidth, videoHeight,
                                     WINDOW_FORMAT_RGBA_8888);
    ANativeWindow_Buffer windowBuffer;

    if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0) {
        LOGD("Could not open codec.");
        return -1; // Could not open codec
    }

    // Allocate video frame
    AVFrame *pFrame = av_frame_alloc();

    // 用于渲染
    AVFrame *pFrameRGBA = av_frame_alloc();
    if (pFrameRGBA == NULL || pFrame == NULL) {
        LOGD("Could not allocate video frame.");
        return -1;
    }

    // Determine required buffer size and allocate buffer
    // buffer中数据就是用于渲染的,且格式为RGBA
    int numBytes = av_image_get_buffer_size(AV_PIX_FMT_RGBA, pCodecCtx->width, pCodecCtx->height,
                                            1);
    uint8_t *buffer = (uint8_t *) av_malloc(numBytes * sizeof(uint8_t));
    av_image_fill_arrays(pFrameRGBA->data, pFrameRGBA->linesize, buffer, AV_PIX_FMT_RGBA,
                         pCodecCtx->width, pCodecCtx->height, 1);

    // 由于解码出来的帧格式不是RGBA的,在渲染之前需要进行格式转换
    struct SwsContext *sws_ctx = sws_getContext(pCodecCtx->width,
                                                pCodecCtx->height,
                                                pCodecCtx->pix_fmt,
                                                pCodecCtx->width,
                                                pCodecCtx->height,
                                                AV_PIX_FMT_RGBA,
                                                SWS_BILINEAR,
                                                NULL,
                                                NULL,
                                                NULL);

    int ret = 0;
    int got_picture;
    AVPacket packet;
    int frameNum = 0;
    int64_t currentTime = 0;
    int64_t lastTime = 0;
    int frameTicker = 40000;   // the unit is us.  park.xu 20170516
    int sleepUs = 0;

    av_init_packet(&packet);
    while (av_read_frame(pFormatCtx, &packet) >= 0) {

        // get frameTicker
        if (packet.flags == AV_PKT_FLAG_KEY) {
            // get magic number
            uint8_t a5 = *(packet.data + 100);
            uint8_t a6 = *(packet.data + 200);
            uint8_t a7 = *(packet.data + 300);

            if (a5 == 11 && a6 == 22 && a7 == 33) {
                uint8_t a8 = *(packet.data + 8);
                uint8_t a9 = *(packet.data + 9);
                uint8_t a10 = *(packet.data + 10);

                int64_t transTicker = ((a8 << 16) + (a9 << 8) + a10) * 1000;
                frameTicker = transTicker / 10;

                LOGD("This packet will be dropped. a8 = 0x%x , a9 = 0x%x , a10 = 0x%x , transTicker = %lld, frameTicker:%d", a8, a9, a10, transTicker, frameTicker);

                continue;
            }
        }

        // just use videoStream
        if (packet.stream_index == videoStream) {

            ret = avcodec_decode_video2(pCodecCtx, pFrame, &got_picture, &packet);

            if(ret < 0){
                LOGD("avcodec_decode_video2 error!");
                continue;
            }

            if (got_picture) {
                frameNum++;

                LOGD("got picture:%d", frameNum);

                if (pFrame->flags == AV_PKT_FLAG_KEY) {
                    LOGD("The frame flags is AV_PKT_FLAG_KEY: num = %d", frameNum);
                }

                // 1. lock native window buffer
                ANativeWindow_lock(nativeWindow, &windowBuffer, 0);
                sws_scale(sws_ctx, (uint8_t const *const *) pFrame->data,
                          pFrame->linesize, 0, pCodecCtx->height,
                          pFrameRGBA->data, pFrameRGBA->linesize);
                // 获取stride
                uint8_t *dst = (uint8_t *) windowBuffer.bits;
                int dstStride = windowBuffer.stride * 4;
                uint8_t *src = (pFrameRGBA->data[0]);
                int srcStride = pFrameRGBA->linesize[0];

                LOGD("dstStride = %d ----srcStride = %d " , dstStride , srcStride);
                // 由于window的stride和帧的stride不同,因此需要逐行复制
                int h;
                for (h = 0; h < videoHeight; h++) {
                    memcpy(dst + h * dstStride, src + h * srcStride, srcStride);
                }
                ANativeWindow_unlockAndPost(nativeWindow);

                // 2. now, delay some times for play control
                currentTime = av_gettime();
                if(lastTime > 0){
                    sleepUs = (int) (frameTicker - (currentTime - lastTime));
                    LOGD("av_usleep: sleepUs:%d, frameTicker:%d, currentTime:%lld, lastTime:%lld",
                         sleepUs, frameTicker, currentTime, lastTime);

                    if (sleepUs > 5000) {
                        av_usleep(sleepUs);   // park.xu 20170516
                    }
                }else{
                    av_usleep(30000);   // first frame, delay 30 ms. park.xu 20170518
                }
                lastTime = av_gettime();

                //av_usleep(frameTicker);  // delay frameTicker every frame. park.xu 20170518
            } else {
                LOGD("avcodec_decode_video2 failed");
            }
        } else {
            LOGD("The packet stream_index is not videoStream");
        }

        av_packet_unref(&packet);
        av_init_packet(&packet);

        LOGD("av_read_frame loop end!");
    }

    av_free(buffer);
    av_free(pFrameRGBA);

    // Free the YUV frame
    av_free(pFrame);

    // Close the codecs
    avcodec_close(pCodecCtx);

    // Close the video file
    avformat_close_input(&pFormatCtx);
    return 0;
}

//这里是利用jni来推流
JNIEXPORT void JNICALL
Java_com_example_jareld_wfdcamera_1server_VideoPlayer_jni_1ffmpeg_1push(JNIEnv *env, jclass type,
                                                                        jstring intput_,
                                                                        jstring output_) {
    //视频文件所在地址
    const char *intput = env->GetStringUTFChars(intput_, 0);
    //推送的流媒体地址
    const char *output = env->GetStringUTFChars(output_, 0);
    LOGD("Jareld:jni_push");
    //封装格式 写入  和 输出    -------------解封装  得到frame
    AVFormatContext *inFmtCtx = NULL, *outFmtCtx = NULL;
    //注册组件
    av_register_all();
    //初始化网络
    avformat_network_init();

    //定义一个返回的参数  来决定是什么
    int ret;
    if (ret = avformat_open_input(&inFmtCtx, intput, 0, 0) < 0) {
        LOGD("Jareld:open input failed");
        //这里就要退出   关闭输入和输出流
//        goto end;

        avformat_free_context(inFmtCtx);
        avformat_free_context(outFmtCtx);

    }

    //获取文件信息：
    if ((ret = avformat_find_stream_info(inFmtCtx, 0)) < 0) {
        LOGD("Jareld:find stream info failed");
        //   goto  end;

        avformat_free_context(inFmtCtx);
        avformat_free_context(outFmtCtx);

    }

    //输出的封装格式   以rtmp协议来输出 我推送的是MP4格式的视频流
    avformat_alloc_output_context2(&outFmtCtx, NULL, "flv", output);

    LOGD("Jareld:avformat_alloc_output_context2");

    //根据输入的avStream，构造输出流AVStream
    int i = 0;

    //思考 在输入的时候 需要几个输出流。 因为输入流  有头信息 ，有音频 ，有视频
    //这个输入流的nb_streams 是什么呢？ 是这个MP4流里面的封装格式是 头文件，音频，视频，音频，视频
    //比如 之前学的flv的封装格式   头文件，音频，视频，音频，视频。。。。。
    //这个有多少个 这个nb_streams就有多少个
    for (i = 0; i < inFmtCtx->nb_streams; i++) {
        AVStream *input_stream = inFmtCtx->streams[i];
        //输入和输出解码器要保持一致
        AVStream *out_stream = avformat_new_stream(outFmtCtx, input_stream->codec->codec);
        //解码器的上下文的copy 把输入的流的解码器上下文copy到输出流里面去
        avcodec_copy_context(out_stream->codec, input_stream->codec);
        //全局的 header
        out_stream->codec->codec_tag = 0;
        if (outFmtCtx->oformat->flags == AVFMT_GLOBALHEADER) {
            //有一个全局的头
            out_stream->codec->flags = CODEC_FLAG_GLOBAL_HEADER;
        }
    }

    LOGD("Jareld:for loop");

    //打开输出的AVIOContext IO流上下文
    AVOutputFormat *ofmt = outFmtCtx->oformat;
    if (!(ofmt->flags & AVFMT_NOFILE)) {
        ret = avio_open(&outFmtCtx->pb, output, AVIO_FLAG_WRITE);
        LOGD("Jareld:avio_open%d", ret);
        char buf[1024];
        av_strerror(ret, buf, 1024);
        LOGD("Couldn't open file : %d(%s)", ret, buf);
    }
    LOGD("Jareld:avio_open%d", ret);
    //先写一个头
    ret = avformat_write_header(outFmtCtx, NULL);
    LOGD("Jareld:avformat_write_heade");

    if (ret < 0) {
        LOGD("Jareld:write header failed");
        char buf[1024];
        av_strerror(ret, buf, 1024);
        LOGD("Couldn't open file : %d(%s)", ret, buf);
        //  goto end;
        avformat_free_context(inFmtCtx);
        avformat_free_context(outFmtCtx);
    }

    //获取视频流的索引位置
    int videoIndex = -1;
    for (i = 0; i < inFmtCtx->nb_streams; i++) {
        if (inFmtCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoIndex = i;
            break;
        }
    }
    //找到视频帧的索引
    int frame_index = 0;
    int64_t start_time = av_gettime();
    AVPacket pkt;
    while (1) {
        AVStream *in_avstream, *out_avstream;
        //读取avpacket
        ret = av_read_frame(inFmtCtx, &pkt);
        if (ret < 0) {
            LOGD("Jareld:read frame failed");
            break;
        }
        //裸流中没有这两个数据
        //PTS : 解码后视频帧要在什么时候显示出来
        //DTS ：在送入解码器开始解码的时候标识什么时候开始解码
        //这里其实可以注释掉，因为我没有用裸流 所以pts和dts是有的
//        if(pkt.pts == AV_NOPTS_VALUE){
//            //写pkt
//            AVRational time_base1 = inFmtCtx->streams[videoIndex]->time_base;
//            int64_t  clac_duration = (double)AV_TIME_BASE/av_q2d(inFmtCtx->streams[videoIndex]->r_frame_rate);
//            pkt.pts = (double)(frame_index * clac_duration) / (double)(av_q2d(time_base1)*AV_TIME_BASE);
//            pkt.dts = pkt.pts;
//            pkt.duration = (double) clac_duration / (double)(av_q2d(time_base1)*AV_TIME_BASE);
//        }
//

        if (pkt.stream_index == videoIndex) {
            //ffmpeg处理速度很快 需要延迟，减少流媒体的负担
            AVRational time_base = inFmtCtx->streams[videoIndex]->time_base;
            AVRational time_base_q = {1, AV_TIME_BASE};
            int64_t pts_time = av_rescale_q(pkt.dts, time_base, time_base_q);
            int64_t now_time = av_gettime() - start_time;
            if ((pts_time - now_time) > 0) {
                av_usleep(pts_time - now_time);
                LOGD("pts_time %d", (pts_time - now_time));
            }
        }

        in_avstream = inFmtCtx->streams[pkt.stream_index];
        out_avstream = outFmtCtx->streams[pkt.stream_index];

        //拷贝这个包packet  如果已经有的情况下
        pkt.pts = av_rescale_q_rnd(pkt.pts, in_avstream->time_base, out_avstream->time_base,
                                   (AVRounding) (AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
        pkt.dts = av_rescale_q_rnd(pkt.dts, in_avstream->time_base, out_avstream->time_base,
                                   (AVRounding) (AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
        pkt.duration = av_rescale_q(pkt.duration, in_avstream->time_base, out_avstream->time_base);
        pkt.pos = -1;
        if (pkt.stream_index == videoIndex) {
            LOGD("frame_index: %d", frame_index);
            frame_index++;
        }

        //这里才是真正的写出去
        ret = av_interleaved_write_frame(outFmtCtx, &pkt);

        if (ret < 0) {
            LOGD("write frame failed");
            break;
        }

        //释放资源
        av_free_packet(&pkt);


    }
    //写结尾
    av_write_trailer(outFmtCtx);
    LOGD("Jareld:push stream success");
    avformat_free_context(inFmtCtx);
    avformat_free_context(outFmtCtx);

    env->ReleaseStringUTFChars(intput_, intput);
    env->ReleaseStringUTFChars(output_, output);

//    end:
//    avformat_free_context(inFmtCtx);
//    avformat_free_context(outFmtCtx);

}
//录像的时候 先设置输入和输出的参数
JNIEXPORT void JNICALL
Java_com_example_jareld_wfdcamera_1server_VideoPlayer_jni_1ffmpeg_1hecv_1set_1configuration(
        JNIEnv *env,
        jclass type,
        jstring intput_,
        jstring output_) {
    const char *intput = env->GetStringUTFChars(intput_, 0);
    const char *output = env->GetStringUTFChars(output_, 0);
    AVFormatContext *pFormatCtx;
    AVOutputFormat *fmt;
    AVStream *video_st;
    AVCodecContext *pCodecCtx;
    AVCodec *pCodec;

    uint8_t *picture_buf;
    AVFrame *picture;
    int size;

    //注册所有组件
    av_register_all();
    pFormatCtx = avformat_alloc_context();
    //猜输出的格式
    fmt = av_guess_format(NULL, output, NULL);
    pFormatCtx->oformat = fmt;

    if (avio_open(&pFormatCtx->pb, output, AVIO_FLAG_READ_WRITE) < 0) {
        LOGD("Failed to open output file! ");
        return;
    }


    env->ReleaseStringUTFChars(intput_, intput);
    env->ReleaseStringUTFChars(output_, output);
}
//录像过程中 buffer的写入
JNIEXPORT void JNICALL
Java_com_example_jareld_wfdcamera_1server_VideoPlayer_jni_1ffmpeg_1hecv_1set_1buffer(JNIEnv *env,
                                                                                     jclass type,
                                                                                     jintArray buffer_) {
    jint *buffer = env->GetIntArrayElements(buffer_, NULL);

    // TODO

    env->ReleaseIntArrayElements(buffer_, buffer, 0);
}

int flush_encoder(AVFormatContext *fmt_ctx, unsigned int stream_index) {
    int ret;
    int got_frame;
    AVPacket enc_pkt;
    if (!(fmt_ctx->streams[stream_index]->codec->codec->capabilities & CODEC_CAP_DELAY)) {
        return 0;
    }
    while (1) {
        LOGD("Flushing stream %d encoder\n", stream_index);
        enc_pkt.data = NULL;
        enc_pkt.size = 0;
        av_init_packet(&enc_pkt);
        ret = avcodec_encode_video2(fmt_ctx->streams[stream_index]->codec, &enc_pkt, NULL,
                                    &got_frame);
        av_frame_free(NULL);

        if (ret < 0) {
            break;
        }

        if (!got_frame) {
            ret = 0;
            break;
        }

        LOGD("encode video success, index:%d \n", stream_index);

        ret = av_write_frame(fmt_ctx, &enc_pkt);

        if (ret < 0) {
            break;
        }
    }
    return ret;
}
AVCodec *pCodec1;
AVCodecContext *pCodecCtx1 = NULL;
int i, ret, got_output;
FILE *fp_out;
AVFrame *pFrame;
AVPacket pkt;
int y_size;
int framecnt = 0;
char filename_out[] = "/storage/emulated/0/yourname.h264";
int in_w = 1920, in_h = 960;
int count = 0;
JNIEXPORT jint JNICALL
Java_com_example_jareld_wfdcamera_1server_VideoPlayer_jni_1ffmpeg_1setVersion(JNIEnv *env,
                                                                              jclass type) {

    // TODO
    LOGD("setVersion E");
    avcodec_register_all();

    pCodec1 = avcodec_find_encoder(AV_CODEC_ID_HEVC);
    if (!pCodec1) {
        LOGD("Codec not found\n");
        return -1;
    }
    pCodecCtx1 = avcodec_alloc_context3(pCodec1);
    if (!pCodecCtx1) {
        LOGD("Could not allocate video codec context\n");
        return -1;
    }
    pCodecCtx1->bit_rate = 400000;
    pCodecCtx1->width = in_h;
    pCodecCtx1->height = in_w;
    pCodecCtx1->time_base.num = 1;
    pCodecCtx1->time_base.den = 30;
    pCodecCtx1->gop_size = 10;
    pCodecCtx1->max_b_frames = 5;
    pCodecCtx1->pix_fmt = AV_PIX_FMT_YUV420P;

    av_opt_set(pCodecCtx1->priv_data, "preset", "superfast", 0);
//  av_opt_set(pCodecCtx->priv_data, "preset", "slow", 0);
    av_opt_set(pCodecCtx1->priv_data, "tune", "zerolatency", 0);


    if (avcodec_open2(pCodecCtx1, pCodec1, NULL) < 0) {
        LOGD("Could not open codec\n");
        return -1;
    }
    if ((fp_out = fopen(filename_out, "wb")) == NULL) {
        LOGD("fopen failure");
        return -1;
    }
    y_size = pCodecCtx1->width * pCodecCtx1->height;
    LOGD("setVersion exit! y_size: %d", y_size);
    return 1;

}


}