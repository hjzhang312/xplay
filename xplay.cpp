#include "xplay.h"
#include "ui_xplay.h"

#include <QFileDialog>
#include <unistd.h>


#ifdef __cplusplus             //���߱��������ⲿ�ִ��밴C���Եĸ�ʽ���б��룬������C++��
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
// ��������
// ��ֹ������Ϊ0
static double r2d(AVRational r)
{
    // �������,����������һ����0,��ֱ�ӷ���0
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
        // ��ȡ������Ϣ,����errorbuf
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
            if (!codec)// ���û���ҵ������õĽ�����
            {
                qDebug()<< "[XFFmpeg::Open] video code not find!\n";
            }

            qDebug() << "[XFFmpeg::Open] video code name      :"<< codec->name;
            qDebug()<<"[XFFmpeg::Open] video code long_name : "<< codec->long_name;

            // ����Ƶ������
            // ����Ƶ�������õ�Ҳ��ͬһ������
            int err = avcodec_open2(codeContext, codec, NULL);
            if (err != 0) // �������Ƶ������ʧ��
            {
                // ��ȡ����ԭ��
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

                    // ��ȡ����ԭ��
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
            if (!codec)// ���û���ҵ������õĽ�����
            {
                qDebug()<< "[XFFmpeg::Open] video code not find!\n";
            }

            qDebug() << "[XFFmpeg::Open] video code name      :"<< codec->name;
            qDebug()<<"[XFFmpeg::Open] video code long_name : "<< codec->long_name;

            // ����Ƶ������
            // ����Ƶ�������õ�Ҳ��ͬһ������
            int err = avcodec_open2(codeContext, codec, NULL);
            if (err != 0) // �������Ƶ������ʧ��
            {
                // ��ȡ����ԭ��
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
    QString name = QFileDialog::getOpenFileName(this,QString::fromLocal8Bit("ѡ����Ƶ�ļ�"));

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
    if (XFFmpeg::Get()->mVideoStream != -1) // ���������Ƶ��
    {
        // �����ͣ״̬,�϶�������֮��,���治ˢ������
        // �϶�������֮��,����һ֡��Ƶ֡����
        int decode = 0;
        while (decode == 0)
        {
            // ��ȡ��Ƶ�ļ����һ֡����
            // ��ȷ������Ƶ֡,������Ƶ֡,������Ļ֡
            AVPacket pkt = XFFmpeg::Get()->Read();

            if (pkt.size == 0) // ���û�л��һ֡����
            {
                continue;
            }

            // ���pkt������һ֡���ݲ�����Ƶ����
            if (pkt.stream_index != XFFmpeg::Get()->mVideoStream)
            {
                // �ͷ���һ֡������ռ���ڴ�
                av_packet_unref(&pkt);
                continue;
            }

            // ��ӡ������ʾ��ʱ���
            printf("[XPlay::sliderRelease] pts = %lld\n", pkt.pts);
            printf("\n");

            // �������е�����,pkt����һ����һ֡��Ƶ����

            // ����
            XFFmpeg::Get()->Decode(&pkt);

            // ����ΪYUV��ʽ֮����Բ�����ת��ΪRGB
            // ��Ϊ��Щ���ϣ���Ƶ��֡���ܸߣ�������ʾ��ʱ�򣬲���Ҫ�ܸߵ�֡��

            // �ͷ���һ֡������ռ���ڴ�
            av_packet_unref(&pkt);

            // ���е�����,˵���Ѿ�������һ֡��Ƶ����
            // decode �� 1 ��ʹ���ٽ�����Ƶ����֡
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


