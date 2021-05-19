#include "xffmpeg.h"

#include <QDebug>
#include <unistd.h>
#include "audiowidget.h"

static double r2d(AVRational r)
{
    return r.num == 0 || r.den == 0 ? 0. : (double)r.num / (double)r.den;
}

bool XFFmpeg::Open(const char *fileName)
{
    Close();

    bool retV = true;
    mutex.lock();

    // 打开视频文件
    // path参数表示打开的视频路径，这个路径可以包括各种视频文件
    // 也包括rtsp和http网络视频流
    // 使用rtsp或http网络视频流做数据源时，可以直接打开，代码不需要动
    // 第三个参数表示传入的视频格式，我这边不传递，由FFmpeg内部获取
    // 最后一个参数是设置，我们这里也不传递
    int ret = avformat_open_input(&ic,fileName,NULL,NULL);
    if(ret != 0)
    {
        av_strerror(ret,mErrorBuffer,sizeof(mErrorBuffer));


        qDebug() << "ERROR: " << mErrorBuffer;
        retV = false;
        mutex.unlock();
        return retV;
    }
    if(avformat_find_stream_info(ic,NULL) < 0)
    {
        retV = false;
        mutex.unlock();
        return retV;
    }
    mTotalTime = ic->duration / (int64_t)AV_TIME_BASE;
    //mTotalTime = mTotalTime.sprintf("%02d:%02d:%02d",totalSec/60/60,totalSec/60,totalSec%60);

    // 下面开始解封装
    // 解封装就是将视频文件里的视频流和音频流分别解析出来

    // 视频文件里一般包含视频流,音频流，可能会有字幕流

    // ic->nb_streams : 视频文件里包含流的数量
    // 遍历视频文件里所有的流
    for(int i = 0; i < ic->nb_streams; i++)
    {
        printf("name : %s log_name:%s \n",ic->iformat->name,ic->iformat->long_name);
        AVCodecContext *avCodecCtx = ic->streams[i]->codec;
        if(AVMEDIA_TYPE_VIDEO == avCodecCtx->codec_type)
        {
            mVideoStream = i;
            fps = r2d(ic->streams[i]->avg_frame_rate);
            AVCodec *avCode = avcodec_find_decoder(avCodecCtx->codec_id);
            // 打开视频解码器
            // 打开音频解码器用的也是同一个函数
            int err = avcodec_open2(avCodecCtx, avCode, NULL);
            if (err != 0) // 如果打开视频解码器失败
            {
                // 获取错误原因
                av_strerror(err,mErrorBuffer,sizeof(mErrorBuffer));


                qDebug() << "ERROR: " << mErrorBuffer;
                retV = false;
                mutex.unlock();
                return retV;
            }
        }
        else if(AVMEDIA_TYPE_AUDIO == avCodecCtx->codec_type)
        {
            qDebug() << "audio: " << ic->streams[i]->avg_frame_rate.den << ic->streams[i]->avg_frame_rate.num;
            mAudioStream = i;
            AVCodec *avCode = avcodec_find_decoder(avCodecCtx->codec_id);
            // 打开视频解码器
            // 打开音频解码器用的也是同一个函数
            int err = avcodec_open2(avCodecCtx, avCode, NULL);
            if (err != 0) // 如果打开视频解码器失败
            {
                // 获取错误原因
                av_strerror(err,mErrorBuffer,sizeof(mErrorBuffer));
                qDebug() << "ERROR: " << mErrorBuffer;
                retV = false;
                mutex.unlock();
                return retV;
            }
            AudioWidget::Get()->audioInit(avCodecCtx->sample_rate,avCodecCtx->channels);
        }
    }

    mutex.unlock();
    return retV;

}

void XFFmpeg::Close()
{
    mutex.lock();
    if(!ic)
    {
        avformat_close_input(&ic);
        ic = NULL;
    }
    if(!mFrame)
    {
        av_frame_free(&mFrame);
        mFrame = NULL;
    }
    mutex.unlock();
}

