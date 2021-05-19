#include "xplay.h"
#include "ui_xplay.h"

#include <QFileDialog>
#include <unistd.h>


#ifdef __cplusplus             //告诉编译器，这部分代码按C语言的格式进行编译，而不是C++的
extern "C"{
#endif

#include "avcodec.h"
#include "avformat.h"
#include "swscale.h"
#include "avutil.h"

#ifdef __cplusplus
}
#endif

#include "xffmpeg.h"
#include "xreadthread.h"
#include "videoplay.h"
#include "audioplay.h"
// 除法运算
// 防止被除数为0
static double r2d(AVRational r)
{
    // 如果除数,被除数中有一个是0,就直接返回0
    return r.num == 0 || r.den == 0 ? 0. : (double)r.num / (double)r.den;
}

#include <QDebug>
xplay::xplay(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::xplay)
{
    ui->setupUi(this);
    ui->totaltime->setStyleSheet("color:rgb(255,255,255)");
    ui->sp->setStyleSheet("color:rgb(255,255,255)");
    ui->curtime->setStyleSheet("color:rgb(255,255,255)");

    connect(ui->openBtn,SIGNAL(clicked(bool)),this,SLOT(btnSlots()));
    connect(ui->horizontalSlider,SIGNAL(sliderPressed()),this,SLOT(mysliderRelsease()));
}

void xplay::openAVFile(const char *path)
{
    AVFormatContext *out;
    out = NULL;

    out = avformat_alloc_context();
    av_register_all();


   //
    int ret = avformat_open_input(&out,path,NULL,NULL);


    if(ret == 0)
    {
        int totolSec = out->duration / (int64_t)AV_TIME_BASE;

        qDebug()<<"time: "<<totolSec/60<< ":" <<totolSec%60;
    }
    else
    {
        char errorbuf[1024];
        // 获取错误信息,存入errorbuf
        av_strerror(ret, errorbuf, sizeof(errorbuf));
    }
    if(avformat_find_stream_info(out,NULL) >= 0)
    {
       // qDebug() << "success";
    }
    // Dump valid information onto standard error
  // av_dump_format(out, 0, path, false);

    for(uint i = 0; i < out->nb_streams; i++)
    {
        AVCodecContext *codeContext = out->streams[i]->codec;
        qDebug() << "code type: " <<codeContext->codec_type << r2d(out->streams[i]->time_base);


        if(codeContext->codec_type == AVMEDIA_TYPE_VIDEO)
        {
            qDebug() << "rate: "<<out->streams[i]->avg_frame_rate.num << out->streams[i]->avg_frame_rate.den;
            AVCodec *codec = avcodec_find_decoder(codeContext->codec_id);
            if (!codec)// 如果没有找到可以用的解码器
            {
                qDebug()<< "[XFFmpeg::Open] video code not find!\n";
            }

            qDebug() << "[XFFmpeg::Open] video code name      :"<< codec->name;
            qDebug()<<"[XFFmpeg::Open] video code long_name : "<< codec->long_name;

            // 打开视频解码器
            // 打开音频解码器用的也是同一个函数
            int err = avcodec_open2(codeContext, codec, NULL);
            if (err != 0) // 如果打开视频解码器失败
            {
                // 获取错误原因
                char buf[1024] = { 0 };
                av_strerror(err, buf, sizeof(buf));

                printf("[XFFmpeg::Open]\n");
                printf(buf);
            }
          //  sws_getCachedContext()
           // avcodec_decode_video2();
            //avcodec_send_packet
/*
            AVPacket *ptk =  n     ();
            while(1)
            {
                int ret = av_read_frame(out,ptk);
                if(ret < 0)
                {

                    // 获取错误原因
                    char buf[1024] = { 0 };
                    av_strerror(err, buf, sizeof(buf));

                    printf(buf);
                    break;
                }

                qDebug() << "ptk->stream_index: " << ptk->stream_index;
                qDebug() << "ptk->pts: " << ptk->pts ;//* r2d(out->streams[ptk->stream_index]->time_base) ;
                qDebug() << "ptk->dts: " << ptk->  dts * r2d(out->streams[ptk->stream_index]->time_base) ;
                av_packet_unref(ptk);
            }
              av_packet_free(&ptk);
*/
        }
        else if (codeContext->codec_type == AVMEDIA_TYPE_AUDIO)
        {

          //  qDebug() << "rate: "<<out->streams[i]->avg_frame_rate.num << out->streams[i]->avg_frame_rate.den;
            qDebug() << "sample: " << codeContext->sample_rate;
            qDebug() << "sample_fmt: " << codeContext->sample_fmt;
            qDebug() << "sampleSize: " <<av_get_bytes_per_sample(codeContext->sample_fmt) * 8;
            qDebug() << "channels: " << codeContext->channels;
            qDebug() << "frame_size: " << codeContext->frame_size;
            qDebug() << "frame_number: " << codeContext->frame_number;
            AVCodec *codec = avcodec_find_decoder(codeContext->codec_id);
            if (!codec)// 如果没有找到可以用的解码器
            {
                qDebug()<< "[XFFmpeg::Open] video code not find!\n";
            }

            qDebug() << "[XFFmpeg::Open] video code name      :"<< codec->name;
            qDebug()<<"[XFFmpeg::Open] video code long_name : "<< codec->long_name;

            // 打开视频解码器
            // 打开音频解码器用的也是同一个函数
            int err = avcodec_open2(codeContext, codec, NULL);
            if (err != 0) // 如果打开视频解码器失败
            {
                // 获取错误原因
                char buf[1024] = { 0 };
                av_strerror(err, buf, sizeof(buf));

                printf("[XFFmpeg::Open]\n");
                printf(buf);

            }

        }


    }





    avformat_close_input(&out);
    //<< "duation: " << out->duration <<":" << out->duration%60;
}

