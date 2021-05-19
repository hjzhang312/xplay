#include "xreadthread.h"

#include <QDebug>
XReadThread::XReadThread()
{
    mRead = false;
}

void XReadThread::run()
{
    mFrameAudioList.clear();
    mPktVideoList.clear();
    while(1)
    {
        if(mRead || mFrameAudioList.size() > 3)
        {
            msleep(10);
           // qDebug() << "sleep";
            continue;
        }
        AVPacket pkt = XFFmpeg::Get()->Read();
        if(pkt.size <= 0)
        {
           // break;
            msleep(10);
            continue;
        }

   // qDebug() << "audio :" << mFrameAudioList.size() <<"video size: " << mPktVideoList.size();
        if(pkt.stream_index == XFFmpeg::Get()->mAudioStream)
        {
            AVFrame *frame = NULL;
            XFFmpeg::Get()->send_packet_data(&pkt);
            av_packet_unref(&pkt);
            while((frame =XFFmpeg::Get()->receiveFrame()) != NULL)
            {
                mFrameAudioList.append(frame);
            }

        }
        else if(pkt.stream_index == XFFmpeg::Get()->mVideoStream)
        {
            mPktVideoList.append(pkt);
        }
        else
        {
            av_packet_unref(&pkt);
        }

    }

}

void XReadThread::clearFrame()
{
    // mRead = true;
    for(int i; i < mPktVideoList.size();i++)
    {
        AVPacket pkt = mPktVideoList.at(i);
        av_packet_unref(&pkt);
    }
    mPktVideoList.clear();
    mFrameAudioList.clear();

}