AVFrame *XFFmpeg::Decode(AVPacket *pkt)
{
    mutex.lock();
    if(!ic)
    {
        mutex.unlock();
        return NULL;
    }
    if(mFrame == NULL /*&& mVideoStream != -1*/) // 如果是第一次解码视频,且存在视频数据
    {
        mFrame = av_frame_alloc();
    }
    AVFrame *yuv = mFrame;



        // 发送未解码的流
    int ret = avcodec_send_packet(ic->streams[pkt->stream_index]->codec,pkt);
    if (ret != 0)
    {
        // 获取错误原因
        av_strerror(ret,mErrorBuffer,sizeof(mErrorBuffer));
        qDebug() << "ERROR: " << mErrorBuffer;
        mutex.unlock();
        return NULL;
    }

    ret = avcodec_receive_frame(ic->streams[pkt->stream_index]->codec,yuv);
    if (ret != 0)
    {
        // 获取错误原因
        av_strerror(ret,mErrorBuffer,sizeof(mErrorBuffer));
        qDebug() << "ERROR: " << mErrorBuffer;
        mutex.unlock();
        return NULL;
    }
    mutex.unlock();
   // qDebug() << "packet pts: " << pkt->pts;
   // qDebug() << "frame  pts: " << mFrame->pts;
   // qDebug() << "frame  pkt_pts: " << mFrame->pkt_pts;
    //qDebug() << "pict_type: " << (int)yuv->pict_type;
    return yuv;
}

bool XFFmpeg::send_packet_data(AVPacket *pkt)
{
    mutex.lock();
    if(!ic)
    {
        mutex.unlock();
        return false;
    }


        // 发送未解码的流
    int ret = avcodec_send_packet(ic->streams[pkt->stream_index]->codec,pkt);
    if (ret != 0)
    {
        // 获取错误原因
        av_strerror(ret,mErrorBuffer,sizeof(mErrorBuffer));
        printf("ERROR:%d %s %s\n ",__LINE__,__FUNCTION__, mErrorBuffer);
        mutex.unlock();
        return false;
    }
#if 0
    ret = avcodec_receive_frame(ic->streams[pkt->stream_index]->codec,pcm);
    if (ret != 0)
    {
        // 获取错误原因
        av_strerror(ret,mErrorBuffer,sizeof(mErrorBuffer));
        printf("ERROR:%d %s %s\n ",__LINE__,__FUNCTION__, mErrorBuffer);
        mutex.unlock();
        return NULL;
    }

    printf("pcm rate: %d\n",pcm->sample_rate);
#endif
    mutex.unlock();
    return true;
}

AVFrame *XFFmpeg::receiveFrame()
{
    mutex.lock();
    if(!ic)
    {
        mutex.unlock();
        return NULL;
    }

    AVFrame *pcm = av_frame_alloc();

    int ret = avcodec_receive_frame(ic->streams[mAudioStream]->codec,pcm);
    if (ret != 0)
    {
        // 获取错误原因
       // av_strerror(ret,mErrorBuffer,sizeof(mErrorBuffer));
       // printf("ERROR:%d %s %s\n ",__LINE__,__FUNCTION__, mErrorBuffer);
        mutex.unlock();
        return NULL;
    }
 //   printf("pcm rate: %d\n",pcm->sample_rate);
    mutex.unlock();
    return pcm;
}

AVPacket XFFmpeg::Read()
{
    mutex.lock();
    AVPacket pkt;
    memset(&pkt,0,sizeof(pkt));


    if (!ic) // 如果没有成功打开文件
    {
        mutex.unlock();
        return pkt;
    }

    // 读取码流中的音频若干帧或者视频一帧
    // 例如，解码视频的时候，每解码一个视频帧，需要先调用 av_read_frame() 获得一帧视频的压缩数据，
    // 然后才能对该数据进行解码（例如H.264中一帧压缩数据通常对应一个NAL）。
    int ret = av_read_frame(ic, &pkt);
    if (ret != 0) // 读到结尾或者出错
    {
        av_strerror(ret,mErrorBuffer,sizeof(mErrorBuffer));
        qDebug() << "ERROR: " << mErrorBuffer;

    }

   // qDebug() << "fenzi: " <<ic->streams[pkt.stream_index]->time_base.num  << ic->streams[pkt.stream_index]->time_base.den;

    mutex.unlock();
    return pkt;
}

