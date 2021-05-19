#include "videoplay.h"

#include "xreadthread.h"
#include "xplay.h"

#include <QDebug>

videoPlay::videoPlay()
{

}

void videoPlay::run()
{
    while(1)
    {
        if(XReadThread::Get()->mPktVideoList.isEmpty())
        {
            msleep(10);
            continue;
        }
        AVPacket pkt = XReadThread::Get()->mPktVideoList.first();


       // int pts = XFFmpeg::Get()->GetPts(&pkt);
       //qDebug() << "pts: " <<pts;
//qDebug() << "video pts: " << pts << "audio pts: " << XFFmpeg::Get()->pts << "size: "<<XReadThread::Get()->mPktVideoList.size() << "fps: " << XFFmpeg::Get()->fps;
//XFFmpeg::Get()->Decode(&pkt);
    if( XFFmpeg::Get()->pts > XFFmpeg::Get()->apts)
        {
            //qDebug() << "continue";
           // XReadThread::Get()->mPktVideoList.takeFirst();
            msleep(10);
            continue;
        }
        XFFmpeg::Get()->Decode(&pkt);
        XReadThread::Get()->mPktVideoList.takeFirst();
        av_packet_unref(&pkt);

        qDebug() << " vide play";
        xplay::Get()->showGlWindow();

#if 0
        if(XFFmpeg::Get()->fps > 0)
        {
            msleep((1000 / XFFmpeg::Get()->fps));
        }
#endif

    }

}

