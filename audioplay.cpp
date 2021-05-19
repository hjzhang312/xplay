#include "audioplay.h"
#include "xreadthread.h"
#include "audiowidget.h"
#include <QDebug>

AudioPlay::AudioPlay()
{

}

void AudioPlay::run()
{
    while(1)
    {
        if(XReadThread::Get()->mFrameAudioList.isEmpty())
        {
            msleep(10);
            continue;
        }
        AVFrame *frame = XReadThread::Get()->mFrameAudioList.first();


        //int pts = XFFmpeg::Get()->GetPts(&pkt);
        //qDebug() << "pts: " <<pts;
        /*
        if(pts > 0)
        {
            msleep(10);
            continue;
        }*/


        //qDebug() << "mPktAudioList: " << XReadThread::Get()->mPktAudioList.size() << AudioWidget::Get()->getBufFree();

        //xplay::Get()->showGlWindow();
        char out[10000] = {0};
        int size = XFFmpeg::Get()->ToPCM(out,frame);
        if(AudioWidget::Get()->getBufFree() < size)
        {
           // qDebug() << "buffer free";
            msleep(10);
            continue;
        }
        XFFmpeg::Get()->GetPts(frame);
        bool ret = AudioWidget::Get()->WriteData(out,size);
        if(!ret)
        {
            qDebug() << "write error";
            msleep(1);
            continue;
        }

        XReadThread::Get()->mFrameAudioList.takeFirst();
        av_frame_free(&frame);
        //int sleep_time = 0.0054 *size  + 0.5;
        //msleep( sleep_time );
       // qDebug() << "aduio pcm write";
        //if (XFFmpeg::Get()->fps > 0)
        {

        }


    }

}