// YUV格式视频数据转码为RGB格式，并缩放
// @para out       存放输出数据
// @para outwidth  输出宽度
// @para outheight 输出高度
bool XFFmpeg::ToRGB(char *out, uint outwidth, uint outheight)
{

    mutex.lock();

    // 如果没有成功打开视频文件或者没有成功的解码或者没有传入可用内存
    if((!ic) || (mFrame == NULL) || (mVideoStream == -1))
    {
        mutex.unlock();
        return false;
    }

    //qDebug()<<"datasize: " << *(mFrame->data[1]) << "linesize" << mFrame->linesize[1];

    AVCodecContext *avCodeCtx = ic->streams[mVideoStream]->codec;

    // 设置视频转码器
    // 参数1 : context 写NULL,会新创建一个转码器
    //                 写转码器,会先比较这个转化器是否和要设置的参数一样。一样，就不新建转码器。不一样就新建转码器。

    // 参数2 : srcW 视频源宽
    // 参数3 : srcH 视频源高
    // 参数4 : srcFormat 视频源格式

    // 参数5 : dstW 目标宽
    // 参数6 : dstH 目标高
    // 参数7 : dstFormat 目标格式

    // 参数8  : flags 转码所用的算法
    // 参数9  : srcFilter 源过滤器
    // 参数10 : dstFilter 目标过滤器
    // 参数11 : param
    mSwsCtx = sws_getCachedContext(mSwsCtx,avCodeCtx->width,avCodeCtx->height,avCodeCtx->pix_fmt,\
                                   outwidth,outheight,AV_PIX_FMT_BGRA,SWS_BICUBIC,NULL,NULL,NULL);

    if(!mSwsCtx)
    {
        mutex.unlock();
        printf("[XFFmpeg::ToRGB] sws_getCachedContext() failed!\n");
        return false;
    }


    // 运行到这里,说明上面成功打开并设置了转码器

    // 下面进行转码

    // 用于存放转码后得到的rgb数据
    uint8_t *data[AV_NUM_DATA_POINTERS];
    data[0] = (uint8_t *)out; // 输出空间指向 out

    // 一行所需字节数
    int linesize[AV_NUM_DATA_POINTERS] = { 0 };
    linesize[0] = outwidth * 4; // 一行有 outwidth 个像素,每个像素4个字节

    // sws_scale : 转码,返回转码后的高度
    int height = sws_scale(mSwsCtx,mFrame->data,mFrame->linesize,0,avCodeCtx->height,data,linesize);
    for(int i = 0; i< AV_NUM_DATA_POINTERS;i ++)
        printf("i = %d , linesize[%d]: %d\n",i,i,mFrame->linesize[i]);

     pts = mFrame->pts * r2d(ic->streams[mAudioStream]->time_base) *1000;
    //qDebug() << "height: " << height;
   // qDebug() << "pict_type: " << (int)mFrame->pict_type;
    mutex.unlock();

    return true;

}

