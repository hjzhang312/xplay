#ifndef XREADTHREAD_H
#define XREADTHREAD_H

#include <QThread>
#include "xffmpeg.h"

class XReadThread : public QThread
{
public:
    XReadThread();

    static XReadThread *Get()
    {
        static XReadThread t;
        return &t;
    }

    void run();
    void clearFrame();

public:
    bool mRead;
    QList<AVFrame*>  mFrameAudioList;
    QList<AVPacket> mPktVideoList;
};

#endif // XREADTHREAD_H