void xplay::showGlWindow()
{
    //qDebug() <<"showGlWindow";

    QString time;
    int value = XFFmpeg::Get()->apts/1000;
    time = time.sprintf("%02d:%02d:%02d",value/60/60,value/60,value%60);

    ui->horizontalSlider->setValue(value*1000);
    ui->openGLWidget->showImage();
    ui->curtime->setText(time);
}

xplay::~xplay()
{
    delete ui;
}

void xplay::btnSlots()
{
    QString name = QFileDialog::getOpenFileName(this,QString::fromLocal8Bit("选择视频文件"));

    XFFmpeg::Get()->Open(name.toStdString().c_str());
    uint totalSec = XFFmpeg::Get()->mTotalTime;
    qDebug() << "totl time: " << totalSec*1000;
    QString mTime ;
    mTime = mTime.sprintf("%02d:%02d:%02d",totalSec/60/60,totalSec/60,totalSec%60);
    ui->horizontalSlider->setMinimum(0);
    ui->horizontalSlider->setMaximum(totalSec*1000);
    ui->horizontalSlider->setValue(0);
    ui->totaltime->setText(mTime);
    XReadThread::Get()->start();
    videoPlay::Get()->start();
    AudioPlay::Get()->start();


}

void xplay::mysliderRelsease()
{
    float pos = 0;
    pos = (float)ui->horizontalSlider->value() / (float)(ui->horizontalSlider->maximum());
    qDebug() << "value : " << ui->horizontalSlider->value() << "pos: " << pos;
    XFFmpeg::Get()->seek(pos);
    XReadThread::Get()->clearFrame();

#if 1
    if (XFFmpeg::Get()->mVideoStream != -1) // 如果存在视频流
    {
        // 解决暂停状态,拖动滑动条之后,画面不刷新问题
        // 拖动滑动条之后,解码一帧视频帧数据
        int decode = 0;
        while (decode == 0)
        {
            // 获取视频文件里的一帧数据
            // 不确定是视频帧,还是音频帧,还是字幕帧
            AVPacket pkt = XFFmpeg::Get()->Read();

            if (pkt.size == 0) // 如果没有获得一帧数据
            {
                continue;
            }

            // 如果pkt所含的一帧数据不是视频数据
            if (pkt.stream_index != XFFmpeg::Get()->mVideoStream)
            {
                // 释放这一帧数据所占的内存
                av_packet_unref(&pkt);
                continue;
            }

            // 打印用于显示的时间戳
            printf("[XPlay::sliderRelease] pts = %lld\n", pkt.pts);
            printf("\n");

            // 程序运行到这里,pkt里存的一定是一帧视频数据

            // 解码
            XFFmpeg::Get()->Decode(&pkt);

            // 解码为YUV格式之后可以不立刻转码为RGB
            // 因为有些场合，视频的帧数很高，但是显示的时候，不需要很高的帧数

            // 释放这一帧数据所占的内存
            av_packet_unref(&pkt);

            // 运行到这里,说明已经解码了一帧视频数据
            // decode 置 1 ，使不再解码视频数据帧
            decode = 1;
        }
    }
#endif
    //sleep(10);
    showGlWindow();
    XReadThread::Get()->mRead = false;
}

void xplay::resizeEvent(QResizeEvent *e)
{
    qDebug() << "resize";
    ui->openGLWidget->resize(this->size());

}


