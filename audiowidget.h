#ifndef AUDIOWIDGET_H
#define AUDIOWIDGET_H

#include <QAudioOutput>
#include <QIODevice>
#include <QMutex>
class AudioWidget
{
public:
    AudioWidget();

    static AudioWidget *Get()
    {
        static AudioWidget vt;
        return &vt;
    }
    void audioInit(int rate,int channels);
    bool WriteData(char *data,int size);
    int getBufFree();
    int getBufSize();

public:
    QAudioOutput *out;
    QIODevice *in;

private:
    QMutex mutex;

};

#endif // AUDIOWIDGET_H
