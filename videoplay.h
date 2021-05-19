#ifndef VIDEOPLAY_H
#define VIDEOPLAY_H

#include <QThread>
class videoPlay : public QThread
{
public:
    videoPlay();

    static videoPlay *Get()
    {
        static videoPlay vt;
        return &vt;
    }

    void run();

};

#endif // VIDEOPLAY_H
