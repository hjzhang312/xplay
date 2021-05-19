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

    // ����Ƶ�ļ�
    // path������ʾ�򿪵���Ƶ·�������·�����԰���������Ƶ�ļ�
    // Ҳ����rtsp��http������Ƶ��
    // ʹ��rtsp��http������Ƶ��������Դʱ������ֱ�Ӵ򿪣����벻��Ҫ��
    // ������������ʾ�������Ƶ��ʽ������߲����ݣ���FFmpeg�ڲ���ȡ
    // ���һ�����������ã���������Ҳ������
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

    // ���濪ʼ���װ
    // ���װ���ǽ���Ƶ�ļ������Ƶ������Ƶ���ֱ��������

    // ��Ƶ�ļ���һ�������Ƶ��,��Ƶ�������ܻ�����Ļ��

    // ic->nb_streams : ��Ƶ�ļ��������������
    // ������Ƶ�ļ������е���
    for(int i = 0; i < ic->nb_streams; i++)
    {
        printf("name : %s log_name:%s \n",ic->iformat->name,ic->iformat->long_name);
        AVCodecContext *avCodecCtx = ic->streams[i]->codec;
        if(AVMEDIA_TYPE_VIDEO == avCodecCtx->codec_type)
        {
            mVideoStream = i;
            fps = r2d(ic->streams[i]->avg_frame_rate);
            AVCodec *avCode = avcodec_find_decoder(avCodecCtx->codec_id);
            // ����Ƶ������
            // ����Ƶ�������õ�Ҳ��ͬһ������
            int err = avcodec_open2(avCodecCtx, avCode, NULL);
            if (err != 0) // �������Ƶ������ʧ��
            {
                // ��ȡ����ԭ��
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
            // ����Ƶ������
            // ����Ƶ�������õ�Ҳ��ͬһ������
            int err = avcodec_open2(avCodecCtx, avCode, NULL);
            if (err != 0) // �������Ƶ������ʧ��
            {
                // ��ȡ����ԭ��
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
    if(mFrame == NULL /*&& mVideoStream != -1*/) // ����ǵ�һ�ν�����Ƶ,�Ҵ�����Ƶ����
    {
        mFrame = av_frame_alloc();
    }
    AVFrame *yuv = mFrame;



        // ����δ�������
    int ret = avcodec_send_packet(ic->streams[pkt->stream_index]->codec,pkt);
    if (ret != 0)
    {
        // ��ȡ����ԭ��
        av_strerror(ret,mErrorBuffer,sizeof(mErrorBuffer));
        qDebug() << "ERROR: " << mErrorBuffer;
        mutex.unlock();
        return NULL;
    }

    ret = avcodec_receive_frame(ic->streams[pkt->stream_index]->codec,yuv);
    if (ret != 0)
    {
        // ��ȡ����ԭ��
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


        // ����δ�������
    int ret = avcodec_send_packet(ic->streams[pkt->stream_index]->codec,pkt);
    if (ret != 0)
    {
        // ��ȡ����ԭ��
        av_strerror(ret,mErrorBuffer,sizeof(mErrorBuffer));
        printf("ERROR:%d %s %s\n ",__LINE__,__FUNCTION__, mErrorBuffer);
        mutex.unlock();
        return false;
    }
#if 0
    ret = avcodec_receive_frame(ic->streams[pkt->stream_index]->codec,pcm);
    if (ret != 0)
    {
        // ��ȡ����ԭ��
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
        // ��ȡ����ԭ��
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


    if (!ic) // ���û�гɹ����ļ�
    {
        mutex.unlock();
        return pkt;
    }

    // ��ȡ�����е���Ƶ����֡������Ƶһ֡
    // ���磬������Ƶ��ʱ��ÿ����һ����Ƶ֡����Ҫ�ȵ��� av_read_frame() ���һ֡��Ƶ��ѹ�����ݣ�
    // Ȼ����ܶԸ����ݽ��н��루����H.264��һ֡ѹ������ͨ����Ӧһ��NAL����
    int ret = av_read_frame(ic, &pkt);
    if (ret != 0) // ������β���߳���
    {
        av_strerror(ret,mErrorBuffer,sizeof(mErrorBuffer));
        qDebug() << "ERROR: " << mErrorBuffer;

    }

   // qDebug() << "fenzi: " <<ic->streams[pkt.stream_index]->time_base.num  << ic->streams[pkt.stream_index]->time_base.den;

    mutex.unlock();
    return pkt;
}

// YUV��ʽ��Ƶ����ת��ΪRGB��ʽ��������
// @para out       ����������
// @para outwidth  ������
// @para outheight ����߶�
bool XFFmpeg::ToRGB(char *out, uint outwidth, uint outheight)
{

    mutex.lock();

    // ���û�гɹ�����Ƶ�ļ�����û�гɹ��Ľ������û�д�������ڴ�
    if((!ic) || (mFrame == NULL) || (mVideoStream == -1))
    {
        mutex.unlock();
        return false;
    }

    //qDebug()<<"datasize: " << *(mFrame->data[1]) << "linesize" << mFrame->linesize[1];

    AVCodecContext *avCodeCtx = ic->streams[mVideoStream]->codec;

    // ������Ƶת����
    // ����1 : context дNULL,���´���һ��ת����
    //                 дת����,���ȱȽ����ת�����Ƿ��Ҫ���õĲ���һ����һ�����Ͳ��½�ת��������һ�����½�ת������

    // ����2 : srcW ��ƵԴ��
    // ����3 : srcH ��ƵԴ��
    // ����4 : srcFormat ��ƵԴ��ʽ

    // ����5 : dstW Ŀ���
    // ����6 : dstH Ŀ���
    // ����7 : dstFormat Ŀ���ʽ

    // ����8  : flags ת�����õ��㷨
    // ����9  : srcFilter Դ������
    // ����10 : dstFilter Ŀ�������
    // ����11 : param
    mSwsCtx = sws_getCachedContext(mSwsCtx,avCodeCtx->width,avCodeCtx->height,avCodeCtx->pix_fmt,\
                                   outwidth,outheight,AV_PIX_FMT_BGRA,SWS_BICUBIC,NULL,NULL,NULL);

    if(!mSwsCtx)
    {
        mutex.unlock();
        printf("[XFFmpeg::ToRGB] sws_getCachedContext() failed!\n");
        return false;
    }


    // ���е�����,˵������ɹ��򿪲�������ת����

    // �������ת��

    // ���ڴ��ת���õ���rgb����
    uint8_t *data[AV_NUM_DATA_POINTERS];
    data[0] = (uint8_t *)out; // ����ռ�ָ�� out

    // һ�������ֽ���
    int linesize[AV_NUM_DATA_POINTERS] = { 0 };
    linesize[0] = outwidth * 4; // һ���� outwidth ������,ÿ������4���ֽ�

    // sws_scale : ת��,����ת���ĸ߶�
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
    // ���û�гɹ�����Ƶ�ļ�����û�гɹ��Ľ������û�д�������ڴ�
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
    /*����ת�����sample����*/
    /*ת����sample�����ļ��㹫ʽΪ��src_nb_samples * dst_sample_rate / src_sample_rate����������£�*/
    int dst_nb_samples = av_rescale_rnd(swr_get_delay(mSwrCtx,frame->sample_rate) + frame->nb_samples,frame->sample_rate,frame->sample_rate,AVRounding(1));

    uint8_t *data[1];
    data[0] = (uint8_t*)out;
    // �ز���
    // swr_convert ���ص���ת�����ͨ��(channel)�Ĳ�������
    int nb = swr_convert(mSwrCtx, \
                        data, dst_nb_samples, \
                            (const uint8_t **)frame->data, frame->nb_samples);
    qDebug() << "dst_nb_samples:" << dst_nb_samples << "nb: " << nb;
   // return NULL;
    int outsize = av_samples_get_buffer_size(NULL, avCodeCtx->channels,\
        frame->nb_samples, AV_SAMPLE_FMT_S16, 0);

    // �ز�������Ƶ�����ֽ��� = ת�������ݵ��ֽ��� = ͨ���� * sample���� * ÿ��sample���ֽ���

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

    if ((!ic)) // ���û�гɹ�����Ƶ�ļ�
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


    // ���ڴ��Ҫ�ƶ�����������λ�õ�ʱ���
    int64_t stamp = 0;
    stamp = pos * ic->streams[StreamIndex]->duration; // �����ʱ���

    // AVSEEK_FLAG_BACKWARD : ������
    // AVSEEK_FLAG_FRAME    : �ؼ�֡λ��

    // �������ָ����ʱ���λ�ò��ǹؼ�֡,��ô�������ҹؼ�֡
    // ����㲻ȡ���ؼ�֡�Ļ�����Ƶ��û�������
    int re = av_seek_frame(ic, StreamIndex, stamp, AVSEEK_FLAG_BACKWARD | AVSEEK_FLAG_FRAME);
    qDebug() << "re: " << re;
    // ��� seek ֮ǰ���뻺���е�����
    // ic->streams[videoStream]->codec : ����Ļ��嶼��������
    avcodec_flush_buffers(ic->streams[StreamIndex]->codec);

    // �����ͣ����ʱ,�϶�������,�����������BUG
    // stamp : Ҫ�ƶ�������Ƶλ�õ� pts
    // �����Ҫ�ƶ�������Ƶʱ��,��λ ms
    this->pts = stamp * r2d(ic->streams[StreamIndex]->time_base) * 1000; // ����ת��Ϊ��λΪ ���� ��ʱ������
    qDebug() << "pts: " << pts;
    // �ͷ���
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

