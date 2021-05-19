#include "xplay.h"
#include <QApplication>

#include "xffmpeg.h"
#include <QDebug>
#include <QImage>


int main(int argc, char *argv[])
{
#if 0
    QApplication a(argc, argv);
    xplay *w = xplay::Get();
    w->show();

#endif
#if 1
    XFFmpeg::Get()->Open("daomeixong.mp4");



    for(;;)
    {
        char buff[800*600*4] = {0};
        AVPacket pkt = XFFmpeg::Get()->Read();

        if(pkt.size == 0)
        {
            qDebug() << "read end";
            break;
        }
        if(pkt.stream_index == XFFmpeg::Get()->mVideoStream)
        qDebug() << "Video pts: " << pkt.pts;

        if(pkt.stream_index == XFFmpeg::Get()->mAudioStream)
        qDebug() << "Audio pts: " << pkt.pts;

        XFFmpeg::Get()->Decode(&pkt);

        printf(" plant: %d \n",av_sample_fmt_is_planar(XFFmpeg::Get()->ic->streams[XFFmpeg::Get()->mAudioStream]->codec->sample_fmt));

        if(pkt.stream_index == XFFmpeg::Get()->mAudioStream)
        {
            if(NULL != XFFmpeg::Get()->mFrame->data[0])
            printf("data[0]:\n");
            if(NULL != XFFmpeg::Get()->mFrame->data[1])
            printf("data[1]:\n");
            if(NULL != XFFmpeg::Get()->mFrame->data[2])
            printf("data[2]:\n");
            if(NULL != XFFmpeg::Get()->mFrame->data[3])
            printf("data[3]:\n");
            if(NULL != XFFmpeg::Get()->mFrame->data[4])
            printf("data[4]:\n");

            printf("data[0] size:%d \n",XFFmpeg::Get()->mFrame->linesize[0]);
            printf("data[1] size:%d \n",XFFmpeg::Get()->mFrame->linesize[1]);
            printf("data[2] size:%d \n",XFFmpeg::Get()->mFrame->linesize[2]);
            printf("data[3] size:%d \n",XFFmpeg::Get()->mFrame->linesize[3]);
            printf("data[4] size:%d \n",XFFmpeg::Get()->mFrame->linesize[4]);
            XFFmpeg::Get()->ToRGB(buff,800,600);

            break;
        }
        av_packet_unref(&pkt);

    }
#endif
    //return a.exec();
}
