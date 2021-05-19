#ifndef AUDIOPLAY_H
#define AUDIOPLAY_H

#include <QThread>
class AudioPlay : public QThread
{
public:
    AudioPlay();

    static AudioPlay *Get()
    {
        static AudioPlay vt;
        return &vt;
    }

    void run();
    int apts;
};

#endif // AUDIOPLAY_H