int XFFmpeg::ToPCM(char *out,AVFrame *frame)
{
    mutex.lock();
    // 如果没有成功打开视频文件或者没有成功的解码或者没有传入可用内存
    if((!ic) || (mFrame == NULL) || (mAudioStream == -1))
    {
        mutex.unlock();
        return false;
    }

    //qDebug()<<"datasize: " << *(mFrame->data[1]) << "linesize" << mFrame->linesize[1];

    AVCodecContext *avCodeCtx = ic->streams[mAudioStream]->codec;

    if(NULL == mSwrCtx)
    {
        mSwrCtx = swr_alloc();
        mSwrCtx =  swr_alloc_set_opts(mSwrCtx, avCodeCtx->channel_layout,
                                      AV_SAMPLE_FMT_S16,
                                      avCodeCtx->sample_rate, avCodeCtx->channels,
                                      avCodeCtx->sample_fmt,
                                      avCodeCtx->sample_rate,
                                      0, 0
                                      );
        if(!mSwrCtx || (swr_init(mSwrCtx) < 0))
        {
            swr_free(&mSwrCtx);
            mSwrCtx = NULL;
            mutex.unlock();
            return false;
        }
    }
    /*计算转换后的sample个数*/
    /*转后后的sample个数的计算公式为：src_nb_samples * dst_sample_rate / src_sample_rate，其计算如下：*/
    int dst_nb_samples = av_rescale_rnd(swr_get_delay(mSwrCtx,frame->sample_rate) + frame->nb_samples,frame->sample_rate,frame->sample_rate,AVRounding(1));

    uint8_t *data[1];
    data[0] = (uint8_t*)out;
    // 重采样
    // swr_convert 返回的是转换后该通道(channel)的采样个数
    int nb = swr_convert(mSwrCtx, \
                        data, dst_nb_samples, \
                            (const uint8_t **)frame->data, frame->nb_samples);
    qDebug() << "dst_nb_samples:" << dst_nb_samples << "nb: " << nb;
   // return NULL;
    int outsize = av_samples_get_buffer_size(NULL, avCodeCtx->channels,\
        frame->nb_samples, AV_SAMPLE_FMT_S16, 0);

    // 重采样后音频数据字节数 = 转换的数据的字节数 = 通道数 * sample个数 * 每个sample的字节数

    int resampled_data_size = 2 * nb * av_get_bytes_per_sample(AV_SAMPLE_FMT_S16);
   // pts = frame->pts * r2d(ic->streams[mAudioStream]->time_base) *1000;
    mutex.unlock();
   // printf("chanle: %d, data_size:%d,outsize:%d \n",nb,resampled_data_size,outsize);
    return outsize;
}

int XFFmpeg::GetPts(AVFrame *pkt)
{
    mutex.lock();
    if(!ic)
    {
        mutex.unlock();
        return -1;
    }

    apts = pkt->pts * r2d(ic->streams[mAudioStream]->time_base) *1000;

    mutex.unlock();
    return pts;
}

bool XFFmpeg::seek(float pos)
{
    mutex.lock();

  //  printf("[XFFmpeg::ToPCM] seek 1 \n ");

    if ((!ic)) // 如果没有成功打开视频文件
    {
        mutex.unlock();
        return false;
    }

    //printf("[XFFmpeg::ToPCM] seek 2 \n ");

    int StreamIndex = -1;

    if (mAudioStream != -1)
    {
        StreamIndex = mAudioStream;
    }

    if (mVideoStream != -1)
    {
        StreamIndex = mVideoStream;
    }


    // 用于存放要移动到的数据流位置的时间戳
    int64_t stamp = 0;
    stamp = pos * ic->streams[StreamIndex]->duration; // 计算出时间戳

    // AVSEEK_FLAG_BACKWARD : 往后找
    // AVSEEK_FLAG_FRAME    : 关键帧位置

    // 设置如果指定的时间戳位置不是关键帧,那么就往后找关键帧
    // 如果你不取到关键帧的话，视频是没法解码的
    int re = av_seek_frame(ic, StreamIndex, stamp, AVSEEK_FLAG_BACKWARD | AVSEEK_FLAG_FRAME);
    qDebug() << "re: " << re;
    // 冲掉 seek 之前解码缓冲中的数据
    // ic->streams[videoStream]->codec : 解码的缓冲都存在这里
    avcodec_flush_buffers(ic->streams[StreamIndex]->codec);

    // 解决暂停播放时,拖动进度条,进度条不变的BUG
    // stamp : 要移动到的视频位置的 pts
    // 计算出要移动到的视频时间,单位 ms
    this->pts = stamp * r2d(ic->streams[StreamIndex]->time_base) * 1000; // 计算转化为单位为 毫秒 的时间数据
    qDebug() << "pts: " << pts;
    // 释放锁
    mutex.unlock();

    if (re >= 0)
    {
        return true;
    }

    return false;
}

std::string XFFmpeg::GetError()
{
    mutex.lock();

    std::string ret = this->mErrorBuffer;
    mutex.unlock();

    return ret;
}

XFFmpeg::XFFmpeg()
{
    ic = NULL;
    mFrame = NULL;
    mSwsCtx = NULL;
    mSwrCtx = NULL;
    mVideoStream = -1;
    mAudioStream = -1;
    av_register_all();
    memset(mErrorBuffer,0,sizeof(mErrorBuffer));

}

